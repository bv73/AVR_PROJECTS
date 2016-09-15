// APIS Panel I/O main header

//---------------------------------------------------------
/*
#define delay_us(V) __delay_cycles(V*16L) // кварц 16 МГц

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}
*/

//-----------------------------------
//
#define COM1_OUT  PORTD_Bit0
#define COM2_OUT  PORTD_Bit1
#define COM3_OUT  PORTD_Bit2
#define COM4_OUT  PORTD_Bit3
#define COM0_OUT  PORTD_Bit4
#define RIS       PORTD_Bit5  //COM2 COM3 COM4 COM0 (инвертируются транзистором)
#define SW6       PIND_Bit6
#define SW7       PIND_Bit7
//----------------------
#define SCL   PORTC_Bit0
#define SDA   PORTC_Bit1
#define SPIN  PORTC_Bit2  // Все COM
#define SEGA  PORTC_Bit3  // Buzzer LED COM0 (Orange)
#define SEGB  PORTC_Bit4  // Clock LED COM0
#define SEGC  PORTC_Bit5  // Ключик LED COM0 (Красный)
#define SEGD  PORTC_Bit6  // Термометр LED COM0 (Красный)
#define SEGE  PORTC_Bit7  // Двоеточие COM0
//------------------------
#define SW2   PINA_Bit0
#define SW9   PINA_Bit1
#define SW8   PINA_Bit2
#define POT   PORTA_Bit3
#define MODES PORTA_Bit4  // Все COM
#define LEDS  PORTA_Bit5  // RED LEDS - COM1 COM2 COM3
#define SEGG  PORTA_Bit6  // Градус (COM0)
#define SEGF  PORTA_Bit7  // Замок LED COM0 (Orange)
//--------------------------
#define SW4   PINB_Bit0
#define SW5   PINB_Bit1
#define SW1   PINB_Bit2
#define SW3   PINB_Bit4

//============================
#define SEK_CONST     (122)     // Значение счетчика для секундного таймера
#define HALF_CONST    (61)  // Примерно 1/2 секунды
#define QUADRO_CONST  (31)  // Примерно 1/4 секунды

// глобальные переменные
unsigned char Reg_Secunda; // Регистр секундного счетчика
unsigned char FlagSecunda;   // Секундный флаг. Если не ноль, то прошла секунда
unsigned char Reg_Half;
unsigned char FlagHalf;
unsigned char Reg_Quadro;
unsigned char FlagQuadro;
unsigned char DynamicStage;

typedef union
{
  char byte;
  struct
  {
    char BUT1:1, BUT2:1, BUT3:1, BUT4:1, BUT5:1, BUT6:1, BUT7:1, BUT8:1;
  };
} U_BUTTONS;

//volatile U_SEGS s;

typedef union
{
  char byte;
  struct
  {
    char BUZZER:1, TIMER:1, KEY:1, TERMO:1, POINTS:1, LOCK:1, GRADUS:1, none:1;
  };
} U_LEDS;

//volatile U_LEDS l;

//-------------------------------
// Reg_COM0:
// Bit0 - BUZZER
// Bit1 - TIMER (CLOCK)
// Bit2 - RED KEY
// Bit3 - RED TERMOMETR
// Bit4 - :
// Bit5 - LOCK
// Bit6 - Gradus
U_LEDS COM0;

//--------- Slave memory I/O registers (4+3+4+9+3 = 23)
// Область памяти, которая отображается на дисплее
// 20 байт
typedef union  //  this is the same data structure used by the client
{
  unsigned char Buf[20];
  struct
  {
    unsigned char Seg1; // Bit0 - SEGA ... Bit6 - SEGG
    unsigned char Seg2;
    unsigned char Seg3;
    unsigned char Seg4;

    unsigned char Red1;  // RED LED 1: zero - off, 0xFF - on
    unsigned char Red2;  // RED LED 2
    unsigned char Red3;  // RED LED 3

    unsigned char Ris1;  // zero - off, 0xFF - on
    unsigned char Ris2;
    unsigned char Ris3;
    unsigned char Ris4;

    unsigned char Modes;  // 0, 0xFF
    unsigned char Spin;   // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Termo;  // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Timer;  // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Lock;   // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Key;    // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Points; // 0, 0x80, 0xC0, 0xE0, 0xFF
    unsigned char Buzzer; // 0, 0xFF
    unsigned char Gradus; // 0, 0xFF
   };
} regspackage;

