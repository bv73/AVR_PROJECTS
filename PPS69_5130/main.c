// Test PPS69 with Nokia 5130 LCD by (R)soft 30.9.2010
// Version 1.8
// Last edition 30.9.2010
// -----------------------
// Test Board Connectors:
// X1.1 SD1 -> PD0 (X8)
// X1.2 CLK -> PD1
// X1.3 A0  -> PD2
// X1.4 A1  -> PD3
// X1.5 SD2 -> PD4
// X1.6 SD3 -> PD5
// ----------------------
// X2.1 BX_SD -> PC0 (X7)
// X2.2 TAKT# -> PC1
// X2.3 CDB   -> PC2
// X2.4 WR#   -> PC3
// X2.5 RD    -> PC4
// ---------------------
// X3.1 D0.1    -> PB0 (X6)
// X3.2 D1.1    -> PB1
// X3.3 D0.2    -> PB2
// X3.4 D1.2    -> PB3
// X3.5 D2.0    -> PB4
// X3.6 D2.1    -> PB5
// X3.7 CARRY5  -> PB6
// X3.8 CARRY5# -> PB7

#include "iom128.h"
#include "inavr.h"
#include "pgmspace.h"
#include "stdio.h"

#define delay_us(V) __delay_cycles(V*8L)
#define FALSE (0)
#define TRUE  (!FALSE)

#define SDAT1 PORTD_Bit0
#define CLK PORTD_Bit1
#define A0 PORTD_Bit2
#define A1 PORTD_Bit3
#define SDAT2 PORTD_Bit4
#define SDAT3 PORTD_Bit5
#define LED1 PORTD_Bit6
#define LED2 PORTD_Bit7

#define BX_SD PORTC_Bit0
#define TAKT PORTC_Bit1
#define CDB PORTC_Bit2
#define MODE PORTC_Bit3
#define RD PORTC_Bit4

// За какие лапки будем дергать LCD
#define NOKIA_D_C    PORTA_Bit0 // Data/Command Bit
#define NOKIA_SCK    PORTA_Bit1 // Serial Data
#define NOKIA_DOUT   PORTA_Bit2 // Clock Data
#define NOKIA_CS     PORTA_Bit3 // Chip Select
#define NOKIA_RESET  PORTA_Bit4 // Reset LCD
#define NOKIA_BLIGHT PORTA_Bit5 // Back Light LCD

#define GLCD_PIXELX (84)
#define GLCD_PIXELY (48)

// Команды ЖКИ (Normal Functionset)
#define GLCD_FUNCTIONSET    (0x20)
#define GLCD_SETYADDR       (0x40)
#define GLCD_SETXADDR       (0x80)
#define GLCD_DISPLAYBLANK   (0x08)
#define GLCD_DISPLAYFLUSH   (0x09)
#define GLCD_DISPLAYNORMAL  (0x0C)
#define GLCD_DISPLAYINVERT  (0x0D)

// Команды ЖКИ (Extended Functionset)
#define GLCD_FUNCTIONSETEXT (0x21)
#define GLCD_SET_BIAS       (0x10)
#define GLCD_SET_VOP        (0x80)
#define GLCD_TEMPCOEF       (0x06)  // TF=2

#define GLCD_BIAS_100       (0x00) // 1:100
#define GLCD_BIAS_80        (0x01) // 1:80
#define GLCD_BIAS_65        (0x02)
#define GLCD_BIAS_48        (0x03)
#define GLCD_BIAS_40        (0x04)
#define GLCD_BIAS_24        (0x05)
#define GLCD_BIAS_18        (0x06)
#define GLCD_BIAS_10        (0x07)

// Шрифт
static __flash char Font5x7[]=
{ 
#include "font_5x7.inc"
};

// Основные переменные LCD
volatile unsigned char nokia_x=0; // Позиция X
volatile unsigned char nokia_y=0;
volatile unsigned char nokia_contrast=0; // Значение контрастности
volatile unsigned char nokia_inverse=0; // Инверсия вывода (0 - выключена)

volatile char InvError=0; // Флаг ошибки инверсии
char CON [14*6+1]; // CONsole buffer

