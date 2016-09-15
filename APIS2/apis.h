// APIS Washer board stupid header by (R)soft 2015
// APIS Errors:
// Error1 - Датчик температуры оборван (значение АЦП 0x000)
// Error2 - Датчик температуры закорочен (значние АЦП 0x3FF)
// Error3 - Датчик уровня неподключен или в обрыве (LEVEL1=LEVEL2=LEVEL3=1)
// Error4 - Неопознанная команда программы или сбой программы.
// Error9 - Неполадки с центральным контроллером или оборвана/закорочена шина I2C

//---------------------------------------------
// Датчик уровня (давления):
// Расположение контактов - 12, 14, 11, 16
// Контакт 11 COM - общий.
// Контакт 12 LEVEL1 - бак пустой.
// Контакт 14 LEVEL2 - бак заполнен (уровень регулируется винтом на датчике сразу под разъемом).
//          Этот уровень переключается с LEVEL1 на LEVEL2 с характерным щелчком.
// Контакт 16 LEVEL3 - бак переполнен (уровень регулируется винтом на датчике (средний)).
//          Этот уровень включается, замыкаясь с общим, и без щелчка.
//          Когда бак переполнен, то должно быть LEVEL1=1, LEVEL2=0, LEVEL3=0


#define SECRET_CODE 0xFA    // Секретный код для включения сервисного режима

// 0x4B - LINE OFF
// 0x10 - LINE ON (показания прыгают, дрыгают и просто рыгают;)
#define LINE_ON_TRESHOLD  (0x40) // Порог срабатывания АЦП блокировки люка

#define APIS_QUARTZ       (8000000L)
#define I2C_TIMER_CONST   (100)       // 50 ms

//---------------------------------------------------------------
// Основные управляющие выходы

//#define XTAL2    PORTB_Bit7
//#define XTAL1    PORTB_Bit6
#define SCK       PORTB_Bit5
#define MISO      PORTB_Bit4
#define MOSI      PORTB_Bit3
#define ROTOR     PORTB_Bit2  // Управление симистором мотора
#define REVERSE   PORTB_Bit1  // Управление статором (реле 2)
#define FORWARD   PORTB_Bit0  // Управление статором (реле 1)

#define LOCK      PORTC_Bit0  // Управление замком люка
#define BUZZER    PORTC_Bit1
#define RESERVED  PORTC_Bit2
// PC3 for input (LEVEL3)
// PC4 - SDA I2C
// PC5 - SCL I2C
// PC6 - ADC6 (LINE)
// PC7 - ADC7 (Termo)

#define WATER2     PORTD_Bit7   // Управление клапаном залива воды (основная стирка)
#define WATER1     PORTD_Bit6   // Управление клапаном залива воды (предварительная стирка)
#define POMP       PORTD_Bit5   // Управление помпой
#define HEATING    PORTD_Bit4   // Нагрев
// PD3(INT1) for input - ZeroCrossDetect
// PD2 (INT0) for input - IR
// PD1 (LEVEL2) for input
// PD0 (LEVEL1) for input


//------------------------------------
// Входы (датчик уровня)

#define LEVEL1  PIND_Bit0
#define LEVEL2  PIND_Bit1
#define LEVEL3  PINC_Bit3

//===================================================================
void InitPorts (void)
{
  //----------------- Port B
  DDRB=0xFF;
  PORTB=0x04; // PB2 должен быть изначально 1 (ROTOR TRIAC)
  
  //----------------- Port C
  DDRC=0xF7;  // PC3 (LEVEL3) for input
  PORTC=0x09; // pullup for PC3, PC0 должен быть изначально 1 (LOCK TRIAC)
  
  //----------------- Port D
  // PD0 (LEVEL1) for input
  // PD1 (LEVEL2) for input
  // PD2 (INT0) for input - IR
  // PD3(INT1) for input - ZeroCrossDetect
  DDRD=0xF0;  // LEVEL1 & LEVEL2 & IR & ZCD for input
  PORTD=0xEF; // IR & ZCD input pullup, PD5 & PD6 & PD7 должны быть изначально 1 (POMP & WATER1 & WATER2 TRIACs)
}

// глобальные переменные
unsigned char Timer_I2C;    // Таймер опроса I2C
unsigned char Timer_Buzzer; // Таймер писка
unsigned char SecretCode;   // Значение секретного кода для управления режимами машинки
unsigned char SecretMode; // При нажатии секретной кнопки, устанавливается секретный режим, позволяющий ввести секретный код

//-------- Основные регистры состояния электроавтоматики машинки ------------------
unsigned char HeatingState = 0; // Heating Relay state
unsigned char RotorState = 0;   // Rotor triac 0..0xFF state
unsigned char ForwardState = 0; // Forward Relay State
unsigned char ReverseState = 0; // Reverse Relay State
unsigned char PompState = 0;    // Pomp triac state
unsigned char WaterState1 = 0;  // Water1 triac state (предварительная стирка)
unsigned char WaterState2 = 0;  // Water2 triac state (основная стирка)
unsigned char LockState = 0;    // Lock triac state

//------- Вспомогательные режимы -----------------------------------
unsigned char ClockMode = 0;  // Режим показания часов минуты/часы
unsigned char TempMode = 0;   // Режим показания температуры градусы/АЦП
unsigned char SpinMode = 0;   // Режим показания устанавливаемых оборотов

unsigned int adcLine;                   // АЦП наличия фазы (LINE) после включения замка блокировки
unsigned int adcTemperature;            // Значение АЦП реальной Температуры
unsigned char temperature;              // Значение реальной температуры в градусах
volatile unsigned char SetTemperature;  // Значение установленной температуры для нагрева воды
volatile unsigned char SetSpin;         // Значение установленной частоты вращения для отжима
volatile unsigned char SpinRpm;         // Значение частоты вращения для PWM (пересчитано по таблице)

unsigned char PWMCount;                 // Счетчик для PWM режима управления мотором
unsigned int SpinCounter;               // Используется для плавного разгона и торможения мторора
unsigned short BeepCounter;             // Используется для звуковой пищалки

//------ переменные Timer'а
volatile unsigned char msec;
volatile unsigned char sec;     // Счетсик секунд
volatile unsigned char minute;  // Счетчик минут
volatile unsigned char hour;    // Счетсик часов
volatile unsigned char timer;   // секундный Таймер режимов стирки (max 255 с)

volatile unsigned char ProgramNumber;   // Номер выполняемой программы (0 - программа не выполняется)
volatile unsigned char ProgramCounter;  // Счетчик кадров
volatile unsigned char SubState;        // Сотояние подпрограммы кадра
volatile unsigned char __flash *zprg;   // Указатель на таблицу программ стирки
volatile unsigned char cmd;             // Регистр выполняемой команды программы стирки
volatile unsigned char t;               // Время выполнения команды
volatile unsigned char p;               // Параметр команды
volatile unsigned char __flash *retp;   // Return Pointer для цикла Do While
unsigned char cycles;          // Количество циклов повторения Do While
unsigned char cnt;             // Счетчик циклов Do While


