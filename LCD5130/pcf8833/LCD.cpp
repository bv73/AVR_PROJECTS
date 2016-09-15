
/* Copyright (c) 2010, Peter Barrett
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/


#include "LCD.h"
#include "WProgram.h"	
#include <avr/pgmspace.h>

void SpiCmd(u8 c);
void SpiData(u8 c);

#define DELAY_MS 0x200

#ifndef abs
#define abs(_x) ((_x) < 0 ? -(_x) : (_x))
#endif
#ifndef max
#define max(_a,_b) (((_a) > (_b)) ? (_a) : (_b))
#define min(_a,_b) (((_a) < (_b)) ? (_a) : (_b))
#endif

//=================================================================================
//=================================================================================
//	Autodetect many nokia displays by rdid commands


// ILI9163 RDDIDIF 54
// EPSON RDDIDIF 29
// PHILIPS RDDIDIF 45

//	SPFD54124B
//	RDDID (04h): Read Display ID 388X4F
//	162x132 ST7669V Sitronix suspected

//	96x65 Mono
//	ID 7C043E	Blue
//	ID 54A851	Nokia 1200

//	98x70 color
//	ID FE890E
//	ID FE850E
//	ID 45800E	dealextreme

//	132x132 Color Nokia 6100
//	PCF8833 == Leadis LDS176 (458003)
//	ID 29C024
//	ID FE8A24

//	132x162 262k
//	ID 389023
//	ID 3870E0 side connector 830A1880 d3h
//	ID 7C8051
//	ID FEE223
//	ID 548066
//	ID 7CF8F1 7C8066 <= OTP difference in id?
//	ID 7C8051 518000 <= cstn
//	ID FE903C  cstn



LCDInfo _lcd;	// LCD info for detected screen
//u8 _mono[96*9];	// Frame buffer for mono screens
u8 _mono[2];	// Frame buffer for mono screens


void Cmd88(u8 cmd, u8 a, u8 b)
{
	SpiCmd(cmd);
	SpiData(a);
	SpiData(b);
}

void Cmd1616(u8 cmd, int a, int b)
{
	SpiCmd(cmd);
	SpiData(a>>8);
	SpiData(a);
	SpiData(b>>8);
	SpiData(b);
}

//=================================================================================
//=================================================================================

#if 0

void FillGeneric(int color, u32 count)
{
	u8 a = (color >> 8);
	u8 b = color;
	if (count <= 0)
		return;
	while (count--)
	{
		SpiData(a);
		SpiData(b);
	}
}

//	TODO color should be a type
void FillGeneric24(int color, u32 count)
{
	if (count <= 0)
		return;
	int r = (color >> 16);
	int g = (color >> 8);
	int b = color;
	while (count--)
	{
		SpiData(r);
		SpiData(g);
		SpiData(b);
	}
}

void BlitGeneric(const u8* data, u32 count)
{
	if (count <= 0)
		return;
	while (count--)
	{
		SpiData(*data++);
		SpiData(*data++);
	}
}

void BlitIndexedGeneric(const u8* data, const u8* palette, u32 count)
{
	if (count <= 0)
		return;
	while (count--)
	{
		int i = *data++;
		i += i;
		SpiData(palette[i]);
		SpiData(palette[i+1]);
	}
}

#else

#define SPI_HW_OFF   SPCR = 0;
#define SPI_HW_ON    SPCR = _BV(MSTR) | _BV(SPE);
#define SPI_HW_WAIT  while (!(SPSR & _BV(SPIF)));


//  portB 8..13
#define MOSI 11
#define MISO 12
#define SCK 13
#define SSEL 10
#define BLIGHT 9
#define RESET 8

#define MOSI0 digitalWrite(MOSI,0)
#define MOSI1 digitalWrite(MOSI,1)

#define SCK0 digitalWrite(SCK,0)
#define SCK1 digitalWrite(SCK,1)

#define SSEL0 digitalWrite(SSEL,0)
#define SSEL1 digitalWrite(SSEL,1)

#define RESET0 digitalWrite(RESET,0)
#define RESET1 digitalWrite(RESET,1)

void LCDInitHW()
{
	SSEL1;
	RESET1;
	MOSI1;
	SCK0;

	pinMode(MOSI, OUTPUT);
	pinMode(SCK, OUTPUT);
	pinMode(SSEL, OUTPUT);
	pinMode(RESET, OUTPUT);
	pinMode(BLIGHT, OUTPUT);
}

void SpiCmd(u8 d)
{
	MOSI0;
	SCK1;
	SCK0;
	SPI_HW_ON;
	SPDR = d;
	SPI_HW_WAIT;
	SPI_HW_OFF;
}

void SpiData(u8 d)
{
	MOSI1;
	SCK1;
	SCK0;
	SPI_HW_ON;
	SPDR = d;
	SPI_HW_WAIT;
	SPI_HW_OFF;
}

int SpiBBRead(int bits)
{
	int n = 0;
	pinMode(MOSI, INPUT);
	while (bits--)
	{
		n <<= 1;
		SCK1;
		if (digitalRead(MOSI))
			n |= 1;
		SCK0;
	}
    pinMode(MOSI, OUTPUT);
	return n;
}

#define nop()  __asm__ __volatile__("nop")

//	Fill a 16 bit color
//	Blit 16 bit pixels
//	Blit 8 bit indexed pixels from palette
//	400,000 pixels/sec on a 16Mhz AVR for Solid fills and 16bpp blits
//	380,000 pixels/sec for indexed blits
//	The code looks crazy 'cause it is using shadow of the spi data clocks
//	do work for the next round. It is within 10% of a hardware 9 bit spi

void FillBlit(u32 color, u32 count, const u8* data = 0, const u8* palette = 0)
{
	PORTB |= (1<<3);			// MOSI = 1 = Data
	u8 sck0 = PORTB & ~(1<<5);	// SCK
	u8 sck1 = PORTB | (1<<5);	// out is 1 clock vs sbi at 2
	SPSR |= 1;	// 2x clock

	u8 a,b;
	
	//	Handle 16 bit blits and index mode
	signed char x = 0;
	u8 y;
	const u8* p = 0;
	if (data)
	{
		a = *data++;
		x = 1;
		if (palette)
		{
			p = palette + ((*data++) << 1);
			a = p[0];
			b = p[1];
			x = -1;
			y = 0;
		}
	}
	else
	{
		a = color >> 8;
		b = color;
	}

	count <<= 1;
	int loop = count >> 8;
	u8 c = count;
	if (c)
		loop++;
	while (loop)
	{
		for (;;)
		{
			SPCR = 0;						// Turn off SPI HW
			PORTB = sck1;
			PORTB = sck0;					// Clock MOSI 1 bit to signify data on 9 bit spi
			SPCR = _BV(MSTR) | _BV(SPE);	// SPI Master
			SPDR = a;						// Now we have 16 clocks to kill

			//	16 clocks to do fill, blit or indexed blit

			//	Indexed blit - 380k
			if (x < 0)
			{
				if ((y^=1))
				{
					a = b;
					p = palette + ((*data++) << 1);
					if (--c)
						continue;	// Early out
				} else {
					a = p[0];
					b = p[1];
					if (--c)
						continue;	// Early out
				}
				break;
			}


			if (x)
			{
			//	Blit
				a = *data++;
				nop();
				nop();		// 400k
			}
			else
			{
				// Solid Fill
				u8 t = a;	// swap pixel halves
				a = b;
				b = t;
				nop();
				nop();		// 400k
			}
			if (!--c)
				break;	// 5 
		};				// loop takes 3 clocks
		c = 0;
		loop--;
		while (!(SPSR & _BV(SPIF)));	// wait for last pixel
	}
	SPCR = 0;
}

void FillGeneric(u32 color, u32 count)
{
	FillBlit(color,count);
}

//	TODO color should be a type
void FillGeneric24(u32 color, u32 count)
{
	if (count <= 0)
		return;
	u8 r = (color >> 16);
	u8 g = (color >> 8);
	u8 b = color;
	while (count)
	{
		u8 c = 255;
		if (count < c)
			c = count;
		count -= c;
		while (c--)
		{
			SpiData(r);
			SpiData(g);
			SpiData(b);
		}
	}
}

void BlitGeneric(const u8* data, u32 count)
{
	FillBlit(0,count,data);
}

void BlitIndexedGeneric(const u8* data, const u8* palette, u32 count)
{
	FillBlit(0,count,data,palette);
}

#endif

void RectangleGeneric(int x, int y, int width, int height, int color)
{
	int right = x + width;
	if (x < 0)
		x = 0;
	if (right > _lcd.width)
		right = _lcd.width;
	width = right - x;
	if (width <= 0)
		return;

	int bottom = y + height;
	if (y < 0)
		y = 0;
	if (bottom > _lcd.height)
		bottom = _lcd.height;
	height = bottom - y;
	if (height <= 0)
		return;

	_lcd.SetBounds(x,y,width,height);
	_lcd.Fill(color,width*((long)height));
}

void InitGeneric(const short* data, int count)
{
	while (count--)
	{
		int b = pgm_read_word(data++);
		if (b == DELAY_MS)
		{
			b = pgm_read_word(data++);
			delay(b);
			count--;
		}
		else
		{
			if (b > 0xFF)
				SpiData(b);
			else
				SpiCmd(b);
		}
	}
}

//=================================================================================
//=================================================================================
//	PCF8833 Philips controller
//	128x128 (132x132) from Nokia 6100 etc Identifies as RDID3 = 0x24
//	98x70 from Nokia 1600 Identifies as RDID3 = 0x0E
//

static const short _initPCF8833[] PROGMEM =
{
	0x11,	// SLEEPOUT
	0x21,
	0x13,

	0x01,				// SWRESET
	DELAY_MS,10,
	0x11,				// SLEEPOUT
	DELAY_MS,10,

	0x36,0x100,			// MADCTL memory access control MY MX V LAO RGB X X X

	0x25,0x13F,			// SETCON set contrast
	DELAY_MS,10,

	0x29,				// DISPON display on

	0xBA, 0x107, 0x115,	// Data Order
	0x25, 0x13F,		// Contrast
	0x11,				// Sleep Out
	0x13,				// Display Normal mode

	0x37,0x100,			// VSCROLL ADDR
	0x3A,0x105,			// COLMOD pixel format 4=12,5=16,6=18
};

void InitPCF8833()
{
	InitGeneric(_initPCF8833,sizeof(_initPCF8833)/2);
}

void SetBoundsPCF8833(int x, int y, int width, int height)
{
	Cmd88(0x2A,x,x+width-1);	// column start/end
	Cmd88(0x2B,y,y+height-1);	// page start/end
	SpiCmd(0x2C);				// RAMWR
}

//=================================================================================
//=================================================================================
//	S1D15G10D Epson

const short _initS1D15G10[] PROGMEM =
{
	0xCA,	// DISCTL
	0x10C,
	0x120,
	0x10C,

	0xBB,	// COMSCN
	0x101,

	0x94,	// SLPOUT

	0xD1,	// OSCON

	0x81,	// VOLCTR
	0x129,
	0x103,

	0x82,	// TMPGRD
	0x101,
	0x100,
	0x100,
	0x100,
	0x100,
	0x101,
	0x115,
	0x117,
	0x122,
	0x124,
	0x126,
	0x128,
	0x100,
	0x117,

	0x20,		//	PWRCTR
	0x108,

	DELAY_MS,100,

	0x20,		// PWRCTR
	0x10F,

	0xA9,		// PTLOUT

	0xAA,		// ASCSET
	0x102,
	0x122,
	0x122,
	0x103,

	0xAB,	// SCSTART
	0x100,

	0xBC,	// DATCTL
	0x100,
	0x100,
	0x104,	// 0000RRRR GGGGBBBB, was 0x102

	0xA7,	// DISINV?
	0xAF	// display on
};

void InitS1D15G10()
{
	InitGeneric(_initS1D15G10,sizeof(_initS1D15G10)/2);
}

void SetBoundsS1D15G10(int x, int y, int width, int height)
{
	Cmd88(0x15,x,x+width-1);	// column start/end
	Cmd88(0x75,y,y+height-1);	// page start/end
	SpiCmd(0x5C);				// RAMWR
}

//=================================================================================
//=================================================================================
//	SPFD54124B Orise
//	132x162 TFT RDID is 0x38 0x84 0x4F
//	Nokia 6060 6101 6125 5200 6070 0.5mm connector
//	Nokia 2630 2760 Side 0.4mm connector
//	Nokia 3500 top

static const short _initSPFD54124B[] PROGMEM =
{
	0xBA, 0x107, 0x115,	// Data Order
	0x25, 0x13F,		// Contrast
	0x11,				// Sleep Out
	0x13,				// Display Normal mode

	0x37,0x100,			// VSCROLL ADDR
	0x3A,0x105,			// COLMOD pixel format 4=12,5=16,6=18
	0x29,				// DISPON
	0x20,				// INVOFF
	0x13,				// NORON
};

void InitSPFD54124B()
{
	InitGeneric(_initSPFD54124B,sizeof(_initSPFD54124B)/2);

	SpiCmd(0x2D);
	int i;
	for (i = 0; i < 32; i++)
		SpiData(i<<1);
	for (i = 0; i < 64; i++)
		SpiData(i);
	for (i = 0; i < 32; i++)
		SpiData(i<<1);
}

void SetBoundsSPFD54124B(int x, int y, int width, int height)
{
	Cmd1616(0x2A,x,x+width-1);	// column start/end
	Cmd1616(0x2B,y,y+height-1);	// page start/end
	SpiCmd(0x2C);				// RAMWR
}

//=================================================================================
//=================================================================================
//	Mono96x65
//

const short _initMono96x65[] PROGMEM =
{
	0xE2,	//
	DELAY_MS,5,
	0xAF,
	0xA4,
	0x2F,
	0xB0,	// Page
	0x10,
	0x00,	// Col low
};


#if 1
u8 _gray = 0;
u8 _prnd = 0;

//	65k period
u16 xor16(void)
{
	static u16 y16 = 0xC0ED;
    y16 ^= (y16 << 1);
    y16 ^= (y16 >> 1);
    return y16 ^= (y16 << 14);
}

//	128 bit period. Nice
u32 xor128(void)
{
  static u32 x = 123456789;
  static u32 y = 362436069;
  static u32 z = 521288629;
  static u32 w = 88675123;
  u32 t;

  t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

#endif

#define RNDOM xor128

//	LCDPoint just mono todo
void MPt(u8 x, u8 y, u8 color)
{
	if (x > 95) return;
	if (y > 64) return;
	int offset = (y >> 3);
	u8* p = _mono + offset * 96 + x;
	y = 1 << (y & 7);

	if (_gray)
		color = color < ((u8)RNDOM());

	if (color)
		*p |= y;
	else
		*p &= ~y;
}


void MHLine(u8 x, u8 y, u8 width, u8 color)
{
	int offset = (y >> 3);
	u8* p = _mono + offset * 96 + x;
	y = 1 << (y & 7);

	if (_gray)
	{
		while (width--)
		{
			if (color >= ((u8)RNDOM()))
				*p |= y;
			else
				*p &= ~y;
			p++;
		}
		return;
	}

	if (color)
	{
		while (width--)
			*p++ |= y;
	} else {
		while (width--)
			*p++ &= ~y;
	}
}


#define SWAP(_a,_b) { _a = _a ^ _b; _b = _a ^ _b; _a = _a ^ _b; }

void LCDLine(int x0, int y0, int x1, int y1, int color)
{
	int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		SWAP(x0, y0);
		SWAP(x1, y1);
	}
	if (x0 > x1) {
		SWAP(x0, x1);
		SWAP(y0, y1);
	}

	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax / 2;		// please don't compile as a div...
	int ystep;
	int y = y0;
	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	int x;
	for (x = x0; x <= x1; x++)
	{
		if (steep)
			MPt(y, x, color);
		else
			MPt(x, y, color);
		error -= deltay;
		if (error < 0)
		{
			y += ystep;
			error += deltax;
		}
	}
}

void MRect(u8 x, u8 y, u8 width, u8 height, u8 color)
{
	while (height--)
	{
		MHLine(x,y,width,color);
		y++;
	}
}

void BlitMono()
{
	SpiCmd(0xB0);
	SpiCmd(0x10);
//	SpiCmd(0x00);
	int i;
	for (i = 0; i < sizeof(_mono); i++)
		SpiData(_mono[i]);
}

void InitMono96x65()
{
	InitGeneric(_initMono96x65,sizeof(_initMono96x65)/2);
}

int _mx = 0;
int _my = 0;
int _my2 = 0;
int _mx2 = 0;
void FillMono96x65(u32 color, u32 count)
{
	color = (color >> 3) & 0xFF;	// green + a bit of red ~ luma
	color = 0xff - color;
	_gray = 1;
	while (count > 0)
	{
		int c = min((int)count,_mx2-_mx);
		if (c == 0)
		{
			_mx = 0;
			_my++;
			if (_my >= _my2)
				return;
		}
		else
		{
			MHLine(_mx,_my,c,color);
			count -= c;
			_mx += c;
		}
	}
	_gray = 0;
}

void SetBoundsMono96x65(int x, int y, int width, int height)
{
	_mx = x;
	_my = y;
	_my2 = _my + height;
	_mx2 = _mx + width;
}

//=================================================================================
//=================================================================================

u32 LCDReadReg(u8 a, int count)
{
	SpiCmd(a);
	return SpiBBRead(count);
}

u32 LCDReadID()
{
	SSEL1;
	SSEL0;
	u8 a = LCDReadReg(0xDA,8);		// Should match RDID4
	u8 b = LCDReadReg(0xDB,8);		// De-selecting between reads will confuse Nokia 1200
	u8 c = LCDReadReg(0xDC,8);
	SSEL1;
	return (((u32)a) << 16) | (((u16)b) << 8) |  c;
}

void LCDBegin()	// Select LCD
{
	SSEL0;
}

void LCDEnd()		// Release LCD
{
	if (_lcd.format == 1)
	{
		BlitMono();
		memset(_mono,0,sizeof(_mono));
	}
	SSEL1;
}

u32 LCDChanged()
{
	u32 id = LCDReadID();
	if (id == _lcd.id)
		return 0;
	return id;
}

int LCDInit()
{
	LCDInitHW();
	delay(120);						// Too long?

	SSEL0;
	SpiCmd(0x11);					// SLEEPOUT
	delay(120);						// Too long?
	SSEL1;

	u32 id = LCDReadID();
	_lcd.id = id;

	RESET0;
	delay(1);
	RESET1;
	delay(100);	// too short?

	if ((id & 0xFF00FF) == 0x540051)	// Nokia 1200 id collision with 132x162
		id &= 0xFF00FF;
	else
		id &= 0xFF;

	LCDInfo* info = &_lcd;
	info->Fill = FillGeneric;
	info->Blit = BlitGeneric;
	info->BlitIndexed = BlitIndexedGeneric;
	info->Rectangle = RectangleGeneric;
	info->width = 132;
	info->height = 132;
	info->format = 16;

	SCK0;
	SSEL0;
	switch (id)
	{
		case 0x0E:
		case 0x24:
			InitPCF8833();
			info->SetBounds = SetBoundsPCF8833;
			if (id == 0x0E)
			{
				info->width = 98;
				info->height = 70;
			} else {
				// 0xFE 0x8C 0x24
				// 0x29 0xC0 0x24 - leadis?
			}
			break;

		case 0x23:
		case 0x3C:
		case 0x4F:
		case 0x51:
		case 0x66:
		case 0xE0:
		case 0xF1:
		case 0x7F:
			// RDID is 0x38 0x84 0x4F
			// 0x7C 0x80 0x51
			InitSPFD54124B();
			info->SetBounds = SetBoundsSPFD54124B;
			info->height = 162;
			//info->Fill = FillGeneric24;
			break;

		case 0x3E:
		case 0x540051:
			InitMono96x65();
			info->SetBounds = SetBoundsMono96x65;
			info->Fill = FillMono96x65;
			info->width = 96;
			info->height = 65;
			info->format = 1;
			break;

		case 0x7F0:	// Nothing detected, it might be an Epson 6100 TODO: disabled
			InitS1D15G10();	// Epson
			info->SetBounds = SetBoundsS1D15G10;
			info->format = 12;
			break;

		default:
			SSEL1;
			return -1;
	}
	SSEL0;
	return 0;
}