volatile char ErrCnt=0;  // Счетчик ошибок
long CounterSD=0;       // Счетчик задержки вывода сигнатур на LCD
#define PSDMAX 100000  // 100000 - Определение максимального значения порога зедержки (для визуального сравнения)
#define PSDMIN 10    // 10 -Минимальная задержка вывода 
long PorogSD=PSDMIN; // Начальное значение порога задержки переключения сигнатур

// Для исполнения платы 069 значение 10000 (даже 1000), для исполнения 069-01 значение 20000 (не определено)
#define DelayC7 1000

// ======================= LCD 5130 Routine ============================
// Запись байта в ЖКИ
// Сначала MSB (первым идет старший бит)
void nokia_w (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) NOKIA_DOUT=1; // Анализируется самый старший бит в байте
    else  NOKIA_DOUT=0;        // и выставляется SDA в соответствии
    NOKIA_SCK=0; // Запись по срезу SCLK
    NOKIA_SCK=1; // Возвращаем SCLK обратно в исходное состояние
    ch<<=1; // Сдвигаем влево
  }
}

// Отправка команды в ЖКИ
void nokia_cmd(unsigned char cmd)
{
  NOKIA_D_C=0; // Признак команды D/C=0
  nokia_w(cmd);
}

// Отправка данных в ЖКИ
void nokia_data(unsigned char dat)
{
  NOKIA_D_C=1; // Признак данных D/C=0
  nokia_w(dat);
  if (++nokia_x>GLCD_PIXELX-1) // Модифицируем позицию в экранном буфере
  {
    nokia_x=0; // Если был достигнут конец экрана то следующая строка
    nokia_y++;
  }
}

// Установка адреса в ЖКИ
void nokia_set_addr(unsigned char x, unsigned char y)
{
  if (y>5) y=5; // Проверка максимального значения строки (не больше 5!)
  if (x>GLCD_PIXELX-1) x=GLCD_PIXELX-1; // Проверка максимального значения столбца
  nokia_x=x;
  nokia_y=y;
  nokia_cmd(GLCD_SETYADDR | y);
  nokia_cmd(GLCD_SETXADDR | x);
}

// Переход к координатам x,y
void nokia_gotoxy(unsigned char x, unsigned char y)
{
  nokia_set_addr(((x<<1)+x),y); // nokia_set_addr(x*6,y)
}
// Вычисление начала символа в таблице шрифта
unsigned int char_to_pos(unsigned char ch)
{
  unsigned int pos;
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  pos=ch;
  pos+=(pos<<2); // *5
  return pos;
}

// Вывод символа на ЖКИ
void nokia_putchar(unsigned char ch)
{
  unsigned int pos = char_to_pos(ch); // Вычисление адреса начала символа
  for (ch=5; ch; ch--)  {  nokia_data(Font5x7[pos++]); } // Вывод 5 байт
  nokia_data(0); // Вывод промежутка между символами
}

// Запись в LCD сразу двух байт
void nokia_ddata(unsigned char b)
{
  nokia_data(b);
  nokia_data(b);
}

// Очистка экрана
void nokia_cls(void)
{
  unsigned char i;
  nokia_gotoxy(0,0); // Установка адреса позиции в ноль
  for (i=252; i; i--) // Цикл для записи двойных байт
    nokia_ddata(0); // Запись в LCD 504 пустых байта
  nokia_gotoxy(0,0);
}

// Вывод строки (строка в ОЗУ)
void nokia_puts (unsigned char *s)
{
  while (*s)
    nokia_putchar(*s++);
}
// Задать значение контраста
void nokia_set_contrast(unsigned char c)
{
  nokia_contrast=c;
  nokia_cmd(GLCD_FUNCTIONSETEXT);
  nokia_cmd(GLCD_SET_VOP | c);
  nokia_cmd(GLCD_FUNCTIONSET);
}

// Задать значение инверсии
void nokia_set_inverse(unsigned char inv)
{
  nokia_inverse=inv;
  if (inv)
    nokia_cmd(GLCD_DISPLAYINVERT);
  else
    nokia_cmd(GLCD_DISPLAYNORMAL);
}

