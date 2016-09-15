// LCD 640x200 by (R)soft 16.5.2011
// LCD on T6A39 & T6A40 driver chips
// Version 1.0
// Last edition 16.5.2011
// -----------------------
// Mega128 KIT Connectors:
// (X8) PORTD
// CLOCK      -> PD0 CP (XSCL, CP2, Shift Clock) ~60 MHz
// LATCH      -> PD1 Load (Latchpuls, LP, YSCL, CP1, HSync) ~31,5 kHz
// DISPLAY    -> PD2
// FRAME      -> PD3 Frame (FLM, Scan Start, VSync) ~ 130 Hz
// no connect -> PD4
// no connect -> PD5
// LED1       -> PD6
// LED2       -> PD7
// ----------------------
// (X7) PORTC
// D0 -> PC0 Data DOWN Half Bit 0
// D1 -> PC1
// D2 -> PC2
// D3 -> PC3
// U0 -> PC4 Data UP Half Bit 0
// U1 -> PC5
// U2 -> PC6
// U3 -> PC7
// ---------------------

#include "iom128.h"
#include "inavr.h"
#include "pgmspace.h"
#include "stdio.h"

#define delay_us(V) __delay_cycles(V*8L)
#define FALSE (0)
#define TRUE  (!FALSE)

#define CLOCK PORTD_Bit0
#define LATCH PORTD_Bit1
#define DISPLAY PORTD_Bit2
#define FRAME PORTD_Bit3
#define LED1 PORTD_Bit6
#define LED2 PORTD_Bit7

// За какие лапки будем дергать LCD
#define NOKIA_D_C    PORTA_Bit0 // Data/Command Bit
#define NOKIA_SCK    PORTA_Bit1 // Clock Data
#define NOKIA_DOUT   PORTA_Bit2 // Serial Data
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

// Задержка в милисекундах
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

void StrobeLCD (void)
{
  CLOCK=1;
  delay_us(5);
  CLOCK=0;
  delay_us(5);
}
void LatchLine (void)
{
unsigned char i;
for (i=5; i; i--)
  { PORTC=0xFF; delay_us(5); StrobeLCD();
  }

}

int main( void )
{
  int CounterLED=0;
  char CountSW=0;
  char ModeSW1=0;
  char ModeSW2=0xFF; // Pause or Meandr Switcher
  char ModeSW3=0;
  char ModeSW4=0;
  char PCB_Mode=0;
  InitPorts();
  InitUART();
  _pch (0x31); // Вывод байта в последовательный порт
  nokia_init();
  ClrScr(); // Чистим буфер
  nokia_cls();
  nokia_gotoxy(0,0);
  sprintf (CON,   "LCD 640x200"); // Вывод приветствия и версии программы
  sprintf (CON+14,"Тестовая плата"); 
  sprintf (CON+28,"v1.0 16/5/2011");
  sprintf (CON+42,"Кнопки");
  sprintf (CON+56,"[1][2][3][4]");
  sprintf (CON+70,"Выбор режима");
  UpdateConsole();
  ClrScr(); // Чистим буфер LCD после вывода приветствия

  CLOCK=0;
  LATCH=0;
  DISPLAY=1;
  FRAME=0;
  PORTC=0xA5; 
  
  while(1) 
  {
  FRAME=~FRAME;
    
  LatchLine();
  LATCH=1;  delay_us(5);  LATCH=0;
  LatchLine();
  LATCH=1;  delay_us(5);  LATCH=0;
  LatchLine();
  LATCH=1;  delay_us(5);  LATCH=0;
  LatchLine();
  LATCH=1;  delay_us(5);  LATCH=0;
  
  
  char KEY=Inkey(); // KEY = 1, 2, 4, 8 или 0 - не нажата
  if (KEY==1) // Переключение и задание режимов в зависимотси от нажатой клавиши
    { PCB_Mode=1; 
      ModeSW1=~ModeSW1;
      if (ModeSW1==0xFF) { sprintf(CON+70,"Режим CLOCK=0 "); CLOCK=0; }
      else               { sprintf(CON+70,"Режим CLOCK=1 "); CLOCK=1; }
      UpdateConsole(); 
      delay_ms(300); 
    }
  if (KEY==2) 
    { PCB_Mode=2; 
      ModeSW2=~ModeSW2; // Инверсия режима
      if (ModeSW2==0xFF) { sprintf(CON+70,"Режим LATCH=0 "); LATCH=0; }
      else               { sprintf(CON+70,"Режим LATCH=1 "); LATCH=1; }
      UpdateConsole(); 
    delay_ms(300);
    }
  if (KEY==4) 
    { PCB_Mode=3; 
      ModeSW3=~ModeSW3;
      if (ModeSW3==0xFF) { sprintf(CON+70,"Режим Disp OFF"); DISPLAY=0; }
      else               { sprintf(CON+70,"Режим Disp ON "); DISPLAY=1; }
      UpdateConsole();
      delay_ms(300);
    }
  if (KEY==8) 
    { PCB_Mode=4;
      ModeSW4=~ModeSW4;
      if (ModeSW4==0xFF) { sprintf(CON+70,"Режим FRAME=0 "); FRAME=0; }
      else               { sprintf(CON+70,"Режим FRAME=1 "); FRAME=1; }
      UpdateConsole();
      delay_ms(300);
    }  
  
  CounterLED++;
  if (CounterLED==15550) { CountSW=~CountSW; CounterLED=0; }
  if (CountSW==0) { LED1=0; LED2=1; }  else { LED1=1; LED2=0; } // Блымает светодиодами

   }
}