regspackage Reg;

// Область памяти, которая читается слэйвом для опроса кнопок и селектора выбора программ
// 3 байта
unsigned char Reg_Prog;   // Регистр номера выбранной программы в соотв. с АЦП
U_BUTTONS Reg_But1;       // Buttons Register 1
unsigned char Reg_But2;   // Buttons Register 2
//-------------------------------------------------
unsigned int  Reg_ADC;


//----------------------------
//

#define DIG_0 0xC0
#define DIG_1 0xF9
#define DIG_2 0xA4
#define DIG_3 0xB0
#define DIG_4 0x99
#define DIG_5 0x92
#define DIG_6 0x82
#define DIG_7 0xF8
#define DIG_8 0x80
#define DIG_9 0x90
#define DIG_A 0x88
#define DIG_B 0x83
#define DIG_C 0xC6
#define DIG_D 0xA1
#define DIG_E 0x86
#define DIG_F 0x8E
#define DIG_P 0x8C
#define DIG_I 0xCF
#define DIG_S 0x92
#define DIG_U 0xC1
#define DIG_L 0xC7
#define DIG_H 0x89
#define DIG_NONE  0xFF
#define DIG_n 0xAB
#define DIG_o 0xA3
#define DIG_r 0xAF

static __flash char HEX[16]=
{
  DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9,
  DIG_A, DIG_B, DIG_C, DIG_D, DIG_E, DIG_F
};



//-------------------------------------
//==
void SetSegments (unsigned char reg)
{
  if (reg&0x01) SEGA=1; else SEGA=0;
  if (reg&0x02) SEGB=1; else SEGB=0;
  if (reg&0x04) SEGC=1; else SEGC=0;  
  if (reg&0x08) SEGD=1; else SEGD=0;  
  if (reg&0x10) SEGE=1; else SEGE=0;  
  if (reg&0x20) SEGF=1; else SEGF=0;
  if (reg&0x40) SEGG=1; else SEGG=0;
}

//---------------------------------------------
// В зависимости от значения АЦП поворотного переключателя
// в регистр номера программы заносится значение выбранной программы.
void ProgramSelect (unsigned int valueADC)
{
  if (valueADC==0) { Reg_Prog = 9; return; }
  if ( (valueADC>0x3F0)&&(valueADC<=0x3FF) ) { Reg_Prog = 8; return; }
  if ( (valueADC>0x3B0)&&(valueADC<=0x3BF) ) { Reg_Prog = 7; return; }
  if ( (valueADC>0x370)&&(valueADC<=0x377) ) { Reg_Prog = 6; return; }
  if ( (valueADC>0x320)&&(valueADC<=0x333) ) { Reg_Prog = 5; return; }
  if ( (valueADC>0x2E0)&&(valueADC<=0x2EF) ) { Reg_Prog = 4; return; }
  if ( (valueADC>0x2A0)&&(valueADC<=0x2AA) ) { Reg_Prog = 3; return; }
  if ( (valueADC>0x250)&&(valueADC<=0x268) ) { Reg_Prog = 2; return; }
  if ( (valueADC>0x210)&&(valueADC<=0x222) ) { Reg_Prog = 1; return; }
  if ( (valueADC>0x030)&&(valueADC<=0x04F) ) { Reg_Prog = 10; return; }
  if ( (valueADC>0x080)&&(valueADC<=0x08F) ) { Reg_Prog = 11; return; }
  if ( (valueADC>0x0C0)&&(valueADC<=0x0CF) ) { Reg_Prog = 12; return; }
  if ( (valueADC>0x100)&&(valueADC<=0x116) ) { Reg_Prog = 13; return; }
  if ( (valueADC>0x140)&&(valueADC<=0x15F) ) { Reg_Prog = 14; return; }
  if ( (valueADC>0x190)&&(valueADC<=0x19F) ) { Reg_Prog = 15; return; }
  if ( (valueADC>0x1D0)&&(valueADC<=0x1E8) ) { Reg_Prog = 16; return; }  
}