void nokia_init (void)
{
  MCUCR=0x00;
  XMCRB=0x00;
  DDRA=0xFF; // Все на вывод
  NOKIA_SCK=1;
  NOKIA_CS=0; // Chip Select
  NOKIA_RESET=0;  // Reset LCD
  delay_us(500);
  NOKIA_RESET=1;  // Return Reset OFF
  delay_us(500);
  NOKIA_BLIGHT=1; // Set Back Light
  nokia_cmd(GLCD_FUNCTIONSETEXT); // Step 2 (Extended instruction Set H=1 Mode)
  // Step 3 Set VOP
  // Step 4 Set Normal Instruction Set H=0 (0x20)
  nokia_cmd(GLCD_TEMPCOEF);
  nokia_cmd(GLCD_SET_BIAS | GLCD_BIAS_10);
  nokia_set_contrast(120);
  nokia_set_inverse(FALSE);
  nokia_cls();
}

// =============================================================================================

void InitPorts (void)
{
  DDRA=0xFF; // All Output
  DDRB=0x00; // Port B for Input
  DDRC=0xFF;
  DDRD=0xFF;
  DDRE=0x0B; // Keys Line for Input And Busy Input (PE2), RXD0&TXD0 output
  DDRF=0xFF;
  PORTB=0x00; // FF - pullup, 00 - input
  PORTC=0x00;
  PORTD=0x00; // Set Port D
  PORTE=0xFF; // For Keys & Busy
  TAKT=0; // Set TAKT=0 & MODE=1
  MODE=1;
}

// Используем UART0 для приема и передачи информации
void InitUART (void)
#define FOSC 8000000
#define Baudrate 1200
#define UBRR_Value (FOSC/(16*Baudrate))-1
{  
  UBRR0H=UBRR_Value>>8; // Задание скорости UART0
  UBRR0L=UBRR_Value&0xFF;
  UCSR0A=(0<<U2X0); // Задание асинхронного режима и Double Speed
  UCSR0B=(1<<RXEN0)+(1<<TXEN0); //  Включение приема и передачи
}

// Отправка байта в последовательный порт
void _pch(char c)
{
  while(!UCSR0A_UDRE0); // Ждем, пока регистр передачи будет пуст
  UDR0=c; // Ложим в регистр передачи байтик
}

void _ps_P(char __flash *p)
{
  char c;
  while ((c=*p++)) _pch(c);
}

// Выводит из буфера консоли CON на ЖКИ
void UpdateConsole(void)
{
  char *p;
  char i;
  char c;  
  nokia_gotoxy(0,0);
  p=CON;
  i=14*6; // How many symbols
  do
  {
    c=*p++;
    if (c==0) c=32; // If End of Line then "SPACE" character
    nokia_putchar(c);
  }
  while (--i);
}

// Чистим буфер пробелами
void ClrScr (void)
{
  memset (CON,' ',sizeof(CON));
}

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

/* ************************************************************** */

char Inkey (void)
{
  char  ckey=(~PINE&0xF0);
  ckey>>=4;
  return (ckey);
}

void StrobeShiftReg (void)
{
  CLK=1;
  delay_us(5);
  CLK=0;
  delay_us(5);
}

void SetShiftRegOnce (void)
  {
    SDAT1=1;
    SDAT2=1;
    SDAT3=1;
    StrobeShiftReg();
    SDAT1=1;
    SDAT2=1;
    SDAT3=1;
    StrobeShiftReg();
    SDAT1=0;
    SDAT2=0;
    SDAT3=0;
    StrobeShiftReg();
  }

void SetShiftReg164 (unsigned long tst)
{
  char HB=tst>>16;
  char MB=tst>>8;
  char LB=tst>>0;
  char cycle=8;
  do {
       if (LB&0x80) SDAT1=1;    
               else SDAT1=0; // Test Bit7 & Set LowByte Register
       if (MB&0x80) SDAT2=1;    
               else SDAT2=0; // Test Bit7 & Set MiddleByte Register
       if (HB&0x80) SDAT3=1;    
               else SDAT3=0; // Test Bit7 & Set HighByte Register
       StrobeShiftReg();
       HB<<=1; // Shift Left
       MB<<=1;
       LB<<=1;
     }
  while (--cycle);
}

