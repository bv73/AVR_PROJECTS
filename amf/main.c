// AMF 90XL diagnostic
// Version 1.0
// By (R)soft 1.10.2011

#include <intrinsics.h>
#include <iom162.h>
#include <iomacro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pgmspace.h>
#include <math.h>


#include "amf.h"
//#include "uart.c"
//#include "interrupt.c"


// ==================================================================
//        PROCS
// ==================================================================

void InitTimers (void)
{
  // ========================== TIMER/COUNTER 1 (Capture Mode)
  // (0x01 by Default 0000 0001)
  // TCCR1B = ICNC1, ICES1, -, WGM13, WGM12, CS12, CS11, CS10
  // ICNC - Input Capture Noise Canceler
  // ICES - Input Capture Edge Select 0=Falling 1=Rising
  // WGM - Waveform Generation Mode
  // CS10..12 - Clock Selection 0,1,8,64,256,1024,Ext
  TCCR1B=(1<<CS10); // Prescaler = 1/1

  // ========================== TIMER/COUNTER 2 (Timer Mode)
  // (0x03 by Default 0000 0011)
  // f=8.0 MHz/32/256 = 977 Hz (976,5625)
  // TCCR2 = FOC2=0, WGM20, COM21, COM20, WGM21, CS22, CS21, CS20
  // FOC2 - Force Output Compare (для PWM режма), ставим в ноль
  // WGM20 (PWM2) - Mode Select
  // COM21 - Compare Match Output Mode (Выбор режима управления выходной ногой OC2)
  // COM20 - Compare Match Output Mode, в обычном режиме COM20=COM21=0
  // WGM21 (CTC2) - Mode Select
  // CS22 CS21 CS20 - Clock Select Prescaler
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:32
  //    100 - 1:64
  //    101 - 1:128
  //    110 - 1:256
  //    111 - 1:1024
  // CS21=1 & CS20=1 - Prescaler 32x

  TCCR2=(0<<FOC2)|(0<<WGM20)|(0<<COM21)|(0<<COM20)|(0<<WGM21)|(0<<CS22)|(1<<CS21)|(1<<CS20);
  
  // ========================== TIMER/COUNTER 3 (Capture Mode)
  // (0x01 by Default 0000 0001)
  // TCCR3B = ICNC3, ICES3, -, WGM33, WGM32, CS32, CS31, CS30
  TCCR3B=(1<<CS30); // No Prescaling

  // ========================== TIMERS/COUNTERS INTERRUPT SETUP
  // Timer Counter Interrupt Mask Register
  // (0x8C by Default 1000 1100)
  // TIMSK = TOIE1, OCIE1A, OCIE1B, OCIE2,   TICIE1, TOIE2, TOIE0, OCIE0
  // TOIE1=1 Timer1 Interrupt Enable
  // TICIE=1 Input1 Capture Interrupt Enable
  // TOIE2=1 Timer2 Interrupt Enable
  TIMSK = (1<<TOIE1)|(1<<TICIE1)|(1<<TOIE2);
  
  // ETIMSK (0x24 By Default 0010 0100)
  //  ETIMSK = -, -, TICIE3, OCIE3A,     OCIE3B, TOIE3, -, -
  // TICIE3=1 Input3 Capture Interrupt Enable
  // TOIE3=1  Timer3 Interrupt Enable
  ETIMSK = (1<<TICIE3)|(1<<TOIE3);
}

void InitPorts (void)
{
  //=========================== PORT A Definition
  // PORTA Direction (0xFF by Default)
  DDRA  = 0xFF;
  // PORTA Setup Bits
  PORTA = 0xFF;

  // ========================== PORT B
  // PORTB Direction (0xFF by Default)
  Control_DIR = 0xFF;

  Control = (0<<AA0)|(0<<AA1)|(0<<AB0)|(0<<AB1)|(0<<AB2)|(1<<PB5)|(1<<PB6)|(1<<PB7);
  
  // =========================== PORT C
  // PORTC Direction (0x00 by Default)
  Data_DIR = 0x00; // All For Input

  // PORTC Setup Bits (0x00 by Default - Z state)
  Data = 0x00;

  // ========================== PORT D
  // PORTD Direction (0xFF by Default)
  DDRD = 0xFF;

  // PORTD Setup Bits
  PORTD = 0xFF; // Гасим все

  // ========================== PORT E
  // PORTE Direction (0xFF by Default)
  DDRE = 0xFF;
  
  // PORTE Setup Bits
  PORTE = 0xFF;
}

void OE_Strobe(void)
{
  OE=0;
  delay_ms(100);
  OE=1;
}

void CLK_Strobe(void)
{
  CLK=0; // /CLK=0
  DDRB_Bit6=1; // For Out
  delay_ms(100);
  DDRB_Bit6=0; // For_In (Z-State)
}

void RES_Strobe(void)
{
  RES=0; // /RES=0
  delay_ms(100);
  RES=1;
}

void Addr_DCA (unsigned char address)
{
  unsigned char byte = Control;
  byte|=(address&0x03);
  Control=byte;
}

void Addr_DCB (unsigned char address)
{
  unsigned char byte = Control;
  byte|=(address&0x07)<<2;
  Control=byte;
}

void Output_Data (unsigned char data)
{
  LED_IN=1;
  LED_OUT=0; // Сигналим что данные на вывод
  DDRC=0xFF; // Порт на вывод
  PORTC=data;
}

unsigned char Input_Data (void)
{
  LED_OUT=1; // Гасим
  LED_IN=0; // Сигналим что данные вводим с шины
  Data_DIR=0x00; // Порт на ввод
  Data=0xFF; // Подтяжка резисторами
  delay_ms(10);
  return Data_PIN;
}


// ===================================================================
//
//                   M A I N
//              
// ===================================================================


void main(void)
{
DDRA=0xFF;
PORTA=0xFF;

DDRB=0x00;
PORTB=0xFF;

DDRC=0xFF;
PORTC=0xFF; // Control

DDRD=0xFF;
PORTD=0xFF; // Data
delay_ms(100);
PORTD=0x00;
delay_ms(100);
PORTD=0xFF;
  
  for(;;)
   {

     if ((PINB&0x01)==0)
     { 
       PORTD=~PORTD;
       delay_ms(300);
     }
     
   }
}