#include <iom162.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

#define delay_us(V) __delay_cycles(V*8L)

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font5x7.h"

#include "sed15b1.h"
#include "sed15b1.c"


float a, b, c;

void main( void )
{
// ============ Работа с первым ЖКИ
  
  a=54.321;
  b=3.14;
  c=a/b;
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  sed15b1_clear_console(0x20); // Чистим пробелами

  sprintf(LIN1,"Драйвер SED15B1");
  sprintf(LIN2,"LCD 96X65 pixels");
  sprintf(LIN3,"For Nokia 6510");
  sprintf(LIN4,"3-wire SPI mode");
  sprintf(LIN5,"Тест %g/%g=%g",a,b,c);
  sprintf(LIN7,"Research & Code");
  sprintf(LIN8,"By (R)soft 2011");

  sed15b1_update_console();

  while (1)
  {
    
  }
}
