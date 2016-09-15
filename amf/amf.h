// By (R)soft 1.10.2011

#define TRUE  0xFF
#define FALSE 0x00

#define delay_us(V) __delay_cycles(V*8L)

void delay_ms (int c)
{
  do
  {
    delay_us (1000); // 1 ms
  }
  while (--c);
}

//==========================================================================
// Port Definitions

// ==== PORT A ====
// Not connect

// ==== PORT B ====
// Address & OE Port
#define Control    PORTB
#define Control_DIR     DDRB

#define AA0   PB0   // PB0  Addr0 of DC A
#define AA1   PB1   // PB1  Addr1 of DC A
#define AB0   PB2   // PB2  Addr0 of DC B
#define AB1   PB3   // PB3  Addr1 of DC B
#define AB2   PB4   // PB4  Addr2 of DC B
#define OE    PORTB_Bit5   // PB5  /OE of DC B
#define CLK   PORTB_Bit6   // PB6  /CLK of U320 & U321
#define RES   PORTB_Bit7   // PB7  /RESET of U323,324,325,326

// ==== PORT C ====
// Data Port
#define Data      PORTC
#define Data_DIR  DDRC
#define Data_PIN  PINC

// ==== PORT D ====
// PD0  nc
// PD1  nc
// PD2  nc
// PD3  nc
#define LED_OUT PORTD_Bit4   // Светодиод режима вывода на шину
#define LED_IN  PORTD_Bit5   // Светодиод режима ввода с шины
#define LED_RX  PORTD_Bit6   // Светодиод приема по UART
#define LED_TX  PORTD_Bit7   // Светодиод передачи по UART

// ==== PORT E ====
// Not Connect

//========================================================================
//========================================================================
//========================================================================


#define SEK_CONST       (977)     // Значение счетчика для секундного таймера

// глобальные переменные

unsigned int Register_OneSec; // Регистр секундного счетчика


