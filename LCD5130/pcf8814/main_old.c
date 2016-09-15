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
#include "pcf8812.h"
#include "pcf8812.c"

#include "pcf8814.h"
#include "pcf8814.c"


float a, b, c;

void main( void )
{
// ============ Работа с первым ЖКИ
  
  pcf8812_init(); // init включает в себя функцию очистки буфера экрана

  a=12.345;
  b=8.16;
  c=a/b;
  pcf8812_clear_console(0x20); // чистим буфер консоли пробелами

  sprintf(LINE1,"Драйвер PCF8812");
  sprintf(LINE2,"LCD 96X65 pixels");
  sprintf(LINE3,"For Mitsubishi");
  sprintf(LINE4,"Trium Mars LCD.");
  sprintf(LINE5,"Тест %g/%g=%g",a,b,c);
//  sprintf(LINE6,"=%g",c);     // вывод - результат деления двух чисел в буфер консоли
  sprintf(LINE7,"LCD Cost $0.5!");
  sprintf(LINE8,"By (R)soft 2011");
  
  pcf8812_update_console(); // вывод из буфера консоли на экран

  
// ====== Работа со вторым ЖКИ
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
  sprintf(LN7,"LCD cost $4");
  sprintf(LN8,"By (R)soft 2011");

  pcf8814_update_console();

  delay_ms(2000);
  pcf8814_cls();
  delay_ms(100);
  pcf8814_update_console();

  while (1)
  {
    
  }
}
