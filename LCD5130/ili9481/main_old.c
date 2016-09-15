#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h>      // for sprintf
#include <pgmspace.h>   // for memcpy_P

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}


#include "ili9481\ili9481.h"
#include "ili9481\ili9481.c"


void main( void )
{

  ili9481_init();
  ili9481_rainbow16();
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char reg=0x00;

  
  while (1)
  {

    if (!KEY1)
      {
        reg--;

        delay_ms(100);
      }

    if (!KEY2)
      {
        reg++;

        delay_ms(100);
      }

  }
}