//===================================================
//--------- Slave memory I/O registers (4+3+4+9+3 = 23)
// Область памяти, которая отображается на дисплее
// 20 байт
typedef union
{
  unsigned char Buf[20];
  struct
  {
    unsigned char Seg1; // Bit0 - SEGA ... Bit6 - SEGG
    unsigned char Seg2;
    unsigned char Seg3;
    unsigned char Seg4;

    unsigned char Red1;  // RED LED 1: zero - off, 0xFF - on
    unsigned char Red2;  // RED LED 2
    unsigned char Red3;  // RED LED 3

    unsigned char Ris1;  // zero - off, 0xFF - on
    unsigned char Ris2;
    unsigned char Ris3;
    unsigned char Ris4;

    unsigned char Modes;  // 0, 0xFF
    unsigned char Spin;   // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Termo;  // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Timer;  // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Lock;   // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Key;    // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Points; // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Buzzer; // 0, 0xFF
    unsigned char Gradus; // 0, 0xFF
   };
} regspackage;

regspackage Reg;

// Область памяти, которая читается слэйвом для опроса кнопок и селектора выбора программ
// 3 байта
volatile unsigned char Reg_Prog;  // Регистр номера выбранной программы в соотв. с АЦП
volatile unsigned char Reg_But1;  // Buttons Register 1
volatile unsigned char Reg_But2;  // Buttons Register 2

unsigned char MaskProg = 0xFF;    // Байт маски для индикации режимов. 0xFF - индикация работает; 0 - не работает 
unsigned char BufRead[3];         // Временный буфер для чтения I2C

//----------------------------
// Indication Symbols

#define DIG_0 0xC0
#define DIG_1 0xF9
#define DIG_2 0xA4
#define DIG_3 0xB0
#define DIG_4 0x99
#define DIG_5 0x92
#define DIG_6 0x82
#define DIG_7 0xF8
#define DIG_8 0x80
#define DIG_9 0x90
#define DIG_A 0x88
#define DIG_B 0x83
#define DIG_C 0xC6
#define DIG_D 0xA1
#define DIG_E 0x86
#define DIG_F 0x8E
#define DIG_P 0x8C
#define DIG_I 0xCF
#define DIG_S 0x92
#define DIG_U 0xC1
#define DIG_L 0xC7
#define DIG_H 0x89
#define DIG_SPACE  0xFF
#define DIG_c 0xA7
#define DIG_d 0xA1
#define DIG_n 0xAB
#define DIG_o 0xA3
#define DIG_r 0xAF
#define DIG_t 0x87
#define DIG__ 0xF7
#define DIG_MINUS 0xBF

// Таблица чаcтот вращения барабана
static __flash char SPIN[9]=
{
  // 0  100   200   300   400   500   600   700   800 prm
     0, 0x36, 0x39, 0x3F, 0x48, 0x51, 0x60, 0x6F, 0x7E
};

static __flash char HEX[16]=
{
  DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9,
  DIG_A, DIG_B, DIG_C, DIG_D, DIG_E, DIG_F
};

