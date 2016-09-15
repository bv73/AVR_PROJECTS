/*************************************************************************************
*  Moodlamp by (R)soft & BSVi
*  version: 1.00
*************************************************************************************/

#include <stdlib.h>
#include "global.h"

#define delay_us(V) __delay_cycles(V*20L)

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

//------------------------------------------------------------------------------------
//  Глобальные переменные
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//  Локальные функции
//------------------------------------------------------------------------------------
static void init_ports();
static void set_leds( uint8 r, uint8 g, uint8 b );
static void start_pwm();
static void hsv_to_rgb( int16 *r, int16 *g, int16 *b, uint16 i_hue, uint16 sat, uint16 val );

__flash static const uint8 brightness_table[256] =
{
   0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03,
   0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07,
   0x07, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B,
   0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x10, 0x10,
   0x11, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16,
   0x16, 0x17, 0x17, 0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1A, 0x1B, 0x1B, 0x1C, 0x1C,
   0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24,
   0x25, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0x29, 0x2A, 0x2B, 0x2B, 0x2C, 0x2D,
   0x2D, 0x2E, 0x2F, 0x2F, 0x30, 0x31, 0x31, 0x32, 0x33, 0x34, 0x34, 0x35, 0x36, 0x37,
   0x37, 0x38, 0x39, 0x3A, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x3F, 0x40, 0x41, 0x42,
   0x43, 0x44, 0x45, 0x46, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
   0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E,
   0x5F, 0x61, 0x62, 0x63, 0x64, 0x65, 0x67, 0x68, 0x69, 0x6A, 0x6C, 0x6D, 0x6E, 0x70,
   0x71, 0x72, 0x74, 0x75, 0x77, 0x78, 0x79, 0x7B, 0x7C, 0x7E, 0x7F, 0x81, 0x82, 0x84,
   0x85, 0x87, 0x88, 0x8A, 0x8C, 0x8D, 0x8F, 0x90, 0x92, 0x94, 0x96, 0x97, 0x99, 0x9B,
   0x9D, 0x9E, 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xA9, 0xAB, 0xAD, 0xAF, 0xB1, 0xB3, 0xB5,
   0xB7, 0xB9, 0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCB, 0xCD, 0xCF, 0xD1, 0xD4,
   0xD6, 0xD8, 0xDB, 0xDD, 0xE0, 0xE2, 0xE5, 0xE7, 0xEA, 0xEC, 0xEF, 0xF2, 0xF4, 0xF7,
   0xFA, 0xFC, 0xFF, 0xFF
};



#define MAX_SPEED     10 //3
#define CHANGE_DELAY  10 //30


//------------------------------------------------------------------------------------
void main()
{

   init_ports();
   start_pwm();

   set_leds(255,0,0);

   srand(0x1234);

   int16 r = 0, g = 0x88, b = 0xE7;
   int16 targetr, targetg, targetb;
   int16 speedr, speedg, speedb;
   bool reached_r, reached_g, reached_b;
   uint16 target_hue;

   while(1)
   {

      reached_r = false;
      reached_g = false;
      reached_b = false;

      target_hue = ABS( rand() % 360 );
      hsv_to_rgb( &targetr, &targetg, &targetb, target_hue, 0xff, 0xff);

/*
      speedr = ABS( rand()%MAX_SPEED ) + 1;
      speedg = ABS( rand()%MAX_SPEED ) + 1;
      speedb = ABS( rand()%MAX_SPEED ) + 1;
*/
      speedr = speedg = speedb = 1;
      if ( r > targetr  ) speedr = -speedr;
      if ( g > targetg  ) speedg = -speedg;
      if ( b > targetb  ) speedb = -speedb;

      while(1)
      {
         delay_ms(CHANGE_DELAY);
         if ( !reached_r )
         {
            r += speedr;
            if ( ( speedr > 0 ) && ( r >= targetr ) ) { r = targetr; reached_r = true; }
            if ( ( speedr < 0 ) && ( r <= targetr ) ) { r = targetr; reached_r = true; }
         }
         set_leds(r,g,b);

         delay_ms(CHANGE_DELAY);
         if ( !reached_g )
         {
            g += speedg;
            if ( ( speedg > 0 ) && ( g >= targetg ) ) { g = targetg; reached_g = true; }
            if ( ( speedg < 0 ) && ( g <= targetg ) ) { g = targetg; reached_g = true; }
         }
         set_leds(r,g,b);

         delay_ms(CHANGE_DELAY);
         if ( !reached_b )
         {
            b += speedb;
            if ( ( speedb > 0 ) && ( b >= targetb ) ) { b = targetb; reached_b = true; }
            if ( ( speedb < 0 ) && ( b <= targetb ) ) { b = targetb; reached_b = true; }
         }
         set_leds(r,g,b);


         if ( reached_r && reached_g && reached_b ) break;

      }
   }
}

