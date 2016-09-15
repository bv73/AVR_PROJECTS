#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font\font5x7.h"
#include "font\arial16.h"

#include "N1200\sed15b1.h"
#include "N1200\sed15b1.c"

#include "Nokia_N93\N93.h"
#include "Nokia_N93\N93.c"

void print_command (unsigned char command)
{
  unsigned char id1,id2,id3,id4;
  N93_write_command(command);
  id1=N93_read_data();
  id2=N93_read_data();
  id3=N93_read_data();
  id4=N93_read_data();
  sprintf(LIN1,"ID1=%02X",id1);
  sprintf(LIN2,"ID2=%02X",id2);
  sprintf(LIN3,"ID3=%02X",id3);
  sprintf(LIN4,"ID4=%02X",id4);
  sed15b1_putchar16(0x30);
  sed15b1_update_console();
}

void main( void )
{

  
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  N93_init();

  sed15b1_clear_console(0x20); // Чистим пробелами

  print_command(N93_READ_DISPLAY_ID);
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char reg=0x00;

  while (1)
  {
        if (!KEY1)
      {
        reg--;

        print_command(reg);  
        sprintf(LIN8,"Reg:%02X",reg);
        sed15b1_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        reg++;


        print_command(reg);  
        sprintf(LIN8,"Reg:%02X",reg);
        sed15b1_update_console();

        delay_ms(100);
      }

  }
}
