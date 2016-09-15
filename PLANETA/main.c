#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
//#include <stdio.h> // for sprintf

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

#define ADR0        PORTA_Bit0
#define ADR1        PORTA_Bit1
#define ADR2        PORTA_Bit2
#define AFK_SELECT  PORTA_Bit3
#define RAMA_N      PORTA_Bit4
#define FASP        PORTA_Bit5
#define RESET_N     PORTA_Bit6
#define EMPS_N      PORTA_Bit7

void delay_ms (unsigned int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

//-----------------------------------------
// Включение реле 8-битной платы
void set_relay8 (unsigned char numrelay)
{
  unsigned char tmp;
  tmp=PORTA&0xF8;
  PORTA=tmp|(numrelay&0x07);
  RAMA_N=0;
  FASP=0;
  delay_ms(1);
  FASP=1;
}  

//---------------------------------------------
// Выключение реле 8-битной платы
void reset_relay8 (unsigned char numrelay)
{
  unsigned char tmp;
  tmp=PORTA&0xF8;
  PORTA=tmp|(numrelay&0x07);
  RAMA_N=1;
  FASP=0;
  delay_ms(1);
  FASP=1;
}  

//-----------------------------------------
// Включение реле 16-битной платы
void set_relay16 (unsigned char numrelay)
{
  unsigned char tmp;
  tmp=PORTA&0xF0;
  PORTA=tmp|(numrelay&0x0F);
  RAMA_N=0;
  FASP=0;
  delay_ms(1);
  FASP=1;
}  

//---------------------------------------------
// Выключение реле 8-битной платы
void reset_relay16 (unsigned char numrelay)
{
  unsigned char tmp;
  tmp=PORTA&0xF0;
  PORTA=tmp|(numrelay&0x0F);
  RAMA_N=1;
  FASP=0;
  delay_ms(1);
  FASP=1;
}  

void main( void )
{


  DDRA=0xFF; // Все на выход
  RESET_N=0;
  delay_ms(100);
  RESET_N=1;
  delay_ms(100);
  
  ADR0=0;
  ADR1=0;  
  ADR2=0;
  AFK_SELECT=0;
  RAMA_N=0;
  FASP=0;
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char mode=0x00;
  unsigned char number=0x00;
//  unsigned char tmp;
  
  while (1)
  {

      set_relay16(mode);
      delay_ms(50);
      reset_relay16(mode);
      delay_ms(50);
      number++;
    
    
    if (!KEY1)
      {
        mode--;
        mode&=0x0F;

        delay_ms(10);
      }

    if (!KEY2)
      {
        mode++;
        mode&=0x0F;


        delay_ms(10);
      }

    if (!KEY3)
      {
        while (1)
        {  }
      }

  }
}
