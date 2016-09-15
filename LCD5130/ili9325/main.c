#include <p18cxxx.h> 
#include <delays.h> 
#include <usart.h>
#include "ILI16bit.h"



// Configuration bits
#pragma config  OSC = HS, OSCS = ON, PWRT = ON, WDT = OFF, LVP = ON, STVR = OFF, DEBUG = OFF
#pragma config  BOR = OFF, CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF, CPB = OFF, CPD = OFF
#pragma config  WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF, WRTB = OFF,WRTC = OFF, WRTD = OFF
#pragma config  EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTRB = OFF



typedef unsigned char BYTE; 
  
// Constant declarations 
	#define CLOCK 	20							// clock in MHz


	
void Init (void) 
{
	ADCON1 = 0b00000111;
	TRISA = 0;
	TRISB = 0;			// all outputs
	TRISC = 0;
	TRISD = 0;			// all outputs
	TRISE = 0;			// all outputs
	LATA = 0;
	LATB = 0;
	LATC = 0;
	LATD = 0;
	LATE = 0;
	return;
}

///////// MAIN

void main (void) 
{
	
	Init();
	lcd_init();	  
	Lcd_Clear(BLACK);
	LCD_write_string(0,10,"abcdefghijklmnopqrstuvwxyz",BRIGHTYELLOW,BLACK,1);
	LCD_write_string(15,10,"ABCDEFGHIJKLMNOPQRSTUVWXYZ",ORANGE,BLACK,1);


}
				
				
				
