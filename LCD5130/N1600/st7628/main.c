#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (unsigned int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font\font5x7.h"

//#include "N1200\sed15b1.h"
//#include "N1200\sed15b1.c"
#include "N1600\st7628\st7628.h"
#include "N1600\st7628\st7628.c"
//#include "N1600\st7628\lds184.c"


float a, b, c;

void main( void )
{

  a=54.321;
  b=3.14;
  c=a/b;

  
  st7628_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  st7628_clear_console(0x20); // Чистим пробелами

  sprintf(LINE1,"Драйвер ST7628");
  sprintf(LINE2,"LCD 96X65 pixels");
  sprintf(LINE3,"For Nokia 1600");
  sprintf(LINE4,"3-wire SPI mode");
  sprintf(LINE5,"Тест %g/%g=%g",a,b,c);
  sprintf(LINE7,"Research & Code");
  sprintf(LINE8,"Бакум В.И. 2013");

  st7628_update_console();


  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char contrast=0x39;

  while (1)
  {
        if (!KEY1)
      {
        contrast--;
        contrast&=0x7F;

        st7628_cmd(ST7628_WR_CNTR);  // Contrast set
        st7628_data(contrast);

        sprintf(LINE8,"Contrast:%02X",contrast);
        st7628_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        contrast++;
        contrast&=0x7F;

        st7628_cmd(ST7628_WR_CNTR);  // Contrast set
        st7628_data(contrast);

        sprintf(LINE8,"Contrast:%02X",contrast);
        st7628_update_console();

        delay_ms(100);
      }

  }
}