//------------------------------------------------------------------------------------
//  Преобразование HSL->RGB
//------------------------------------------------------------------------------------
static void hsv_to_rgb( int16 *r, int16 *g, int16 *b, uint16 i_hue, uint16 sat, uint16 val )
{
   int ii;
   double fr, hue;
   unsigned char c1, c2, c3;

   if (!sat)
   {
      *r = *g = *b = val;
   }
   else
   {
      while (i_hue >= 360) i_hue -= 360;

      hue = i_hue;
      ii = (int)(hue /= 60.0);
      fr = hue - ii;
      c1 = (val * (255 - sat)) / 255;
      c2 = (unsigned char)(val * (255 - sat * fr)) / 255;
      c3 = (unsigned char)(val * (255 - sat * (1.0 - fr))) / 255;

      switch (ii)
      {
        case 0: *r = val; *g = c3; *b = c1; break;
        case 1: *r = c2;  *g = val;*b = c1; break;
        case 2: *r = c1;  *g = val;*b = c3; break;
        case 3: *r = c1;  *g = c2; *b = val; break;
        case 4: *r = c3;  *g = c1; *b = val; break;
        case 5: *r = val; *g = c1; *b = c2; break;
      }
   }
}
//------------------------------------------------------------------------------------
//  Начальная инициализация ножек
//------------------------------------------------------------------------------------
static void init_ports()
{
   // PORTB
   PORTB = ENABLE1_PB + ENABLE2_PB;
   DDRB = 0xff;

   // PORTC
   PORTC = 0;
   DDRC = 0xff - GAIN10_PC - GAIN2_PC - GAIN100_PC - MIC_PC;

   // PORTD
   PORTD = ENABLE3_PD;
   DDRD = 0xff;
}



//------------------------------------------------------------------------------------
//  Устанавливает цвет светодиодов
//------------------------------------------------------------------------------------
static void set_leds( uint8 r, uint8 g, uint8 b )
{
   OCR1A = 0xff-brightness_table[r];
   OCR2B = 0xff-brightness_table[g];
   OCR1B = 0xff-brightness_table[b];
}

//--------------------------------------------------------------
//  Запускаем ШИМы
// Период ШИМа 0,8 мс -> 1250 Гц
// 20 MHz / 256 / 64 = 1220 Hz
// 20 MHz / 256 = 78125 Hz

static void start_pwm()
{
  // TCCR2A = COM2A1,COM2A0,COM2B1,COM2B0,-,-,WGM21,WGM20
  // COM2A1,COM2A0 - Compare Match Output A Mode
  // COM2B1,COM2B0 - Compare Match Output B Mode
  // 10 - Clear OC2B on Compare Match, Set OC2B at BOTTOM, (non inverting mode)
  // WGM22, WGM21,WGM20 - Waveform Generation Mode
  // 000 - Normal
  // 001 - PWM, Phase Correct
  // 010 - CTC
  // 011 - Fast PWM
  // 100 - Reserved
  // 101 - PWM, Phase Correct
  // 110 - Reserved
  // 111 - Fast PWM
  TCCR2A = (1<<COM2B1)|(1<<WGM21)|(1<<WGM20); // Fast PWM

  // TCCR2B = FOC2A,FOC2B,-,-,WGM22,CS22,CS21,CS20
  // FOC2A,FOC2B - Force Output Compare
  // WGM22 - Mode Select
  // CS22 CS21 CS20 - Clock Select Prescaler
  // 000 - T/C Stopped
  // 001 - No prescaller
  // 010 - 1/8
  // 011 - 1/32
  // 100 - 1/64
  // 101 - 1/128
  // 110 - 1/256
  // 111 - 1/1024
  TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20); // No prescaller
  OCR2B = 0;

  // TCCR1A = COM1A1,COM1A0,COM1B1,COM1B0,-,-,WGM11,WGM10
  // WGM13,WGM12,WGM11,WGM10 - Waveform Generation Mode 
  // 0000 - Normal
  // 0001 - PWM, Phase Correct, 8-bit
  // 0010 - PWM, Phase Correct, 9-bit
  // 0011 - PWM, Phase Correct, 10-bit
  // 0100 - CTC
  // 0101 - Fast PWM, 8-bit
  // 0110 - Fast PWM, 9-bit
  // 0111 - Fast PWM, 10-bit
  // 1000 - PWM, Phase & Frequency Correct
  // 1001 - PWM, Phase & Frequency Correct 
  // 1010 - PWM, Phase Correct
  // 1011 - PWM, Phase Correct  
  // 1100 - CTC
  // 1101 - Reserved
  // 1110 - Fast PWM
  // 1111 - Fast PWM
  // set 0101
  TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM10);

  //TCCR1B = ICNC1,ICES1,-,WGM13,WGM12,CS12,CS11,CS10
  // ICNC1 - Input Capture Noise Canceler
  // ICES1 - Input Capture Edge Select
  // CS12,CS11,CS10 - Clock Select
  // 000 - TC Stopped
  // 001 - No prescaling
  // 010 - 1/8
  // 011 - 1/64
  // 100 - 1/256
  // 101 - 1/1024
  // 110 - External Clock Source T1 Falling Edge
  // 111 - External Clock Source T1 Rising Edge

  TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
  OCR1A = 0;
  OCR1B = 0;
}