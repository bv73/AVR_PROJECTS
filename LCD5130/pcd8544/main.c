#include <iom162.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

#include "font5x7.h"
#include "pcd8544.h"
#include "pcd8544.c"


float a, b, c;

void main( void )
{
  pcd8544_init(); // init включает функцию очистку буфера экрана

  a=12.345;
  b=8.16;
  c=a/b;
  clear_console(0x20); // чистим буфер консоли пробелами
  sprintf(LINE1,"Oldest BIG LCD");
  sprintf(LINE2,"PCF8544 Driver");
  sprintf(LINE3,"For Nokia5130");
  sprintf(LINE4,"%f/%f",a,b);
  sprintf(LINE5,"=%f",c);
  sprintf(LINE6,"By (R)soft2010"); // вывод результат деления двух чисел в буфер консоли
  update_console(); // вывод из буфера консоли на экран
  while (1)
  {
    
  }
}
