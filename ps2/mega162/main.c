#include <intrinsics.h> // for delay procs
#include <pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <iom162.h>

#define delay_us(V) __delay_cycles(V*8L)

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#define LO8(var) ((unsigned char)((var)&0xFF))
#define HI8(var) ((unsigned char)((var>>8)&0xFF))

#define LED_PORT  PORTD
#define KBD_PORT  PORTD
#define KBD_PIN   PIND
#define KBD_DDR   DDRD
#define RXD       PD0 // USART0 Receive Data (Input)
#define TXD       PD1 // USART0 Transmit Data (Output)
#define KBD_CLK   PD2 // Keyboard Clock Line (INT0 connected)
#define KBD_DATA  PD3 // Keyboard Data Line
#define LED_TX    PD4
#define LED_RX    PD5
#define LED_KBD   PD6
#define LED_4     PD7

#include "keyboard.c"
#include "uart.c"
#include "interrupt.c"

void Init_Ports (void)
{
  // ========================== Keyboard PORT
  // PORT Direction
  KBD_DDR = (1<<LED_4)|(1<<LED_KBD)|(1<<LED_RX)|(1<<LED_TX)|(0<<KBD_DATA)|(0<<KBD_CLK)|(1<<TXD)|(0<<RXD);
  //            KBD_DATA-   KBD_CLK-     TXD-Out  RXD-In

  // PORT Setup Bits
  KBD_PORT = (1<<LED_4)|(1<<LED_KBD)|(1<<LED_RX)|(1<<LED_TX)|(1<<KBD_DATA)|(1<<KBD_CLK)|(1<<TXD)|(1<<RXD);
  
}


void main(void)
{
  InitTimer2();
  Init_Ports();
  InitUART();       // »нициализаци€ UART
  init_keyboard();  // »нициализаци€ приемника клавиатуры

  __enable_interrupt();
    
  unsigned char key;
  unsigned char ack_ok;
  
  //  sprintf (UART_Buf,"Hello");
  UART_Buf[0]=':';
//  UART_Buf[1]=0x42;
  Transmit(1);

//  delay_ms(500); // ѕосле начального старта подождем ответа от клавы - 0xAA

  unsigned char i;

  for (i=0;i<7;i++)
  {
    ack_ok=kbd_command(0xED);
//    delay_ms(50); // ѕосле посылки каждой команды ждем ~ 2 мс пока клава не ответит в прерывани€х
    UART_Buf[0]=ack_ok;
    Transmit(1); // Print Ack Byte 1 - 00

    ack_ok=kbd_command(i); // Bit2:CAPS Lock; Bit1:Num Lock; Bit0:Scroll Lock.
//    delay_ms(50);
    UART_Buf[0]=ack_ok;
    Transmit(1); // Print Ack Byte 2
  }

  ack_ok=kbd_command(0xEE); // Echo Command
//  delay_ms(5);
    
  while(1)
    {
        if (buffcnt != 0)
          {
            key=getchar_kbd();
            UART_Buf[0]=key;
            Transmit(1);
          }

//        delay_ms(10);
    }
}

