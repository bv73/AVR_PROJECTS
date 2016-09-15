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

#include "font.h"
#include "s1d15g14.h"
#include "s1d15g14.c"


void main( void )
{
// ============ Работа с ЖКИ
  

int i;

   nokiacolorinit();             // LCD Initialization


   bgcolor=colors[white];
   sprintf(CON15G14,"DEMO DE 3510iLCD");
   
   for(i=0; i<6; i++)            // Shows all the colors less white
     {
       fgcolor=colors[i];           // change the color of letters
       nokia_outs(CON15G14,1,2+(i*8));          // next line ( each line=8 bits )
     }

   sprintf(CON15G14,"                ");
   
   while(1)
   {
      for(i=0; i<7; i++)            // Shows all the colors in 2 strip
        {
          bgcolor=colors[i];           // change the background color
          nokia_outs(CON15G14,1,2+(6*8));          // next line ( each line=8 bits )
          nokia_outs(CON15G14,1,2+(7*8));          // next line ( each line=8 bits )
          delay_ms(300);
        }
   }
  
}
