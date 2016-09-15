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


#include "s1d15e06.h"
#include "s1d15e06.c"


void main( void )
{

  unsigned char i;
  for (i=0;i<255;i++)
  {
    send_cmd(i);
  }
  
  
  //   s1d15e06_init();             // LCD Initialization

   
   while(1)
   {
   }
  
}
