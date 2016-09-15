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
#include "hd66753.h"
#include "hd66753.c"


float a, b, c;

void main( void )
{
// ============ Работа с первым ЖКИ
  
  hd66753_init(); // init включает в себя функцию очистки буфера экрана

//  unsigned int wrd = 0xFFF3;
//  DDRDATA=~((unsigned char)((wrd)&0xFF)); // Выделяем младший байт и инвертируем


  a=12.345;
  b=8.16;
  c=a/b;
  hd66753_clear_console(0x20); // чистим буфер консоли пробелами

  sprintf(LINEN1,"Драйвер hd66752");
  sprintf(LINEN2,"LCD 150x100 pixs");
  sprintf(LINEN3,"For Alcatel");
  sprintf(LINEN4,"715 LCD.");
  sprintf(LINEN5,"Тест %g/%g=%g",a,b,c);
  sprintf(LINEN7,"The Best LCD!");
  sprintf(LINEN8,"By (R)soft 2011");
  
//  hd66753_update_console(); // вывод из буфера консоли на экран



  while (1)
  {
    
  }
}
