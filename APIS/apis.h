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

/*
#define delay_us(V) __delay_cycles(V*8L) // кварц 8 МГц


void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}
*/

#define APIS_QUARTZ       (8000000L)
#define DISPLAY_CONST     (1384)      // Значение счетчика для таймера индикации
#define I2C_TIMER_CONST   (100)       // 50 ms
#define BEEP_CONST        (10)        // Время писка в мс

//---------------------------------------------------------------
// Основные управляющие выходы

//#define SCLK     PORTB_Bit7  // Зарезервирован под индикатор
//#define SDATA    PORTB_Bit6  // Зарезервирован под индикатор
#define SCK       PORTB_Bit5
#define MISO      PORTB_Bit4
#define HEATING   PORTB_Bit3  // (MOSI) Управление нагревом
#define ROTOR     PORTB_Bit2  // Управление симистором мотора
#define FORWARD   PORTB_Bit1  // Управление статором (реле 1)
// portb_bit0 for ICP1 (TACHO input)

#define NEUTRAL   PORTC_Bit0  // Включение нейтрали
#define BUZZER    PORTC_Bit1
#define REVERSE   PORTC_Bit2  // Управление статором (реле 2)
#define RESET_LCD PORTC_Bit3  // Сброс LCD
// PC4 - SDA I2C
// PC5 - SCL I2C
// PC6 - ADC6 (LINE)
// PC7 - ADC7 (Termo)

#define POMP      PORTD_Bit7  // Управление помпой
#define WATER     PORTD_Bit6  // Управление клапаном залива воды
#define LOCK      PORTD_Bit5  // Управление замком люка
// PD4 for input - LEVEL3
// PD3(INT1) for input - IR
// PD2 (INT0) for input - ZeroCrossDetect
// PD1 (LEVEL2) for input
// PD0 (LEVEL1) for input


//------------------------------------
// Входы

#define LEVEL1  PIND_Bit0
#define LEVEL2  PIND_Bit1
#define LEVEL3  PIND_Bit4

//#define KEY1    PINC_Bit5
//#define KEY2    PIND_Bit4

//===================================================================
void InitPorts (void)
{
  //----------------- Port B
  DDRB=0xFE; // TACHO (ICP1) на ввод
  PORTB=0x01; // подтяжка TACHO (ICP1)
  
  //----------------- Port C
  DDRC=0xFF;
  PORTC=0x00;
  
  //----------------- Port D
  // PD0 (LEVEL1) for input
  // PD1 (LEVEL2) for input
  // PD2 (INT0) for input - ZeroCrossDetect
  // PD3(INT1) for input - IR
  // PD4 for input - LEVEL3
  DDRD=0xE0;  // LEVEL3 & IR & ZCD for input
  PORTD=0x1F; // LEVEL3 & IR & ZCD input pullup
}

// глобальные переменные
#ifdef DEBUG_DISPLAY
  unsigned short Timer_Display;     // Регистр счетчика дисплея
#endif //DEBUG_DISPLAY
unsigned char Timer_I2C;   // Таймер опроса I2C
unsigned char Timer_Buzzer; // Таймер писка
unsigned char i2c_errors;
unsigned char i2c_dead;

//-------- Основные регистры состояния электроавтоматики машинки ------------------
unsigned char NState=0;       // Neutral Relay state
unsigned char HeatingState=0; // Heating Relay state
unsigned char RotorState=0;   // Rotor triac 0..0xFF state
unsigned char ForwardState=0; // Forward Relay State
unsigned char ReverseState=0; // Reverse Relay State
unsigned char PompState=0;    // Pomp triac state
unsigned char WaterState=0;   // Water triac state
unsigned char LockState=0;    // Lock triac state

//------- Вспомогательные режимы -----------------------------------
unsigned char ClockMode=0;  // Режим показания часов минуты/часы
unsigned char TempMode=0; // Режим показания температуры градусы/АЦП

unsigned int adcLine;  // АЦП наличия фазы (LINE) после включения замка блокировки
unsigned int adcTemperature; // Значение АЦП Температуры
unsigned char temperature;  // Значение температуры в градусах
volatile unsigned char SetTemperature; // Значение установленной температуры для нагрева воды

unsigned char PWMCount;

unsigned short BeepCounter;
//===============================
volatile unsigned char FlagTacho;
volatile unsigned int Tacho;