static __flash unsigned int DEC[256]=
{
  // 00 - 99
  DIG_0<<8|DIG_0, DIG_0<<8|DIG_1, DIG_0<<8|DIG_2, DIG_0<<8|DIG_3, DIG_0<<8|DIG_4, DIG_0<<8|DIG_5,
  DIG_0<<8|DIG_6, DIG_0<<8|DIG_7, DIG_0<<8|DIG_8, DIG_0<<8|DIG_9, DIG_1<<8|DIG_0, DIG_1<<8|DIG_1,
  DIG_1<<8|DIG_2, DIG_1<<8|DIG_3, DIG_1<<8|DIG_4, DIG_1<<8|DIG_5, DIG_1<<8|DIG_6, DIG_1<<8|DIG_7, 
  DIG_1<<8|DIG_8, DIG_1<<8|DIG_9, DIG_2<<8|DIG_0, DIG_2<<8|DIG_1, DIG_2<<8|DIG_2, DIG_2<<8|DIG_3, 
  DIG_2<<8|DIG_4, DIG_2<<8|DIG_5, DIG_2<<8|DIG_6, DIG_2<<8|DIG_7, DIG_2<<8|DIG_8, DIG_2<<8|DIG_9, 
  DIG_3<<8|DIG_0, DIG_3<<8|DIG_1, DIG_3<<8|DIG_2, DIG_3<<8|DIG_3, DIG_3<<8|DIG_4, DIG_3<<8|DIG_5, 
  DIG_3<<8|DIG_6, DIG_3<<8|DIG_7, DIG_3<<8|DIG_8, DIG_3<<8|DIG_9, DIG_4<<8|DIG_0, DIG_4<<8|DIG_1, 
  DIG_4<<8|DIG_2, DIG_4<<8|DIG_3, DIG_4<<8|DIG_4, DIG_4<<8|DIG_5, DIG_4<<8|DIG_6, DIG_4<<8|DIG_7, 
  DIG_4<<8|DIG_8, DIG_4<<8|DIG_9, DIG_5<<8|DIG_0, DIG_5<<8|DIG_1, DIG_5<<8|DIG_2, DIG_5<<8|DIG_3, 
  DIG_5<<8|DIG_4, DIG_5<<8|DIG_5, DIG_5<<8|DIG_6, DIG_5<<8|DIG_7, DIG_5<<8|DIG_8, DIG_5<<8|DIG_9, 
  DIG_6<<8|DIG_0, DIG_6<<8|DIG_1, DIG_6<<8|DIG_2, DIG_6<<8|DIG_3, DIG_6<<8|DIG_4, DIG_6<<8|DIG_5, 
  DIG_6<<8|DIG_6, DIG_6<<8|DIG_7, DIG_6<<8|DIG_8, DIG_6<<8|DIG_9, DIG_7<<8|DIG_0, DIG_7<<8|DIG_1, 
  DIG_7<<8|DIG_2, DIG_7<<8|DIG_3, DIG_7<<8|DIG_4, DIG_7<<8|DIG_5, DIG_7<<8|DIG_6, DIG_7<<8|DIG_7, 
  DIG_7<<8|DIG_8, DIG_7<<8|DIG_9, DIG_8<<8|DIG_0, DIG_8<<8|DIG_1, DIG_8<<8|DIG_2, DIG_8<<8|DIG_3, 
  DIG_8<<8|DIG_4, DIG_8<<8|DIG_5, DIG_8<<8|DIG_6, DIG_8<<8|DIG_7, DIG_8<<8|DIG_8, DIG_8<<8|DIG_9, 
  DIG_9<<8|DIG_0, DIG_9<<8|DIG_1, DIG_9<<8|DIG_2, DIG_9<<8|DIG_3, DIG_9<<8|DIG_4, DIG_9<<8|DIG_5, 
  DIG_9<<8|DIG_6, DIG_9<<8|DIG_7, DIG_9<<8|DIG_8, DIG_9<<8|DIG_9,
  // 100 - 199
  DIG_0<<8|DIG_0, DIG_0<<8|DIG_1, DIG_0<<8|DIG_2, DIG_0<<8|DIG_3, DIG_0<<8|DIG_4, DIG_0<<8|DIG_5,
  DIG_0<<8|DIG_6, DIG_0<<8|DIG_7, DIG_0<<8|DIG_8, DIG_0<<8|DIG_9, DIG_1<<8|DIG_0, DIG_1<<8|DIG_1,
  DIG_1<<8|DIG_2, DIG_1<<8|DIG_3, DIG_1<<8|DIG_4, DIG_1<<8|DIG_5, DIG_1<<8|DIG_6, DIG_1<<8|DIG_7, 
  DIG_1<<8|DIG_8, DIG_1<<8|DIG_9, DIG_2<<8|DIG_0, DIG_2<<8|DIG_1, DIG_2<<8|DIG_2, DIG_2<<8|DIG_3, 
  DIG_2<<8|DIG_4, DIG_2<<8|DIG_5, DIG_2<<8|DIG_6, DIG_2<<8|DIG_7, DIG_2<<8|DIG_8, DIG_2<<8|DIG_9, 
  DIG_3<<8|DIG_0, DIG_3<<8|DIG_1, DIG_3<<8|DIG_2, DIG_3<<8|DIG_3, DIG_3<<8|DIG_4, DIG_3<<8|DIG_5, 
  DIG_3<<8|DIG_6, DIG_3<<8|DIG_7, DIG_3<<8|DIG_8, DIG_3<<8|DIG_9, DIG_4<<8|DIG_0, DIG_4<<8|DIG_1, 
  DIG_4<<8|DIG_2, DIG_4<<8|DIG_3, DIG_4<<8|DIG_4, DIG_4<<8|DIG_5, DIG_4<<8|DIG_6, DIG_4<<8|DIG_7, 
  DIG_4<<8|DIG_8, DIG_4<<8|DIG_9, DIG_5<<8|DIG_0, DIG_5<<8|DIG_1, DIG_5<<8|DIG_2, DIG_5<<8|DIG_3, 
  DIG_5<<8|DIG_4, DIG_5<<8|DIG_5, DIG_5<<8|DIG_6, DIG_5<<8|DIG_7, DIG_5<<8|DIG_8, DIG_5<<8|DIG_9, 
  DIG_6<<8|DIG_0, DIG_6<<8|DIG_1, DIG_6<<8|DIG_2, DIG_6<<8|DIG_3, DIG_6<<8|DIG_4, DIG_6<<8|DIG_5, 
  DIG_6<<8|DIG_6, DIG_6<<8|DIG_7, DIG_6<<8|DIG_8, DIG_6<<8|DIG_9, DIG_7<<8|DIG_0, DIG_7<<8|DIG_1, 
  DIG_7<<8|DIG_2, DIG_7<<8|DIG_3, DIG_7<<8|DIG_4, DIG_7<<8|DIG_5, DIG_7<<8|DIG_6, DIG_7<<8|DIG_7, 
  DIG_7<<8|DIG_8, DIG_7<<8|DIG_9, DIG_8<<8|DIG_0, DIG_8<<8|DIG_1, DIG_8<<8|DIG_2, DIG_8<<8|DIG_3, 
  DIG_8<<8|DIG_4, DIG_8<<8|DIG_5, DIG_8<<8|DIG_6, DIG_8<<8|DIG_7, DIG_8<<8|DIG_8, DIG_8<<8|DIG_9, 
  DIG_9<<8|DIG_0, DIG_9<<8|DIG_1, DIG_9<<8|DIG_2, DIG_9<<8|DIG_3, DIG_9<<8|DIG_4, DIG_9<<8|DIG_5, 
  DIG_9<<8|DIG_6, DIG_9<<8|DIG_7, DIG_9<<8|DIG_8, DIG_9<<8|DIG_9,
  // 200 - 255
  DIG_0<<8|DIG_0, DIG_0<<8|DIG_1, DIG_0<<8|DIG_2, DIG_0<<8|DIG_3, DIG_0<<8|DIG_4, DIG_0<<8|DIG_5,
  DIG_0<<8|DIG_6, DIG_0<<8|DIG_7, DIG_0<<8|DIG_8, DIG_0<<8|DIG_9, DIG_1<<8|DIG_0, DIG_1<<8|DIG_1,
  DIG_1<<8|DIG_2, DIG_1<<8|DIG_3, DIG_1<<8|DIG_4, DIG_1<<8|DIG_5, DIG_1<<8|DIG_6, DIG_1<<8|DIG_7, 
  DIG_1<<8|DIG_8, DIG_1<<8|DIG_9, DIG_2<<8|DIG_0, DIG_2<<8|DIG_1, DIG_2<<8|DIG_2, DIG_2<<8|DIG_3, 
  DIG_2<<8|DIG_4, DIG_2<<8|DIG_5, DIG_2<<8|DIG_6, DIG_2<<8|DIG_7, DIG_2<<8|DIG_8, DIG_2<<8|DIG_9, 
  DIG_3<<8|DIG_0, DIG_3<<8|DIG_1, DIG_3<<8|DIG_2, DIG_3<<8|DIG_3, DIG_3<<8|DIG_4, DIG_3<<8|DIG_5, 
  DIG_3<<8|DIG_6, DIG_3<<8|DIG_7, DIG_3<<8|DIG_8, DIG_3<<8|DIG_9, DIG_4<<8|DIG_0, DIG_4<<8|DIG_1, 
  DIG_4<<8|DIG_2, DIG_4<<8|DIG_3, DIG_4<<8|DIG_4, DIG_4<<8|DIG_5, DIG_4<<8|DIG_6, DIG_4<<8|DIG_7, 
  DIG_4<<8|DIG_8, DIG_4<<8|DIG_9, DIG_5<<8|DIG_0, DIG_5<<8|DIG_1, DIG_5<<8|DIG_2, DIG_5<<8|DIG_3, 
  DIG_5<<8|DIG_4, DIG_5<<8|DIG_5

};

// Таблица перевода температуры 0...99 градусов
static __flash unsigned int TEMP[100]=
{
  (45), 46, 47, 48, 49, (50), 51, 53, 54, 56, (58), 59, 60, 62, 65,
  (68), 70, 72, 75, 78, (81), 84, 86, 90, 94, (98), 102, 105, 109, 112, 
  (117), 121, 125, 130, 135, (142), 146, 150, 155, 160, (164), 170, 176, 182, 188, 
  (196), 202, 207, 213, 220, (227), 233, 238, 244, 250, (258), 266, 273, 280, 288, 
  (296), 302, 312, 319, 326, (334), 342, 348, 355, 363, (372), 380, 388, 395, 403, 
  (411), 420, 428, 436, 443, (451), 461, 469, 475, 483, (492), 500, 508, 516, 522, 
  (531), 540, 548, 554, 560, (568), 580, 590, 602, 616
};



//---------------------------------------
// Задержка на дребезг
#define KEY_DELAY (-20)
// Задержка до первого повтора
#define KEY_REPDEL (100)
// Задержка на автоповтор
#define KEY_REPPER (78)

// Коды клавиш
#define kSuper  0x01
#define kEasy   0x02
#define kIron   0x03
#define kSpin   0x04
#define kTermo  0x05
#define kTimer  0x06
#define kLock   0x07
#define kOn     0x08
#define kStart  0x09

