#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

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

#include "font\font5x7.h"
#include "font\arial16.h"
#include "font\arial32.h"

#include "N1200\sed15b1.h"
#include "N1200\sed15b1.c"



void main( void )
{

  
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана


  sed15b1_clear_console(0x20); // Чистим пробелами
  sed15b1_putchar32(0x30,0,0); // char,x,y
  sed15b1_putchar16(0x31,0,4);
  sed15b1_putchar16(0x32,16,4);  
  sed15b1_putchar32(0x33,30,4);    
  sed15b1_putchar32(0x34,50,2);
  sed15b1_putchar32(0x35,32,0);
  sed15b1_putchar16(0x36,54,0);
  sed15b1_putchar16(0x37,22,1);
  sed15b1_putchar32(0x38,72,1);
  sed15b1_putchar16('9',10,6);
  sed15b1_putchar32('!',52,5);  
  sed15b1_putchar32('+',70,5);  
  
  
  //  unsigned char id1,id2,id3,id4;
//  sprintf(LIN5,"Hello!");
//  sprintf(LIN5,"ID1=%02X",id1);
//  sprintf(LIN2,"ID2=%02X",id2);
//  sprintf(LIN3,"ID3=%02X",id3);
//  sprintf(LIN4,"ID4=%02X",id4);

//  sed15b1_update_console();
  
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char reg=0x00;

  while (1)
  {
        if (!KEY1)
      {
        reg--;


        sprintf(LIN8,"Reg:%02X",reg);
        sed15b1_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        reg++;



        sprintf(LIN8,"Reg:%02X",reg);
        sed15b1_update_console();

        delay_ms(100);
      }

  }
}
