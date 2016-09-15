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

#include "N1200\sed15b1.h"
#include "N1200\sed15b1.c"


float a, b, c;

void main( void )
{

  a=54.321;
  b=3.14;
  c=a/b;
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  sed15b1_clear_console(0x20); // Чистим пробелами

  sprintf(LIN1,"Драйвер SED15B1");
  sprintf(LIN2,"LCD 96X65 pixels");
  sprintf(LIN3,"For Nokia 1200");
  sprintf(LIN4,"3-wire SPI mode");
  sprintf(LIN5,"Тест %g/%g=%g",a,b,c);
  sprintf(LIN7,"Research & Code");
  sprintf(LIN8,"Бакум В.И. 2013");

  sed15b1_update_console();

  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char contrast=0x0F;

  while (1)
  {
        if (!KEY1)
      {
        contrast--;
        contrast&=0x1F;

        sed15b1_cmd(SED15B1_RESISTOR_RATIO | 0x04); // V0 Voltage Settings 0...6 (грубо)
        sed15b1_cmd(SED15B1_ELECTRONIC_VOLUME | contrast); // 0...1F (точно)

        sprintf(LIN8,"Contrast:%02X",contrast);
        sed15b1_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        contrast++;
        contrast&=0x1F;

        sed15b1_cmd(SED15B1_RESISTOR_RATIO | 0x04); // V0 Voltage Settings 0...6 (грубо)
        sed15b1_cmd(SED15B1_ELECTRONIC_VOLUME | contrast); // 0...1F (точно)
          
        sprintf(LIN8,"Contrast:%02X",contrast);
        sed15b1_update_console();

        delay_ms(100);
      }

  }
}
