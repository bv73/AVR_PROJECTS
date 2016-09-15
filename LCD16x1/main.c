// Процедуры вывода на LCD 16 символов одна строка
// Контроллер LCD Toshiba T7934
// MCU ATMEGA128 плата стартеркита от Космодрома

#include "iom128.h"
#include "inavr.h"
#include "clib\pgmspace.h"
#include "stdio.h"

#define delay_us(V) __delay_cycles(V*4L)

char CON[16+1];

void InitPorts(void)
{
  DDRA=0xFF; // Все на вывод
  DDRB=0xFF; // Все на вывод
  DDRC=0xFF; // Все на вывод
  DDRD=0xFF; // Все на вывод
  DDRE=0xFF;
  DDRF=0xFF;
  PORTD=0x7F; // Сброс светодиодов, один горит, второй не горит
}

#pragma inline=forced
void LCDstrobe(void)
{
  PORTE_Bit2=1;
  __no_operation();
  __no_operation();
  PORTE_Bit2=0;
}

//Посылает в ЖКИ комманду
void SendCmdLCD(char c)
{
  PORTC_Bit6=0;   // RS=0
  PORTC_Bit5=0;   // RW=0
  PORTA=(c&0xF0);
  LCDstrobe();
  PORTA=(c&0x0F)<<4;
  LCDstrobe();
  delay_us(50);
}

//Посылает в ЖКИ символ
void SendDataLCD(char c)
{
  PORTC_Bit6=1;   // RS=1
  PORTC_Bit5=0;   // RW=0
  PORTA=(c&0xF0);
  LCDstrobe();
  PORTA=((c&0x0F)<<4);
  LCDstrobe();
  delay_us(50);
}

//Начальная инициализация ЖКИ
void InitLCD(void)
{
  char c=1;
  do
  {
    delay_us(3000);
  }
  while(--c);
  PORTA=0x30;
  LCDstrobe();
  delay_us(6000); // 6 ms Delay
  PORTA=0x30;
  LCDstrobe();
  delay_us(1000); // 1 ms Delay
  PORTA=0x30;
  LCDstrobe();
  delay_us(1000); // 1 ms Delay
  PORTA=0x20;     // Set 4 bit interface
  LCDstrobe();
  delay_us(50);
  SendCmdLCD(0x28); // Set Function - 4 bit data, 2 Line mode, 5x7 dots
  SendCmdLCD(0x08); // Display OFF, Cursor OFF, Blink Cursor OFF
  SendCmdLCD(0x06); // Set Entry Mode, Addr Increment, Display not shifted
}

void UpdateConsole(void)
{
  char *p;
  char i;
  char c;

  SendCmdLCD(0x0C); //Выключаем курсор
  SendCmdLCD(128); //Позиция 0
  p=CON;
  i=8*2;
  do
  {
    if (i==8) SendCmdLCD(192); //Позиция 40 (вторая половина экрана)
    c=*p++;
    if (c==0) c=32;
//    if (c&128) c=Win2LCDTable[c-192];
    SendDataLCD(c);
  }
  while(--i);
  
}

void ClrScr(void)
{
  memset(CON,' ',sizeof(CON));
}

void PrintHello(void)
{
  ClrScr();
  sprintf(CON,"Test-2test-3test");
  UpdateConsole();
}

void main( void )
{
  InitPorts();
  InitLCD();
  PrintHello();

  while(1);
  //  return 0;
}


