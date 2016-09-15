// Процедуры вывода на LCD 16 символов две строки
// LCD 1602_04_S02_E00
// MCU ATMEGA2561 плата стартеркита от Космодрома

#include "iom2561.h"
#include "inavr.h"
#include "pgmspace.h"
#include "stdio.h"

#define delay_us(V) __delay_cycles(V*14L)

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#define EN PORTE_Bit2
#define RW PORTC_Bit5
#define RS PORTC_Bit6
#define sbi(port,bit)	(port |=  (1<<bit))
#define cbi(port,bit)	(port &= ~(1<<bit))


char CON[16*2+1];
__no_init char SRAM[0x6F00] @ 0x1100; // Start 0x1100 Len 0x6F00
__no_init char CODEREG[1] @ 0xF000;   // CODE Register of LCD
__no_init char DATAREG[1] @ 0xF001;   // DATA Register of LCD

void InitPorts(void)
{
  DDRA=0xFF; // Все на вывод
  DDRB=0xFF; // Все на вывод
  DDRC=0xFF; // Все на вывод
  DDRD=0xFF; // Все на вывод
  DDRE=0xFF;
  DDRF=0xFF;
  PORTD=0x7F; // Сброс светодиодов, один горит, второй не горит
  EN=0;
  RW=0;
}

//#pragma inline=forced
void LCDstrobe(void)
{
  EN=1;
  delay_us(10);
  EN=0;
}

//Посылает в ЖКИ команду
void SendCmdLCD(char c)
{
  RS=0;
//  RW=0;
  delay_us(70);
  PORTA=(c&0xF0);
  LCDstrobe();
  PORTA=((c<<4)&0xF0);
  LCDstrobe();

}

//Посылает в ЖКИ символ
void SendDataLCD(char c)
{
  RS=1;
//  RW=0;
  delay_us(70);
  PORTA=(c&0xF0);
  LCDstrobe();
  PORTA=((c<<4)&0xF0);
  LCDstrobe();

}

//Начальная инициализация ЖКИ
void InitLCD(void)
{
  delay_ms(50);
  RW=0;
  PORTA=0x30;
  LCDstrobe();
  delay_ms(6); // 6 ms Delay
  PORTA=0x30;
  LCDstrobe();
  delay_ms(1); // 1 ms Delay
  PORTA=0x30;
  LCDstrobe();
  delay_ms(1); // 1 ms Delay
  PORTA=0x20;     // Set 4 bit interface
  LCDstrobe();
  delay_ms(1);
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
  i=16*2;       // Сколько символов
  do
  {
    if (i==16) SendCmdLCD(192); //Позиция 40 (вторая строка)
    c=*p++;
    if (c==0) c=32; // Если конец строки, то превращаем в пробел
    SendDataLCD(c);
  }
  while(--i);
  
}

void ClrScr(void)
{
  memset(CON,' ',sizeof(CON));
}

void *memmo(void *s, int c, size_t n)
{       /* store c throughout unsigned char s[n] */
   const unsigned char uc = c;
   unsigned char *su = (unsigned char *)s;

   for (; 0 < n; ++su, --n)
     *su = uc;
   return (s);
}


void main( void )
{
  char d;
  char *sram;
  char *codereg;
  char *datareg;
  sbi (MCUCR,SRE);    // Enable SRAM
  memset(SRAM,0xAA,sizeof(SRAM));
  cbi (MCUCR,SRE);    // Disable SRAM
//  float a,b,c;
  InitPorts();
  InitLCD();
  ClrScr();
//  a=9;
//  b=3.14159;
  sram=SRAM;
  codereg=CODEREG;
  do 
  {
//  c=a/b;
//  MCUCR |= (1<<SRE);
    sbi (MCUCR,SRE);    // Enable SRAM
    d=*sram++;
    *codereg=d;
    *datareg=d;
    cbi (MCUCR,SRE);    // Disable SRAM
//  sprintf(CON,"%.12f",c);
    sprintf (CON, "%X",sram);
    sprintf(CON+16,"%X",d);
    UpdateConsole();
//  delay_us(20000);
//  a=a-0.1;
  } while (sram < (SRAM+sizeof(SRAM)) );

  while(1)
  {
    PORTD=0x00;
    delay_ms(100);
    PORTD=0xFF;
    delay_ms(100);
   };
  //  return 0;
}