unsigned long ShiftToPPS69 (unsigned long tst)
{
  unsigned long shft=0; // dword for read
  char d;
  char cycle=20; // 20 bit shifting

  CDB=1; // Enable Shift (logical AND with TAKT)
  delay_us(500);

  MODE=0; // Set Mode "Shift" and automatically Latch inputs to outputs
  delay_us(DelayC7); // 10 ms PAUSE for charging Capacitor C7 (0,15 uF)

  do {
       shft<<=1; // First Shift to Left Receiver
       if (tst&(1UL<<(20-1))) BX_SD=1; 
       else                   BX_SD=0;
       delay_us(1000); // Wait for stability BX_SD logical level

       TAKT=1; // Positive Front for Latch Data
       delay_us(1000);

       if (PINB&0x40) shft|=1; // Set Receiver low bit
       d=(PINB&0x80)!=0; // Проверка бита инверсии последовательного выхода
       if (((PINB&0x40)!=0)==d) InvError=1;
       TAKT=0; // Return to Low
       delay_us(500);
       tst<<=1;  // Shift Left output test dword
      }
  while (--cycle);

  MODE=1; // Return Mode to "Parallel"
  delay_us(DelayC7); // 10 ms

  CDB=0; // Disable Shift
  delay_us(500);
  return (shft);
}

unsigned long ReadFromPPS69 (void)
{
  char d;
  unsigned long rdw; // Reading dword
  rdw=0;
  RD=1; // Enable Out Data from PPS69

// First read from High bits (MUX addr set to 3 and decrement)  
  A0=1; // MUX Addr = 3 (First High bits)
  A1=1;
  delay_us(1000);
  d=(PINB&0x03); // Read from PINs after delay
  rdw=rdw|d;
  rdw<<=6; // First 6 bits

  A0=0; // MUX Addr = 2
  delay_us(100);
  d=(PINB&0x3F);
  rdw=rdw|d;
  rdw<<=6; // Second 6 bits

  A0=1; // MUX Addr = 1
  A1=0;
  delay_us(100);
  d=(PINB&0x3F);
  rdw=rdw|d;
  rdw<<=6; // Third 6 bits
  
  A0=0; // MUX Addr = 0
  delay_us(100);
  d=(PINB&0x3F);
  rdw=rdw|d; // Last 2 bits
  
  A1=0; // Return MUX Addr to 0
  RD=0; // Disable Out Data from PPS69
  delay_us(1000);
  return (rdw);
}

void Print20bit (char pos, unsigned long rdw)
// Print to CONsole into position pos double word only 20 bit
{
  char hrdw=(rdw>>16)&0xF; // Only One tetrade
  sprintf (CON+pos,"%X",hrdw);
  sprintf (CON+pos+1,"%04X",rdw);
}

void SendSignature (unsigned long TST_S, unsigned long TST_P)
{
  SetShiftReg164 (TST_P); // Prepare Parallel Inputs for PPS69
                          // On Outputs of Shift Register 74164 setting Parallel Combination
  Print20bit (0,TST_S);   // Print First Test Signature "S->P" from Left Up Corner
  sprintf (CON+5,"s");

  Print20bit (7,TST_P);   // Print Second Test Signature "P->S" from Left in Second Line
  sprintf (CON+12,"p");
  
  unsigned long RdTstSerial=ShiftToPPS69(TST_S); // TST_S serially shigting to parallel output
                                                 // and automatically reading TST_P from serial lines
                                                 // result to RdTstSerial
  Print20bit (14+7, RdTstSerial); // Print Result "S->P" from Right
  sprintf (CON+14+12,"s");
  unsigned long RdTstParallel=ReadFromPPS69(); // Read Parallel Data From MUXs
  Print20bit (14,RdTstParallel);  // Print Result "P->S" in Second Line
  sprintf (CON+14+5,"p");  
  if (ErrCnt!=255) // Если счетчик ошибок не достиг 255, то проверяем несовпадения
  {
    if (TST_S!=RdTstParallel) 
      { 
        ErrCnt++; // Подсчет кол-ва ошибок
        Print20bit(28,TST_S); // Печать ошибочных данных
        Print20bit(42,RdTstParallel);
      }
    if (TST_P!=RdTstSerial) 
      {
        ErrCnt++;
        Print20bit(28+7,TST_P);
        Print20bit(42+7,RdTstSerial);
      }
  }
  sprintf (CON+56,"Ошибок=%d",ErrCnt); // Вывод ко-ва ошибок на LCD
  UpdateConsole();
}

