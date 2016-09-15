// LCD KS0108 by (R)soft 17.5.2011
// LCD on two KS0108 driver chips
// Version 1.1
// Last edition 19.5.2011
// -----------------------
// Mega128 KIT Connectors:
// (X8) PORTD - Control Bus
// D/I#       -> PD0 Data(1)/Instructions(0) Select Bit
// R/W#       -> PD1 Read(1)/Write(0) Bit
// EN         -> PD2 Enable (1) Bit
// CS1        -> PD3 Chip Select First Chip (1)
// CS2        -> PD4 Chip Select Second Chip (1)
// RST#       -> PD5 Reset Chip (0)
// LED1       -> PD6 Debug LED1 Onboard
// LED2       -> PD7 Debug LED2 Onboard
// ----------------------
// (X7) PORTC - Data Bus
// D[0..7] -> P[C0..7]
// ---------------------

#include "iom128.h"
#include "inavr.h"
#include "pgmspace.h"
#include "stdio.h"

#define delay_us(V) __delay_cycles(V*8L)

#include "KS0108.c"
#include "KS0108-AVR.c"

#define FALSE (0)
#define TRUE  (!FALSE)

#define LED1    PORTD_Bit6
#define LED2    PORTD_Bit7

#include "LCD5130.c"
#include "uart.c"

// =============================================================================================

void InitPorts (void)
{
// For DDRx: FF - output, 00 - input
  DDRA=0xFF; // All Output
  DDRB=0x00;
  DDRC=0xFF;
  DDRD=0xFF;
  DDRE=0x0B; // Keys Line for Input And Busy Input (PE2), RXD0&TXD0 output
  DDRF=0xFF;
// For PORTx: FF - pullup, 00 - input
  PORTB=0x00;
  PORTC=0x00;
  PORTD=0x00; // Set Port D
  PORTE=0xFF; // For Keys & Busy
}

// Задержка в милисекундах
void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

/* ************************************************************** */
// Опрос кнопок.
// ckey = 1, 2, 4, 8 или 0 - не нажата
char Inkey (void)
{
  char  ckey=(~PINE&0xF0);
  ckey>>=4;
  return (ckey);
}

// ======================================================================================================

int main( void )
{
  int CounterLED=0;
  char CountSW=0;
  char ModeSW1=0;
  char ModeSW2=0xFF; // Pause or Meandr Switcher
  char ModeSW3=0;
  char ModeSW4=0;

  InitPorts();
  InitUART();
  _pch (0x31); // Вывод байта в последовательный порт
  nokia_init();
  ClrScr(); // Чистим буфер
  nokia_cls();
  nokia_gotoxy(0,0);
  sprintf (CON,   "KS0108 LCD "); // Вывод приветствия и версии программы
  sprintf (CON+14,"Тест программы"); 
  sprintf (CON+28,"v1.0 17/5/2011");
  sprintf (CON+42,"Кнопки");
  sprintf (CON+56,"[1][2][3][4]");
  sprintf (CON+70,"Выбор режима");
  UpdateConsole();
  ClrScr(); // Чистим буфер LCD после вывода приветствия

  GLCD_Initialize();
  delay_ms(100);
  GLCD_ClearScreen();

  GLCD_GoTo(0,0);
  GLCD_WriteString("+-------------------+");
  GLCD_GoTo(0,1);
  GLCD_WriteString("|     Universal     |");
  GLCD_GoTo(0,2);
  GLCD_WriteString("|   KS0108 library  |");
  GLCD_GoTo(0,3);
  GLCD_WriteString("|                   |");
  GLCD_GoTo(0,4);
  GLCD_WriteString("|  en.radzio.dxp.pl |");
  GLCD_GoTo(0,5);
  GLCD_WriteString("|  STM32 Cortex-M3  |");
  GLCD_GoTo(0,6);
  GLCD_WriteString("|  microcontrollers |");
  GLCD_GoTo(0,7);
  GLCD_WriteString("+-------------------+");
 
  
  while(1) 
  {

  char KEY=Inkey(); // KEY = 1, 2, 4, 8 или 0 - не нажата
  if (KEY==1) // Переключение и задание режимов в зависимотси от нажатой клавиши
    { 
      ModeSW1=~ModeSW1;
      if (ModeSW1==0xFF) { sprintf(CON+70,"Режим CLOCK=0 "); }
      else               { sprintf(CON+70,"Режим CLOCK=1 "); }
      UpdateConsole(); 
      delay_ms(300); 
    }
  if (KEY==2) 
    { 
      ModeSW2=~ModeSW2; // Инверсия режима
      if (ModeSW2==0xFF) { sprintf(CON+70,"Режим LATCH=0 "); }
      else               { sprintf(CON+70,"Режим LATCH=1 "); }
      UpdateConsole(); 
      delay_ms(300);
    }
  if (KEY==4) 
    { 
      ModeSW3=~ModeSW3;
      if (ModeSW3==0xFF) { sprintf(CON+70,"Режим Disp OFF"); }
      else               { sprintf(CON+70,"Режим Disp ON "); }
      UpdateConsole();
      delay_ms(300);
    }
  if (KEY==8) 
    { 
      ModeSW4=~ModeSW4;
      if (ModeSW4==0xFF) { sprintf(CON+70,"Режим FRAME=0 "); }
      else               { sprintf(CON+70,"Режим FRAME=1 "); }
      UpdateConsole();
      delay_ms(300);
    }  
  
  CounterLED++;
  if (CounterLED==15550) { CountSW=~CountSW; CounterLED=0; }
  if (CountSW==0) { LED1=0; LED2=1; }  else { LED1=1; LED2=0; } // Блымает светодиодами

   }
}
