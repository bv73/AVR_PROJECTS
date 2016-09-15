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


#include "lds184.h"
#include "lds184.c"

unsigned char djn[];

void main( void )
{
// ============ Работа с ЖКИ

    lds184_init();
  while(1)
    {

//       display_graphic(djn); 
    
       test_patten(0xf8,0x00);
       delay_ms(300);
       test_patten(0x07,0xe0);
       delay_ms(300);
       test_patten(0x00,0x1f);
       delay_ms(300);
     }

  
}