#define KEY_PRESSED_SHORT (1) // 1 соответствует 5 мс
#define KEY_PRESSED_LONG  (10)

volatile unsigned char Key = 0x00;  // Код нажатой клавиши, 0 - нет нажатия
unsigned char OldKey = 0x00;        // Старый код клавиши, 0 - все кнопки отпущены
signed char KeyCounter = KEY_DELAY; // Счетчик для задержек клавиатуры
volatile unsigned char KeyFlag = 0;
volatile unsigned char RepeatFlag;


//-------------------------------------------------------------------
// Процедура вычисления температуры в градусах
// На входе - значение АЦП
// На выходе - тепература в 0..99 градусов Цельсия или 0xFF - вне диапазона
//
unsigned char TemperatureConvert (unsigned int tempvalue)
{
  if (tempvalue <45 || tempvalue>616) return 0xFF;

  unsigned char i;
  for (i=0; i<100; i++)
  {
    if (tempvalue <= TEMP[i]) return i;
  }

  return i;
}



//-------------------------------------------------
// Пауза по таймеру прерываний (в секундах)
void delay_timer (unsigned char time)
{
  timer=time; // timer обрабатывается в прерываниях
  while (timer);
}


//-----------------------------------------------------------------------------
// Запускает/останавливает программу (Кнопкой Start/Stop)
// В параметры передается номер программы и указатель на таблицу программы.
// Также отображается Start/Stop программы во время старта, а также
// выдается звуковой сигнал. По останову сбрасываются регистры состояния
// электроавтоматики. 

void ProgramStartStop (unsigned char PrgNum, unsigned char __flash* TableProg)
{
  Timer_Buzzer = KEY_PRESSED_LONG;  // Пропищим

  if (ProgramNumber) // Если программа работает, то светим STOP и сбрасываем PC
  {
    MaskProg = 0xFF; // Восстанавливаем индикацию, если она была отключена (в режиме ожидания door)
    ProgramNumber = 0;
//    StoreEEPROM(&EE_nProgram, ProgramNumber);

    // Сразу сбрасываем все регистры электроавтоматики после нажатия ОТМЕНА.
    // А также выходные ноги контроллера т.к. их состояние не сбросится в delay_timer.
    // это чуть длиннее, но надежнее.
    HeatingState=0; HEATING=0;
    RotorState=0;
    ForwardState=0; FORWARD=0;
    ReverseState=0; REVERSE=0;
    PompState=0; POMP=1;        // В этой версии управление симисторами инверсное
    WaterState1=0; WATER1=1;    // В этой версии управление симисторами инверсное
    WaterState2=0; WATER2=1;    // В этой версии управление симисторами инверсное
    LockState=0; LOCK=1;        // В этой версии управление симисторами инверсное
    Reg.Seg1 = DIG_S;
    Reg.Seg2 = DIG_t;
    Reg.Seg3 = DIG_o;    
    Reg.Seg4 = DIG_P;
    Reg.Lock = 0; // Сбросить индикацию работы программы (Замочек)
    i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // Передаем регистры индикации
    delay_timer(2); // Покажем на две секунды Stop
  }
  else  // если программа не запущена, то светим Good и устанавливаем PC
  {
    // Если запустили в режиме редактирования, то сбрасываем эти режимы и регистры индикации
    if (SpinMode!=0)  { SpinMode = 0; Reg.Spin = 0; }
    if (TempMode!=0)  { TempMode = 0; Reg.Termo = 0; Reg.Gradus = 0; }

    SpinRpm = SPIN[SetSpin]; // Устанавливаем обороты для драйвера мотора, пересчитаные по таблице
    ProgramNumber = PrgNum; // Номер программы (нужен фактически, чтобы знать программа работает или нет)
    zprg = TableProg; // Указатель на выполняемую программу
    ProgramCounter = 0;
    SubState = 0;
    Reg.Seg1 = DIG_6;
    Reg.Seg2 = DIG_o;
    Reg.Seg3 = DIG_o;    
    Reg.Seg4 = DIG_d;
    Reg.Lock = 0xE0; // Установить индикацию работы программы (Мигающий Замочек)
    i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // Передаем регистры индикации
    delay_timer(2);                     // Покажем на 2 секунды "Good"
//    StoreEEPROM(&EE_nProgram, ProgramNumber);
  }  
}