//------ Timer
volatile unsigned char msec;
volatile unsigned char sec;
volatile unsigned char minute;
volatile unsigned char hour;
volatile unsigned char timer; // Таймер режимов стирки (max 255 с)

volatile unsigned char ProgramNumber;  // Номер выполняемой программы (0 - программа не выполняется)
volatile unsigned char ProgramCounter; // Счетчик кадров
volatile unsigned char SubState;  // Сотояние подпрограммы кадра
volatile unsigned char __flash* zprg; // Указатель на таблицу программ
volatile unsigned char cmd; // Регистр команды
volatile unsigned char t;    // Время выполнения команды
volatile unsigned char p;    // Параметр команды

//===================================================
//--------- Slave memory I/O registers (4+3+4+9+3 = 23)
// Область памяти, которая отображается на дисплее
// 20 байт
typedef union  //  this is the same data structure used by the client
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
volatile unsigned char Reg_Prog;   // Регистр номера выбранной программы в соотв. с АЦП
volatile unsigned char Reg_But1;   // Buttons Register 1
volatile unsigned char Reg_But2;   // Buttons Register 2
unsigned char MaskProg = 0xFF; // Байт маски для индикации 0xFF - индикация работает; 0 - не работает 
unsigned char BufRead[3]; // Временный буфер для чтения

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
#define DIG_d 0xA1
#define DIG_n 0xAB
#define DIG_o 0xA3
#define DIG_r 0xAF
#define DIG_t 0x87
#define DIG__ 0xF7
#define DIG_MINUS 0xBF

static __flash char HEX[16]=
{
  DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9,
  DIG_A, DIG_B, DIG_C, DIG_D, DIG_E, DIG_F
};

