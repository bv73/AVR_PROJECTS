#include <iom162.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

#define delay_us(V) __delay_cycles(V*8L)

#include "font5x7.h"
#include "pcf8812.h"
#include "pcf8812.c"

#include "sed15b1.h"
#include "sed15b1.c"


float a, b, c;

void main( void )
{
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
  sprintf(LINE7,"Research & Code");
  sprintf(LINE8,"By (R)soft 2011");
  
  pcf8812_update_console(); // вывод из буфера консоли на экран

// ====== Работа со вторым ЖКИ
  a=54.321;
  b=3.14;
  c=a/b;
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  sed15b1_clear_console(0x20); // Чистим пробелами

  sprintf(LIN1,"Драйвер SED15B1");
  sprintf(LIN2,"LCD 96X65 pixels");
  sprintf(LIN3,"For ALCATEL OT");
  sprintf(LIN4,"311/511/525 LCD");
  sprintf(LIN5,"Тест %g/%g=%g",a,b,c);
  sprintf(LIN7,"Research & Code");
  sprintf(LIN8,"By (R)soft 2011");

  sed15b1_update_console();
  
  while (1)
  {
    
  }
}
