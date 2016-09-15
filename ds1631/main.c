#include <iom162.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include <stdlib.h> // for abs
#include <pgmspace.h>

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
#include "i2c.h"
#include "i2c.c"
#include "ds1621.h"
#include "ds1621.c"


//#include "onewire.c"

void main( void )
{
//  unsigned char ROM[8];

  
  I2C_init();
  Pusk_DS1621();
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  sed15b1_clear_console(0x20); // Чистим пробелами

  sprintf(LIN1,"Температура");
  sprintf(LIN2,"DS1631");

//  sprintf(LIN5,"Реперная точка: %d градусов",REPER_POINT);

  sprintf(LIN8,"By (R)soft 2011");

  sed15b1_update_console();

  while (1)
  {
    delay_ms(500);
//    Start_DS1621();
    Read_Internal_T();
    Send_Result_DS1621();

/*    
    unsigned char b;
    b=OWFirst(ROM, 1, 0);
    sprintf(LIN5,"Search=%X",b);
    sprintf(LIN6,"Found:%02X %02X%",ROM[0],ROM[1]);
    sprintf(LIN7,"%02X %02X",ROM[2],ROM[3]);
*/

    sed15b1_update_console();
  }
}