static __flash unsigned int DEC[100]=
{
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
  DIG_9<<8|DIG_6, DIG_9<<8|DIG_7, DIG_9<<8|DIG_8, DIG_9<<8|DIG_9 
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
#define KEY_DELAY (-20) // 50 мс
// Задержка до первого повтора
#define KEY_REPDEL (100) // 60
// Задержка на автоповтор
#define KEY_REPPER (78) // 48

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

#define KEY_PRESSED_SHORT (1)
#define KEY_PRESSED_LONG  (10)

volatile unsigned char Key=0x00; //Код нажатой клавиши, 0 - нет нажатия
unsigned char OldKey=0x00; //Старый код клавиши, 0 - все кнопки отпущены
signed char KeyCounter = KEY_DELAY; // Счетчик для задержек клавиатуры
volatile unsigned char KeyFlag=0;
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

//unsigned char __eeprom *zp;// = (unsigned char __eeprom *) 0x00;

/*
//----------------------------------------------------------------------------
// Store to EEPROM
void StoreEEPROM (unsigned char __eeprom* address, unsigned char byte)
{
//  unsigned char intState = __save_interrupt();
  __disable_interrupt();
  *address = byte;
  __enable_interrupt();  
}

//---------------------------------------------------------------------------
// Load from EEPROM
unsigned char LoadEEPROM (unsigned char __eeprom* address)
{
  unsigned char byte;
//  __disable_interrupt();
  byte = *address;;
//  __enable_interrupt();  
  return (byte);
}

*/


//-------------------------------------------------
// Пауза по таймеру прерываний
void delay_timer (unsigned char time)
{
  timer=time; // timer обрабатывается в прерываниях
  while (timer);
}


//-----------------------------------------------------------------------------
// Запускает/останавливает программу
// В параметры передается номер программы и указатель на таблицу программы
// Также отображается Start/Stop программы во время старта, а также
// выдается звуковой сигнал. По останову сбрасываются регистры состояния
// электроавтоматики. Состояние выполнения программы записывается
// в EEPROM.
void ProgramStartStop (unsigned char PrgNum, unsigned char __flash* TableProg)
{
  Timer_Buzzer = KEY_PRESSED_LONG;  // Пропищим

  if (ProgramNumber) // Если программа работает, то светим STOP и сбрасываем PC
  {
    MaskProg = 0xFF; // Восстанавливаем индикацию, если она была отключена (в режиме ожидания door)
    ProgramNumber = 0;
//    StoreEEPROM(0x00, ProgramNumber);

    // Сразу сбрасываем все регистры электроавтоматики после нажатия ОТМЕНА.
    // А также выходные ноги контроллера т.к. их состояние не сбросится в delay_timer.
    // это чуть длиннее, но надежнее.
    HeatingState=0; HEATING=0;
    RotorState=0;
    ForwardState=0; FORWARD=0;
    ReverseState=0; REVERSE=0;
    PompState=0; POMP=0;
    WaterState=0; WATER=0;
    LockState=0; LOCK=0;
    Reg.Seg1 = DIG_S;
    Reg.Seg2 = DIG_t;
    Reg.Seg3 = DIG_o;    
    Reg.Seg4 = DIG_P;    
    i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
    delay_timer(2); // Покажем на две секунды
  }
  else  // если программа не запущена, то светим Good и устанавливаем PC
  {
    ProgramNumber = PrgNum; // Номер программы (нужен фактически, чтобы знать программа работает или нет)
    zprg = TableProg; // Указатель на выполняемую программу
    ProgramCounter = 0;
    SubState = 0;
    Reg.Seg1 = DIG_6;
    Reg.Seg2 = DIG_o;
    Reg.Seg3 = DIG_o;    
    Reg.Seg4 = DIG_d;    
    i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
    delay_timer(2);
//    StoreEEPROM(0x00, ProgramNumber);
  }  
}

//--------------------------------------------------------------
// Процедура обработчика программы стирки

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

      // 3 bytes
    case PRG_WaterState:
      zprg++;
      WaterState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes
    case PRG_PompState:
      zprg++;
      PompState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes
    case PRG_HeatingState:
      zprg++;
      HeatingState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes
    case PRG_RotorState:
      zprg++;
      RotorState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes
    case PRG_ForwardState:
      zprg++;
      ForwardState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 3 bytes
    case PRG_ReverseState:
      zprg++;
      ReverseState = *zprg++;
      t = *zprg++;
      if (t!=0) timer = t;
      ProgramCounter++;
      break;

      // 2 bytes - без таймера
    case PRG_Buzzer:
      zprg++;
      Timer_Buzzer = *zprg++;
      ProgramCounter++;
      break;

      // 3 bytes
      // Составная подпрограмма.
      // Включение мотора вперед,
      // плавный разгон мотора до оборотов в пределах 0x41...0x50 (второй байт)
      // третий байт - время работы до плавного торможения.
      // Использовать для циклов "деликатный отжим" и "интенсивный отжим".
      // В параметрах скорости: 0x41 - деликатный отжим, 0x50 - интенсивный отжим.
    case PRG_MotorSpin:
      {
        switch (SubState)
        {
        case 0:
          ForwardState = 0xFF; // Включить мотор вперед
          SubState++;
          break;
        case 1:
          RotorState = 0x00;
          timer=1;
          SubState++;
          break;
        case 2:
          RotorState = 0x28; // Начальная нагрузка
          timer=1;
          SubState++;
          break;
        case 3:
          RotorState = 0x32; // Минимальные обороты, с которых трогается бак
          timer=2;
          SubState++;
          break;
        case 4:
          RotorState = 0x37;  // Малые обороты
          timer=3;
          SubState++;
          break;
        case 5:
          RotorState = 0x41;  // Обороты средние, можно использовать для деликатного отжима
          timer=5;
          SubState++;
          break;
        case 6:
          zprg++;
          RotorState = *zprg++; // Здесь задаем параметр оборотов (желательно не более 0x50)
          timer = *zprg;  // Время отжима (максимум 255 с)
          zprg = zprg - 2;  // Вернули указатель на команду, чтобы продолжить подпрограмму
          SubState++;
          break;
        // Тормозим также как и разгонялись
        case 7:
          RotorState = 0x41;
          timer=5;
          SubState++;
          break;
        case 8:
          RotorState = 0x37;
          timer=1;
          SubState++;
          break;
        case 9:
          RotorState = 0x32; 
          timer=1;
          SubState++;
          break;
        case 10: 
          RotorState = 0x00;
          timer=7;
          SubState++;
          break;
        case 11: 
          ForwardState = 0x00; // Выключить мотор вперед
          SubState=0;
          ProgramCounter++;          
          zprg = zprg + 3; // Указатель на следующую команду
        }
        break;
      }

      
      // 3 bytes
      // Составная подпрограмма для циклов стирки.
      // ZigZag - туда-сюда.
      // Плавный разгон мотора влево до оборотов в пределах 0x37...0x3С (второй байт), пауза,
      // плавное торможение, пауза. Повтор цикла с движением мотора вправо.
      // т.е. сначала в одну сторону, потом в другую.
      // третий байт - время работы в промежутках между разгоном и торможением, а также паузы.
      //  ____      ____
      // /    \____/    \____
      // Использовать для циклов "обычная стирка" и "интенсивная стирка".
      // В параметрах скорости: 0x37 - обычная стирка, 0x3С - интенсивная стирка.
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
          RotorState = 0x32;  // Малые обороты
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
          RotorState = 0x32;
          timer=2;
          SubState++;
          break;
        case 5:
          RotorState = 0x00;
          timer=5;          // 5 секунд пауза
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
          RotorState = 0x32;
          timer=2;
          SubState++;
          break;
        case 9:
          RotorState = p;
          timer=t;
          SubState++;
          break;
        case 10:
          RotorState = 0x32;
          timer=2;
          SubState++;
          break;
        case 11:
          RotorState = 0x00;
          timer=5;          // 5 секунд пауза
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

      // 2 bytes
      // Ожидание нагрева воды до заданной температуры.
      // Перед этой командой нужно включить ТЭН, чтобы температура temperature
      // увеличивалась. Также нужно проверить уровень воды в баке.
      // В секции индикации, если включен ТЭН, то будет отображаться температура воды
    case PRG_TWait:
      {
        switch (SubState)
        {
        case 0: // Зашли один раз за параметром, далее только проверка
          zprg++;
          SetTemperature = *zprg; // Читаем параметр заданной температуры
          zprg--;  // Вернули указатель на команду, чтобы продолжить подпрограмму
          SubState++;
        case 1:      
          if (SetTemperature >= temperature) // Если требуемая t больше или равна текущей, то ОК, едем дальше
          {
            SubState=0;
            ProgramCounter++;
            zprg = zprg + 2;  // указатель на следующую команду
          }
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
        zprg++;  // увеличивем указатель команд
      }
      else
      {
        BeepCounter++;
        // Ненавязчиво пикаем, намекая, что пора уже и обратить внимание
        if (BeepCounter == 0 || BeepCounter == 0x100)     Timer_Buzzer = 10;
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
        
      if (adcLine>0x17F) // Если сигнал появился, то 
      {
        MaskProg = 0xFF; // Включаем индикацию режимов (убирает индикацию door)
        ProgramCounter++;
        zprg++;  // увеличивем указатель команд
      }
      else
      {
        BeepCounter++;
        // Ненавязчиво пикаем, намекая, что пора уже и обратить внимание
        if (BeepCounter == 0 || BeepCounter == 0x100) Timer_Buzzer = 10;
      }
      break;
      
      // 1 byte
    case PRG_End:       
      ProgramNumber=0;
//        StoreEEPROM(0x00, ProgramNumber);
      Timer_Buzzer=50;  // Первый писк
      // Индикация конца выполнения программы
      Reg.Seg1 = DIG_E;
      Reg.Seg2 = DIG_n;
      Reg.Seg3 = DIG_d;
      Reg.Seg4 = DIG_SPACE;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
      delay_timer(2);
      Timer_Buzzer=25;  // Второй писк
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

















/*
void Programmer (unsigned char __flash* prg )
{
  ProgramCounter=0;
  unsigned char p;
  unsigned char cmd;
  unsigned char t;
  do
  {
    cmd = *prg++;
    
    if (cmd==PRG_Pause)
    {
      p = *prg++;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }

    if (cmd==PRG_LockState)
    {
      p = *prg++;
      LockState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }

    if (cmd==PRG_WaterState)
    {
      p = *prg++;
      WaterState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }

    if (cmd==PRG_PompState)
    {
      p = *prg++;
      PompState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }
    
    if (cmd==PRG_HeatingState)
    {
      p = *prg++;
      HeatingState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }

    if (cmd==PRG_RotorState)
    {
      p = *prg++;
      RotorState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }
    
    if (cmd==PRG_ForwardState)
    {
      p = *prg++;
      ForwardState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }
    
    if (cmd==PRG_ReverseState)
    {
      p = *prg++;
      ReverseState = p;
      t = *prg++;
      if (t!=0) delay_timer(t);
      ProgramCounter++;
    }

  }
  while (cmd!=PRG_End); // Выполняем пока не конец
  
}

*/