//--------------------------------------------------------------
// Режим запуска программ и редактирования параметров стирки
// Когда программа не запущана.
void ManualMode (void)
{
      //------------ Запуск 8 программы - продолжительная стирка ------------------
      if (Reg_Prog==8 && Key==kStart && !RepeatFlag)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram8);  // Читаем требуемую температуру нагрева из EEPROM
        SetSpin = LoadEEPROM(&EE_rpmProgram8);      // Читаем из EEPROM код оборотов
        ProgramStartStop(8, PROGRAM8);                  // Запуск программы №8
      }

      //------------ Запуск 9 программы - быстрая стирка ---------------------
      if (Reg_Prog==9 && Key==kStart && !RepeatFlag)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram9);
        SetSpin = LoadEEPROM(&EE_rpmProgram9);
        ProgramStartStop(9, PROGRAM9);
      }

      //------------ Запуск 12 программы - интенсивное полоскание, слив и деликатный отжим ------------
      if (Reg_Prog==12 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram12);
        ProgramStartStop(12, PROGRAM12);
      }

      //------------ Запуск 13 программы - легкое полоскание, слив и деликатный отжим -----------
      if (Reg_Prog==13 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram13);
        ProgramStartStop(13, PROGRAM13);
      }
      
      //------------ Запуск 14 программы - интенсивный отжим -------------------
      if (Reg_Prog==14 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram14);
        ProgramStartStop(14, PROGRAM14);
      }

      //------------ Запуск 15 программы - легкий отжим ----------------------
      if (Reg_Prog==15 && Key==kStart && !RepeatFlag)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram15);
        ProgramStartStop(15, PROGRAM15);
      }

      //------------ Запуск 16 программы - слив воды ----------------------
      if (Reg_Prog==16 && Key==kStart && !RepeatFlag)
      {
        ProgramStartStop(16, PROGRAM16);
      }

      // Timer mode minute/hour
      if (Reg_Prog==11 && Key==kTimer && !RepeatFlag)
      {
        ClockMode = ~ClockMode;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Timer Reset
      if (Reg_Prog==11 && Key==2 && !RepeatFlag)
      {
        hour = 0;
        minute = 0;
        sec = 0;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      //------------ Управление секретным кодом для сервисного режима --------------------
      // В режиме 7 нажимаем кнопку Замка - загорится знак буззера,
      // теперь можно кнопками + и - выставить секретный код.
      // После ввода кода будут доступны кнопки управления всеми сервисными режимами машинки.
      if (Reg_Prog==7 && Key==kLock && !RepeatFlag)
      {
        if (SecretMode==0)  SecretMode = 0xFF; 
        else                SecretMode = 0; 
        Timer_Buzzer = KEY_PRESSED_SHORT;
      }
      // SecretCode Increase
      if (Reg_Prog==7 && Key==1 && SecretMode==0xFF)
      {
        SecretCode++;
        Timer_Buzzer = KEY_PRESSED_SHORT;
      }
      // SecretCode Decrease
      if (Reg_Prog==7 && Key==3 && SecretMode==0xFF)
      {
        SecretCode--;
        Timer_Buzzer = KEY_PRESSED_SHORT;
      }
      
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 8 -------------------
      // Обороты отжима 8 программы можно изменить, нажав кнопку оборотов.
      // После нажатия замигает синий знак оборотов и можно будет
      // кнопками + и - установить желаемое значение в оборотах/мин, кратное 100.
      // Установленное значение сохраняется в EEPROM.
      if (Reg_Prog==8 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 8;
          TempMode = 0; // Сброс режима ввода температуры если нажали изменение оборотов
          Reg.Termo = 0;
          Reg.Spin = 0xE0; // Зажигаем мигающую иконку оборотов
          SetSpin = LoadEEPROM(&EE_rpmProgram8); // Копируем значение из EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 8
      if (Reg_Prog==8 && Key==1 && SpinMode==8)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram8);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram8, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 8
      if (Reg_Prog==8 && Key==3 && SpinMode==8)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram8);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram8, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 9 -------------------
      // Обороты отжима 9 программы можно изменить, нажав кнопку оборотов.
      // После нажатия замигает синий знак оборотов и можно будет
      // кнопками + и - установить желаемое значение в оборотах/мин, кратное 100.
      // Установленное значение сохраняется в EEPROM.
      if (Reg_Prog==9 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 9;
          TempMode = 0;
          Reg.Termo = 0;
          Reg.Spin = 0xE0; // Зажигаем мигающую иконку оборотов
          SetSpin = LoadEEPROM(&EE_rpmProgram9); // Копируем значение из EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 9
      if (Reg_Prog==9 && Key==1 && SpinMode==9)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram9);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram9, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 9
      if (Reg_Prog==9 && Key==3 && SpinMode==9)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram9);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram9, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 12 -------------------
      // Обороты отжима 12 программы можно изменить, нажав кнопку оборотов.
      // После нажатия замигает синий знак оборотов и можно будет
      // кнопками + и - установить желаемое значение в оборотах/мин, кратное 100.
      // Установленное значение сохраняется в EEPROM.
      if (Reg_Prog==12 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 12;
          Reg.Spin = 0xE0; // Зажигаем мигающую иконку оборотов
          SetSpin = LoadEEPROM(&EE_rpmProgram12); // Копируем значение из EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 12
      if (Reg_Prog==12 && Key==1 && SpinMode==12)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram12);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram12, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 12
      if (Reg_Prog==12 && Key==3 && SpinMode==12)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram12);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram12, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 13 -------------------
      // Обороты отжима 13 программы можно изменить, нажав кнопку оборотов.
      // После нажатия замигает синий знак оборотов и можно будет
      // кнопками + и - установить желаемое значение в оборотах/мин, кратное 100.
      // Установленное значение сохраняется в EEPROM.
      if (Reg_Prog==13 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 13;
          Reg.Spin = 0xE0; // Зажигаем мигающую иконку оборотов
          SetSpin = LoadEEPROM(&EE_rpmProgram13); // Копируем значение из EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 13
      if (Reg_Prog==13 && Key==1 && SpinMode==13)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram13);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram13, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 13
      if (Reg_Prog==13 && Key==3 && SpinMode==13)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram13);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram13, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Spin mode for program 14 -------------------
      // Обороты отжима 14 программы можно изменить, нажав кнопку оборотов.
      // После нажатия замигает синий знак оборотов и можно будет
      // кнопками + и - установить желаемое значение в оборотах/мин, кратное 100.
      // Установленное значение сохраняется в EEPROM.
      if (Reg_Prog==14 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 14; 
          Reg.Spin = 0xE0; // Зажигаем мигающую иконку оборотов
          SetSpin = LoadEEPROM(&EE_rpmProgram14); // Копируем значение из EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 14
      if (Reg_Prog==14 && Key==1 && SpinMode==14)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram14);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram14, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 14
      if (Reg_Prog==14 && Key==3 && SpinMode==14)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram14);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram14, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------
      
      // ----------- Set Spin mode for program 15 -------------------
      // Обороты отжима 15 программы можно изменить, нажав кнопку оборотов.
      // После нажатия замигает синий знак оборотов и можно будет
      // кнопками + и - установить желаемое значение в оборотах/мин, кратное 100.
      // Установленное значение сохраняется в EEPROM.
      if (Reg_Prog==15 && Key==kSpin && !RepeatFlag)
      {
        if (SpinMode==0)
        { 
          SpinMode = 15; 
          Reg.Spin = 0xE0; // Зажигаем мигающую иконку оборотов
          SetSpin = LoadEEPROM(&EE_rpmProgram15); // Копируем значение из EEPROM
        }
        else              
        {
          SpinMode = 0; 
          Reg.Spin = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Spin Increase for program 15
      if (Reg_Prog==15 && Key==1 && SpinMode==15)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram15);
        if (SetSpin <= MAX_SPIN-1)
        {
          SetSpin++;
          StoreEEPROM(&EE_rpmProgram15, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Spin Decrease for program 15
      if (Reg_Prog==15 && Key==3 && SpinMode==15)
      {
        SetSpin = LoadEEPROM(&EE_rpmProgram15);
        if (SetSpin >= MIN_SPIN+1)
        {
          SetSpin--;
          StoreEEPROM(&EE_rpmProgram15, SetSpin);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------

      // ----------- Set Temperature mode for program 8 -------------------
      // Температуру 8 программы можно изменить, нажав кнопку термометра.
      // После нажатия замигает красный знак термометра и можно будет
      // кнопками + и - установить желаемое значение в градусах, кратное 5.
      // Установленная температура сохраняется в EEPROM.
      if (Reg_Prog==8 && Key==kTermo && !RepeatFlag)
      {
        if (TempMode==0)  
        { 
          TempMode = 8;
          SpinMode = 0;
          Reg.Spin = 0;
          Reg.Termo = 0xE0; 
          SetTemperature = LoadEEPROM(&EE_tProgram8); 
        }
        else              
        {
          TempMode = 0; 
          Reg.Termo = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Temperature Increase for program 8
      if (Reg_Prog==8 && Key==1 && TempMode==8)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram8);
        if (SetTemperature <= MAX_TEMPERATURE-5)
        {
          SetTemperature = SetTemperature + 5;
          StoreEEPROM(&EE_tProgram8, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Temperature Decrease for program 8
      if (Reg_Prog==8 && Key==3 && TempMode==8)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram8);
        if (SetTemperature >= MIN_TEMPERATURE+5)
        {
          SetTemperature = SetTemperature - 5;
          StoreEEPROM(&EE_tProgram8, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //----------------------------------------------------------------------------------
      
      // ------------- Temperature mode for program 9 -------------------------
      // Температуру 9 программы можно изменить, нажав кнопку термометра.
      // После нажатия замигает красный знак термометра и можно будет
      // кнопками + и - установить желаемое значение в градусах, кратное 5.
      // Установленная температура сохраняется в EEPROM.
      if (Reg_Prog==9 && Key==kTermo && !RepeatFlag)
      {
        if (TempMode==0)  
        { 
          TempMode = 9;
          SpinMode =0;
          Reg.Spin = 0;
          Reg.Termo = 0xE0; 
          SetTemperature = LoadEEPROM(&EE_tProgram9);
        }
        else              
        {
          TempMode = 0; 
          Reg.Termo = 0; 
        }
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      // Temperature Increase for program 9
      if (Reg_Prog==9 && Key==1 && TempMode==9)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram9);
        if (SetTemperature <= MAX_TEMPERATURE-5)
        {
          SetTemperature = SetTemperature + 5;
          StoreEEPROM(&EE_tProgram9, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      // Temperature Decrease for program 9
      if (Reg_Prog==9 && Key==3 && TempMode==9)
      {
        SetTemperature = LoadEEPROM(&EE_tProgram9);
        if (SetTemperature >= MIN_TEMPERATURE+5)
        {
          SetTemperature = SetTemperature - 5;
          StoreEEPROM(&EE_tProgram9, SetTemperature);
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      }
      //---------------------------------------------------------------------------------

      //-------------- Секретные функции управления ------------------------------
      // Эти функции доступны после ввода секретного кода
      if (SecretCode==SECRET_CODE && SecretMode==0) // Если секретный режим сброшен и выставлен секретный код
      {
        //Temperature mode gradus/ADC
        if (Reg_Prog==10 && Key==kTermo && !RepeatFlag)
        {
          if (TempMode==0)  TempMode = 0xFF;
          else              TempMode = 0;
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }

        // Lock ON/OFF and Line ON/OFF
        if (Reg_Prog==1 && Key==kStart && !RepeatFlag)
        {
          LockState = ~LockState;
          Timer_Buzzer=KEY_PRESSED_SHORT;
        }
      
        // Forward ON/OFF     
        if (Reg_Prog==2 && Key==kStart && !RepeatFlag)
        {
          // Если НАЗАД выключено и нет автоповтора кнопки, то переключаем ВПЕРЕД
          if (!ReverseState)  { ForwardState = ~ForwardState; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Reverse ON/OFF
        if (Reg_Prog==2 && Key==8 && !RepeatFlag)
        {
          if (!ForwardState)  { ReverseState = ~ReverseState; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Speed Increase
        if (Reg_Prog==2 && Key==1)
        {
          if (RotorState<=0xFF-3) { RotorState = RotorState + 3; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Speed Decrease
        if (Reg_Prog==2 && Key==3)
        {
          if (RotorState >= 3)  { RotorState = RotorState - 3; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }

        // Speed Reset to Zero
        // middle button
        if (Reg_Prog==2 && Key==2)  
        {
          if (RotorState) { RotorState=0; Timer_Buzzer=KEY_PRESSED_SHORT; }// Если не равен нулю, то обнуляем
        }

        // Speed Set to minimal rotation
        // SPIN button
        if (Reg_Prog==2 && Key==4)  
        {
          if (RotorState!=0x3C) { RotorState=0x3C; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }
      
        // Speed Set to speed rotation 2
        // TERMO button
        if (Reg_Prog==2 && Key==5)  
        {
          if (RotorState!=0x50) { RotorState=0x50; Timer_Buzzer=KEY_PRESSED_SHORT; }
        }
      
        // Pomp ON/OFF
        if (Reg_Prog==4 && Key==kStart && !RepeatFlag)
        {
         PompState = ~PompState; 
         Timer_Buzzer=KEY_PRESSED_SHORT;
        }

        // Water ON/OFF
        if ( Reg_Prog==3 && Key==kStart && !RepeatFlag) 
        {
         WaterState1 = ~WaterState1;
         WaterState2 = WaterState1;
         Timer_Buzzer=KEY_PRESSED_SHORT;
        }

        // Heater ON/OFF
        if (Reg_Prog==5 && Key==kStart && !RepeatFlag)
        { // Блокировка нагрева, если бак пустой. Включаем только когда уровень2=0
          if ( LEVEL1==1 && LEVEL2==0 ) HeatingState = ~HeatingState;
          else HeatingState=0;  // Если уровень изменился на запрещенный, то сбросить нагрев
          if (HeatingState)  Timer_Buzzer=50; // Предупреждаем о включении нагревателя длинным писком
        }
      }
    
}

//--------------------------------------------------------------
// === Процедура обработчика программы стирки ===
// Для полного кайфа добавлены команды цикла DO и WHILE.
// Например: PRG_Do, 10,  // Цикл 10 раз
//           PRG_Buzzer, 5, 
//           PRG_While,   // Повторить цикл - пропищать 10 раз

void WashingProcess (void)
{
  //------------- Program Counter ----------------------------
  // Обработчик программы стирки.
  // Пока таймер не отработает, мы не попадем в обработчик кадров.

  if (ProgramNumber && timer==0)  // Если программа запущена и таймер свободен то берем следующий кадр.
  {
    cmd = *zprg;  // Взяли команду

    // Здесь будет вертеться всегда, пока в обработчике команд не будет увеличен указатель на следующую команду
    switch (cmd)
    {

     // 2 bytes - Цикл Do While (Do TWhile). Секция Do.
      // Эта команда лишь запоминает точку возврата и может
      // использоваться с командами TWhile и While.
     // Точка возврата будет за этой командой, чтобы каждый раз не брать параметры
    case PRG_Do:
      retp = zprg + 2;  // Запоминаем указатель возврата цикла Do While - сразу за этой командой
      zprg++;           // указатель на параметр
      cycles = *zprg++; // Взяли параметр количества циклов
      ProgramCounter++; // Счетчик кадров увеличили
      break;

    // 1 byte - Цикл Do While. Секция While - возврат на цикл. Кадры While не считает.
    case PRG_While:
      cnt++;  // Увеличиваем счетчик циклов
      if (cnt==cycles)  // Если счетчик достиг требуемого количества циклов, то едем дальше
      {
        cnt = 0; // Сбросили счетчик циклов на выходе
        zprg++;  // увеличиваем указатель команд - программа продолжается
      }
      else  // иначе возвращаем указатель команд на начало цикла Do
      {
        zprg = retp;      // точка возврата цикла Do While
      }
      break;

      // 2 bytes
    case PRG_Pause:
      zprg++; // Увеличили указатель
      t = *zprg++;
      if (t!=0) timer = t;  // Проверяем, если не ноль, то устанавливаем таймер
      ProgramCounter++;
      break;

      // 2 bytes
    case PRG_LockState:
      zprg++;
      LockState = *zprg++;  // Передаем значение параметра
      ProgramCounter++; // Увеличиваем счетчик кадров
      break;

      // 3 bytes (with pause)
      // Клапан залива воды предварительной стирки
    case PRG_WaterState1:
      zprg++;
      WaterState1 = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
      // Клапан залива воды основной стирки
    case PRG_WaterState2:
      zprg++;
      WaterState2 = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_PompState:
      zprg++;
      PompState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_HeatingState:
      zprg++;
      HeatingState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_RotorState:
      zprg++;
      RotorState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_ForwardState:
      zprg++;
      ForwardState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes (with pause)
    case PRG_ReverseState:
      zprg++;
      ReverseState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 2 bytes
      // второй байт - параметр t=(n*5 ms)
    case PRG_Buzzer:
      zprg++;
      Timer_Buzzer = *zprg++;
      ProgramCounter++;
      break;

      // 1 byte
      // Составная подпрограмма.
      // плавное тоорможение и выключение мотора
    case PRG_MotorBrake:
      {
        switch (SubState)
        {
        case 0:
          SpinCounter = RotorState<<8; // Подготовка счетчика
          SubState++;
          break;
        case 1:
          SpinCounter--; // Счетчик уменьшения скорости
          // Если счетчик больше нуля,
          if ((SpinCounter>>8) > 0)   RotorState = SpinCounter>>8; // то уменьшаем обороты
          else
          {
            ForwardState = 0x00; // Выключить мотор вперед
            SubState=0;
            ProgramCounter++;
            zprg++; // Указатель на следующую команду
          }
        }
        break;
      }

      // 2 bytes
      // Составная подпрограмма.
      // Включение мотора вперед и плавный разгон до оборотов, указанных в параметре
    case PRG_MotorStart:
      {
        switch (SubState)
        {
        case 0:
          zprg++;
          p = *zprg;      // берем параметр оборотов
          zprg--;      // Вернули указатель на команду, чтобы продолжить подпрограмму
          // Если скорость <= 0x28 то сразу пропускаем команду - идем дальше
          if (p <= 0x28) { zprg = zprg+2; break; }
          SpinCounter = 0; // Подготовка счетчика - обнуление
          ForwardState = 0xFF; // Включить мотор вперед
          SubState++;
          break;
        case 1:
          SpinCounter++;  // Крутим счетчик - плавно увеличиваем обороты
          // Если счетчик меньше установленной скорости
          if ((SpinCounter>>8) < p)   RotorState = SpinCounter>>8; // то увеличиваем обороты ротора
          else
          {
            RotorState = SpinCounter>>8;
            SubState=0;
            ProgramCounter++;          
            zprg = zprg + 2; // Указатель на следующую команду
          }
        }
        break;
      }

      // 3 bytes
      // Составная подпрограмма.
      // Включение мотора вперед,
      // плавный разгон мотора до оборотов n (второй байт)
      // третий байт - время работы в секундах до плавного торможения.
      // Аналогична PRG_MotorSetSpin, только со своим параметром оборотов
    case PRG_MotorSpin:
      {
        switch (SubState)
        {
        case 0:
          zprg++;
          p = *zprg++;      // берем параметр оборотов
          t = *zprg;        // берем параметр времени (максимум 255 с)
          zprg = zprg - 2;      // Вернули указатель на команду, чтобы продолжить подпрограмму
          // Если скорость <= 0x28 то сразу пропускаем команду - идем дальше
          if (p <= 0x28) { zprg = zprg+3; break; }
          SpinCounter = 0; // Подготовка счетчика - обнуление
          ForwardState = 0xFF; // Включить мотор вперед
          SubState++;
          break;
        case 1:
          SpinCounter++;  // Крутим счетчик - плавно увеличиваем обороты
          // Если счетчик меньше установленной скорости
          if ((SpinCounter>>8) < p)   RotorState = SpinCounter>>8; // то увеличиваем обороты ротора
          else                        
          {
            RotorState = SpinCounter>>8;
            SubState++; // иначе едем дальше
          }
          break;
        case 2:
          timer = t; // Время отжима в пареметре t (максимум 255 с)
          SubState++;
          break;
        // Тормозим также как и разгонялись
        case 3:
          SpinCounter--; // Счетчик уменьшения скорости
          // Если счетчик больше нуля,
          if ((SpinCounter>>8) > 0)   RotorState = SpinCounter>>8; // то уменьшаем обороты
          else                        SubState++; // иначе идем дальше
          break;
        case 4:
          ForwardState = 0x00; // Выключить мотор вперед
          SubState=0;
          ProgramCounter++;          
          zprg = zprg + 3; // Указатель на следующую команду
        }
        break;
      }

      // 2 bytes
      // Составная подпрограмма.
      // Включение мотора вперед,
      // плавный разгон мотора до оборотов, установленных в SpinRpm и плавное торможение.
      // Второй байт - время работы в секундах до плавного торможения.
      // Использовать для циклов отжим.
    case PRG_MotorSetSpin:
      {
        switch (SubState)
        {
        case 0:
          // Если скорость <= 0x28 то сразу пропускаем команду - идем дальше
          if (SpinRpm <= 0x28) { zprg = zprg+2; break; }
          SpinCounter = 0; // Подготовка счетчика - обнуление
          ForwardState = 0xFF; // Включить мотор вперед
          SubState++;
          break;
        case 1:
          SpinCounter++;  // Крутим счетчик - плавно увеличиваем обороты
          // Если счетчик меньше установленной скорости
          if ((SpinCounter>>8) < SpinRpm)   RotorState = SpinCounter>>8; // то увеличиваем обороты ротора
          else                              
          {
            RotorState = SpinCounter>>8;
            SubState++; // иначе едем дальше
          }
          break;
        case 2:
          zprg++;
          timer = *zprg; // Время отжима (максимум 255 с)
          zprg--;      // Вернули указатель на команду, чтобы продолжить подпрограмму
          SubState++;
          break;
        // Тормозим также как и разгонялись
        case 3:
          SpinCounter--; // Счетчик уменьшения скорости
          // Если счетчик больше нуля,
          if ((SpinCounter>>8) > 0)   RotorState = SpinCounter>>8; // то уменьшаем обороты
          else                        SubState++; // иначе идем дальше
          break;
        case 4:
          ForwardState = 0x00; // Выключить мотор вперед
          SubState=0;
          ProgramCounter++;          
          zprg = zprg + 2; // Указатель на следующую команду
        }
        break;
      }
      
      // 3 bytes
      // Составная подпрограмма для циклов стирки.
      // ZigZag - туда-сюда.
      // Плавный разгон мотора влево до оборотов в пределах 0x3C...0x41 (второй байт), пауза,
      // плавное торможение, пауза. Повтор цикла с вкл. мотора вправо.
      // т.е. сначала в одну сторону, потом в другую.
      // третий байт - время работы в промежутках между разгоном и торможением, а также паузы.
      //  ____      ____
      // /    \____/    \____
      // Использовать для циклов "обычная стирка" и "интенсивная стирка".
      // В параметрах скорости: 0x3C - обычная стирка, 0x41 - интенсивная стирка.
    case PRG_MotorZigZag:
      {
        switch (SubState)
        {
        case 0: 
          ForwardState = 0xFF;
          SubState++;
          break;
        case 1:
          RotorState = 0x00;
          timer=1;
          SubState++;
          break;
        case 2:
          RotorState = 0x37;  // Малые обороты
          timer=2;
          SubState++;
          break;
        case 3:
          zprg++;
          p = *zprg++;
          t = *zprg;
          zprg = zprg - 2;  // Вернули указатель на команду, чтобы продолжить подпрограмму
          RotorState = p;
          timer=t;
          SubState++;
          break;
        case 4:
          RotorState = 0x37;
          timer=2;
          SubState++;
          break;
        case 5:
          RotorState = 0x00;
          timer=3;          // 3 секунды пауза до полного останова
          SubState++;
          break;
        case 6:
          ForwardState = 0x00;  // Выключение вперед
          timer=1;              // pause
          SubState++;
          break;
        case 7:
          ReverseState = 0xFF;  // Включаем назад
          timer=1;
          SubState++;
          break;
        case 8:
          RotorState = 0x37;
          timer=2;
          SubState++;
          break;
        case 9:
          RotorState = p;
          timer=t;
          SubState++;
          break;
        case 10:
          RotorState = 0x37;
          timer=2;
          SubState++;
          break;
        case 11:
          RotorState = 0x00;
          timer=3;          // 3 секунды пауза до полного останова
          SubState++;
          break;
        case 12:
          ReverseState = 0x00;  // Выключаем назад
          timer=1;
          SubState=0;
          ProgramCounter++;
          zprg = zprg + 3; // Указатель на следующую команду
        }
        break;
      }
      
      // 1 byte
      // Проверка установленной температуры и включение ТЭНа, если нужно
      // (Интеллектуальное включение ТЭНа)
    case PRG_HeatingTest:
      {
        ProgramCounter++;
        zprg++;  // указатель на следующую команду
        // Если текущая t меньше требуемой, то включаем ТЭН и едем дальше
        if (temperature < SetTemperature)   HeatingState = 0xFF;
        break;
      }
      
      // 1 byte
      // Цикл жидания нагрева воды до заданной температуры.
      // Используется вместе с командой Do, которая запоминает программный указатель цикла.
      // Если температура достигла заданной юзером, то продолжаем программу дальше;
      // Если температура не достигла заданной, то указатель на следующую команду устанваливается в
      // соответствии с командой Do. При этом параметр цикла Do не важен.
      // Перед этой командой нужно включить ТЭН, чтобы температура temperature
      // увеличивалась. Также нужно проверить уровень воды в баке (иначе ТЭН не включится).
      // При нагреве воды нужно включить медленное вращение бака для перемешивания воды 
      // (обычно внутри цикла).
      // В секции индикации программы, если включен ТЭН, то будет отображаться температура воды
      // и красная иконка термометра.
    case PRG_TWhile:
      {
        if (temperature >= SetTemperature) // Если текущая t больше или равна требуемой, то ОК, едем дальше
        {
          ProgramCounter++;
          zprg++;  // указатель на следующую команду
        }
        else  // иначе возвращаем указатель команд на начало цикла Do
        {
          zprg = retp;      // точка возврата цикла Do TWhile
        }
        break;
      }
        
      // 1 byte
      // Ожидание уровня LEVEL2=0 ( в баке есть вода).
      // Будет ждать до бесконечности, или пока не нажать СТОП/ОТМЕНА.
      // Специально введена индикация ожидания наличия воды в баке - H2O.
    case PRG_L2Wait:
      MaskProg=0; // Выключаем индикацию режимов (фиксация индикации H2O)
      Reg.Seg1 = DIG_H; // Светим H2O - регистры передадутся в панель в основном цикле
      Reg.Seg2 = DIG_2;
      Reg.Seg3 = DIG_o;
      Reg.Seg4 = DIG_SPACE;
        
      if (LEVEL1==1 && LEVEL2==0) // Если сигнал появился, то 
      {
        MaskProg = 0xFF; // Включаем индикацию режимов (убирает индикацию H2O)
        ProgramCounter++;
        zprg++;  // увеличиваем указатель команд
      }
      else
      {
        BeepCounter++;
        // Ненавязчиво пикаем, намекая, что пора уже и обратить внимание
        if (BeepCounter == 0 || BeepCounter == 0x100)     Timer_Buzzer = 10;
      }
      break;

      // 1 byte
      // Ожидание уровня LEVEL1=0 (бак пустой).
      // Будет ждать до бесконечности, или пока не нажать СТОП/ОТМЕНА.
      // Специально введена индикация ожидания пустого бака - FULL.
      // Полезно, когда нужно проконтролировать опустошение бака после включения помпы.
    case PRG_L1Wait:
      MaskProg=0; // Выключаем индикацию режимов (фиксация индикации FULL)
      Reg.Seg1 = DIG_F; // Светим FULL - регистры передадутся в панель в основном цикле
      Reg.Seg2 = DIG_U;
      Reg.Seg3 = DIG_L;
      Reg.Seg4 = DIG_L;
        
      if (LEVEL1==0 && LEVEL2==1) // Если сигнал появился, то 
      {
        MaskProg = 0xFF; // Включаем индикацию режимов (убирает индикацию FULL)
        ProgramCounter++;
        zprg++;  // увеличиваем указатель команд
      }
      break;


      // 1 byte
      // Ожидание закрытия дверцы люка и появления сигнала с АЦП контроля фазы.
      // Будет ждать до бесконечности, или пока не нажать СТОП/ОТМЕНА.
      // Специально введена индикация ожидания закрытия люка - door.
    case PRG_LockWait:
      MaskProg=0; // Выключаем индикацию режимов (фиксация индикации door)
      Reg.Seg1 = DIG_d; // Светим door - ожидание закрытия люка
      Reg.Seg2 = DIG_o; // (регистры передадутся в панель в основном цикле)
      Reg.Seg3 = DIG_o;
      Reg.Seg4 = DIG_r;
        
      if (adcLine < LINE_ON_TRESHOLD) // Если сигнал появился, то 
      {
        MaskProg = 0xFF; // Включаем индикацию режимов (убирает индикацию door)
        ProgramCounter++;
        zprg++;  // увеличивем указатель команд
      }
      else
      {
        BeepCounter++;
        // Ненавязчиво пикаем, намекая, что пора уже и обратить внимание - закрыть люк
        if (BeepCounter == 0 || BeepCounter == 0x100) Timer_Buzzer = 10;
      }
      break;
      
      // 1 byte
    case PRG_End:       
      ProgramNumber=0;
//        StoreEEPROM(0x00, ProgramNumber);
      Timer_Buzzer = 50;  // Первый писк
      // Индикация конца выполнения программы
      Reg.Seg1 = DIG_E;
      Reg.Seg2 = DIG_n;
      Reg.Seg3 = DIG_d;
      Reg.Seg4 = DIG_SPACE;
      Reg.Lock = 0; // Сбросить индикацию работы программы (Замочек)
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
      delay_timer(2);
      Timer_Buzzer = 25;  // Второй писк
      break;

    // Неопознанная команда
    default:
      ProgramNumber=0;
      Reg.Seg1 = DIG_E; // Error4 - неопознанная команда или сбой программы стирки.
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_4;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
      while (1) { __no_operation(); } // Светим номер ошибки и зависаем
     }
  }
  
  
  
}



