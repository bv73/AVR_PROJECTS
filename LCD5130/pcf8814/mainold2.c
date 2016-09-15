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

#include "pcf8814.h"
#include "pcf8814.c"


float a, b, c;

void main( void )
{
// ============ Работа с первым ЖКИ
  
  a=54.321;
  b=3.14;
  c=a/b;
  pcf8814_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  pcf8814_clear_console(0x20); // Чистим пробелами

  sprintf(LN1,"Драйвер PCF8814");
  sprintf(LN2,"LCD 96X65 pixels");
  sprintf(LN3,"For Nokia 6510");
  sprintf(LN4,"3-wire SPI mode");
  sprintf(LN5,"Тест %g/%g=%g",a,b,c);
  sprintf(LN7,"Research & Code");
  sprintf(LN8,"By (R)soft 2011");

  pcf8814_update_console();

  while (1)
  {
    
  }
}