/* ********************************************************************************************************
Кнопка 1 - режим посылки и приема сигнатур на ППС69, на LCD выводятся 20-битные шестнадцатиричные
           сигнатуры в виде
AAAAAs 55555p
AAAAAp 55555s
Errors=00
Где в верхнем ряду указаны сигнатуры, которые посылаются на ППС69 - AAAAAs (последовательный сдвиг),
55555p (параллельный сдвиг). 
Во втором ряду соответственно указываются сигнатуры, которые принимаются из ППС69 - 
AAAAAp (принят параллельный код), 55555s (принят последовательный код), 
В третьем ряду показано 00 - количество ошибок несовпадений сигнатур (без обнуления).


В этом режиме при нажатии кнопки 4 переключается скорость вывода сигнатур на ППС69 и результатов на LCD.
--------------------------------------------------------------------------------------------------------
Кнопка 2 - режим паузы или задания меандра на всех входах ППС69 (для удобства работы с логическим 
           пробником и выявления неисправностей осциллографом путем контроля лог. уровней). 
           При первом нажатии утанавливается режим паузы. При втором нажатии задается режим меандра.
           Режимы отображаются на LCD.
--------------------------------------------------------------------------------------------------------
Кнопка 3 - задание статических лог. уровней на входах ППС69 с отображением результата на LCD.
--------------------------------------------------------------------------------------------------------
Кнопка 4 - переключение задержки посылки сигнатур и вывода результатов на LCD. Режим минимальной задержки
           предназначен для выявления нестабильных плат ППС69 при их максимальной нагрузке. Этот режим
           работает совместно с режимом 1. Вывод кол-ва ошибок позволяет автоматизировать процесс проверки.

*********************************************************************************************************** */
int main( void )
{
  int CounterLED=0;
  char CountSW=0;
  char ModeSW2=0xFF; // Pause or Meandr Switcher
  char ModeSW3=0;
  char ModeSW4=0;
  char PCB_Mode=0; // For Reading Text & Version
  char NumSig=0; // Номер сигнатуры
  InitPorts();
  InitUART();
  _pch (0x31); // Вывод байта в последовательный порт
  nokia_init();
  ClrScr(); // Чистим буфер
  nokia_cls();
  nokia_gotoxy(0,0);
  sprintf (CON,   "-=< ППС69 >=-"); // Вывод приветствия и версии программы
  sprintf (CON+14,"Тестовый стенд"); 
  sprintf (CON+28,"v1.8 5/10/2010");
  sprintf (CON+42,"Кнопки");
  sprintf (CON+56,"[1][2][3][4]");
  sprintf (CON+70,"Выбор режима");
  UpdateConsole();
  ClrScr(); // Чистим буфер LCD после вывода приветствия
  while(1) 
  {
    
  char KEY=Inkey(); // KEY = 1, 2, 4, 8 или 0 - не нажата
  if (KEY==1) // Переключение и задание режимов в зависимотси от нажатой клавиши
    { PCB_Mode=1; 
      ModeSW2=0xFF; // Установка режима Пауза после нажатия кнопки 2
      sprintf(CON+70,"Режим 1 Тест "); 
      UpdateConsole(); 
      delay_ms(300); 
    }
  if (KEY==2) 
    { PCB_Mode=2; 
    ModeSW2=~ModeSW2; // Инверсия режима, либо пауза, либо меандр
    if (ModeSW2==0xFF) { sprintf(CON+70,"Режим 2 Меандр"); }
    else               { sprintf(CON+70,"Режим 2 Пауза "); }
      UpdateConsole(); 
    delay_ms(300);
    }
  if (KEY==4) 
    { PCB_Mode=3; 
      ModeSW3=~ModeSW3;
      sprintf(CON+70,"Режим 3 Лог=%X",ModeSW3);
      UpdateConsole();
      delay_ms(300);
    }
  if (KEY==8) 
    { PCB_Mode=4;
      ModeSW4=~ModeSW4;
      delay_ms(300);
    }  

  if (InvError==1) { sprintf(CON,"Ошибка инверсии!!!"); UpdateConsole(); delay_ms (500); }
  
  if (PCB_Mode==1) // Первый режим шлет сигнатуры (кнопка 1)
                {
                  CounterSD++; // Counter Sign Delay
                  if (CounterSD==PorogSD) // Если счетчик досчитал, то проверяем выводить ли сигнатуру
                  { 
                    CounterSD=0; // Сбрасываем счетчик задержки вывода
                    NumSig++;  // Увеличиваем номер сигнатуры
#define SSDelay 10
                    if (NumSig==1) { SendSignature(0xAAAAA,0xAAAAA); delay_us (SSDelay); } // S->P & P->S
                    if (NumSig==2) { SendSignature(0x55555,0x55555); delay_us (SSDelay); }
                    if (NumSig==3) { SendSignature(0xFFFFF,0xFFFFF); delay_us (SSDelay); }
                    if (NumSig==4) { SendSignature(0x00000,0x00000); delay_us (SSDelay); }
                    if (NumSig==5) { SendSignature(0x11111,0xEEEEE); delay_us (SSDelay); }
                    if (NumSig==6) { SendSignature(0x22222,0xDDDDD); delay_us (SSDelay); }
                    if (NumSig==7) { SendSignature(0x44444,0xBBBBB); delay_us (SSDelay); }
                    if (NumSig==8) { SendSignature(0x88888,0x77777); delay_us (SSDelay); }
                    if (NumSig==9) { SendSignature(0xEEEEE,0x11111); delay_us (SSDelay); }
                    if (NumSig==10){ SendSignature(0xDDDDD,0x22222); delay_us (SSDelay); }
                    if (NumSig==11){ SendSignature(0xBBBBB,0x44444); delay_us (SSDelay); }
                    if (NumSig==12){ SendSignature(0x77777,0x88888); delay_us (SSDelay); }
                    if (NumSig==13){ SendSignature(0x0A0A0,0x50505); delay_us (SSDelay); }
                    if (NumSig==14){ SendSignature(0xCCCCC,0x33333); delay_us (SSDelay); }
                    if (NumSig==15){ SendSignature(0x33333,0xCCCCC); delay_us (SSDelay); NumSig=0; }
                  }
                }
  if (PCB_Mode==2) // Второй режим дрыгает всеми ногами на входе ППС69 (кнопка 2)
              {
                if (ModeSW2==0xFF)
                  {
                    SetShiftRegOnce();
                    BX_SD=1;
                    TAKT=1;
                    CDB=1;
                    MODE=1;
                    RD=1;
                    delay_us(50);
                    BX_SD=0;
                    TAKT=0;
                    CDB=0;
                    MODE=0;
                    RD=0;
                    delay_us(50);
                  }
               }

  if (PCB_Mode==3)   // Третий режим (кнопка 3) - пауза с выводом лог. уровней 0/1 на все входы ППС69
                      // Каждое нажатие кнопки изменяет состояния лог. уровней
               {
                 if (ModeSW3==0xFF)
                 {
                  PCB_Mode=0; // Один раз выполнили, после этого крутим в пустом цикле
                  SetShiftReg164(0xFFFFF); // На всех входах данных ППС69 единицы
                  BX_SD=1;
                  TAKT=1;
                  CDB=1;
                  MODE=1;
                  RD=1;                
                 }
                 else 
                 {
                  PCB_Mode=0; 
                  SetShiftReg164(0x00000);
                  BX_SD=0;
                  TAKT=0;
                  CDB=0;
                  MODE=0;
                  RD=0;                                 
                  }
               }
  if (PCB_Mode==4)  // Четвертый режим (кнопка 4) - переключение задержки вывода сигнатуры
               {
                 PCB_Mode=1; 
                 CounterSD=0; // Если не сбросить то зависает после переключ. режимов
                 if (ModeSW4==0) { PorogSD=PSDMIN; }
                 else            { PorogSD=PSDMAX; }
               }  
  CounterLED++;
  if (CounterLED==50) { CountSW=~CountSW; CounterLED=0; }
  if (CountSW==0) { LED1=0; LED2=1; }  else { LED1=1; LED2=0; } // Блымает светодиодами

   }
}
