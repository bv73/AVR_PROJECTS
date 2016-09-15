//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// Atmel AVR MCU low-level driver.
// (c) Radosіaw Kwiecieс, radek@dxp.pl
// (c) Added & Corrected 18.5.2011 By (R)soft
//-------------------------------------------------------------------------------------------------

// Определение порта данных
#define KS0108_DATA_PORT  PORTC
#define KS0108_DATA_DIR	  DDRC
#define KS0108_DATA_PIN	  PINC

// Определение порта управления
#define KS0108_CTRL_PORT	PORTD
#define KS0108_CTRL_DIR		DDRD

// Определение битов управления
#define KS0108_RS		(1 << 0)
#define KS0108_RW		(1 << 1)
#define KS0108_EN		(1 << 2)
#define KS0108_CS1		(1 << 3)
#define KS0108_CS2		(1 << 4)
#define KS0108_RST PORTD_Bit5

// Определение функции pgm_read_byte для совместимости с IAR C
#define pgm_read_byte(a)  (*(unsigned char __flash *)(a))

extern unsigned char screen_x;
extern unsigned char screen_y;

#define DISPLAY_STATUS_BUSY	0x80

//-------------------------------------------------------------------------------------------------
// Disalbe Controller (0-1)
//-------------------------------------------------------------------------------------------------

void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_PORT &= ~KS0108_CS1; break; // CBI ctrl,3
	case 1 : KS0108_CTRL_PORT &= ~KS0108_CS2; break; // CBI ctrl,4
	}
}

//-------------------------------------------------------------------------------------------------
// Enable Controller (0-1)
//-------------------------------------------------------------------------------------------------

void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_PORT |= KS0108_CS1; break; // SBI ctrl,3
	case 1 : KS0108_CTRL_PORT |= KS0108_CS2; break; // SBI ctrl,4
	}
}

//-------------------------------------------------------------------------------------------------
// Read Status from specified controller (0-1)
//-------------------------------------------------------------------------------------------------

unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;

GLCD_EnableController(controller);
KS0108_DATA_PORT = 0xFF; // Установка резисторов подтяжки на входах при чтении байта готовности
KS0108_CTRL_PORT &= ~KS0108_RS; // CBI ctrl,0 (Instruction Mode)
KS0108_DATA_DIR = 0x00; // Select Input Mode

KS0108_CTRL_PORT |= KS0108_RW; // SBI ctrl,1 (Read Mode)
KS0108_CTRL_PORT |= KS0108_EN; // SBI ctrl,2 (Enable Select)
delay_us(10);
status = KS0108_DATA_PIN;
KS0108_CTRL_PORT &= ~KS0108_EN; // CBI ctrl,2
KS0108_CTRL_PORT &= ~KS0108_RW;
KS0108_DATA_DIR = 0xFF;

GLCD_DisableController(controller);
return status;
}

//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------

void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0xFF;
KS0108_CTRL_PORT &= ~(KS0108_RW | KS0108_RS); //IN PORTD; ANDI PORTD, 0xFC; OUT PORTD (Reset RW & RS Bits)

GLCD_EnableController(controller);

KS0108_DATA_PORT = commandToWrite;
KS0108_CTRL_PORT |= KS0108_EN; // SBI ctrl,2
delay_us(10);
KS0108_CTRL_PORT &= ~KS0108_EN; // CBI ctrl,2

GLCD_DisableController(controller);
}

//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------

unsigned char GLCD_ReadData(void)
{
unsigned char data;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0x00;
KS0108_CTRL_PORT |= (KS0108_RW | KS0108_RS);

GLCD_EnableController(screen_x / 64);
KS0108_CTRL_PORT |= KS0108_EN;
delay_us(10);
data = KS0108_DATA_PIN;
KS0108_CTRL_PORT &= ~KS0108_EN;

GLCD_DisableController(screen_x / 64);
screen_x++;
return data;
}

//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------

void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR = 0xFF;
KS0108_CTRL_PORT &= ~KS0108_RW; //=0
KS0108_CTRL_PORT |= KS0108_RS;  //=1
KS0108_DATA_PORT = dataToWrite;

GLCD_EnableController(screen_x / 64);
KS0108_CTRL_PORT |= KS0108_EN;
delay_us(10);
KS0108_CTRL_PORT &= ~KS0108_EN;

GLCD_DisableController(screen_x / 64);
screen_x++;
}

//-------------------------------------------------------------------------------------------------
// Инициализация портов ЖКИ
//-------------------------------------------------------------------------------------------------

void GLCD_InitializePorts(void)
{
  KS0108_DATA_DIR = 0xFF; // All output for Data Bus
  KS0108_DATA_PORT = 0xFF;

  KS0108_CTRL_DIR = 0xFF; // All output for Control Port
  KS0108_CTRL_PORT = 0x00; // Сбросили все (Ресет тоже)
  delay_us(50);
  KS0108_RST=1; // Release RST Pin
  delay_us(10);
}

//-------------------------------------------------------------------------------------------------
// Чтение байта из памяти программ
//-------------------------------------------------------------------------------------------------

unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
return pgm_read_byte(ptr);
}
