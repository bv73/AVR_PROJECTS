/* 

	PIC 18F4520 ILI9325 Driver
	File Name: ILI16bit.c
	File Description: Code File
	Version 1.0
	Language: Microchip C18
	Blog post URL: http://sunbizhosting.co.uk/~spiral/blog/?p=18
	email: spiralbrain@yahoo.com

*/

#include <p18cxxx.h> 
#include <delays.h> 
#include <usart.h>
#include "pins.h"
#include "english_16X8.h"
#include "ILI16bit.h"


void lcd_init(void)
{
		
	LCD_RESET = 0;
	Delay10KTCYx(50);
	LCD_RESET = 1;	

	
	Lcd_Command(0xe3,0x3008);		//set internal timing
	Lcd_Command(0xe7,0x0012);
	Lcd_Command(0xef,0x1231);
	Lcd_Command(0x01,0x0100);		//set SS and SM bit
	Lcd_Command(0x02,0x0700);		//set 1 line inversion
//	Lcd_Command(0x03,0x1030);		//set GRAM write direction and BGR=1 PORTRAIT MODE
//	Lcd_Command(0x03,0x1018);		//set GRAM write direction and BGR=1 LANDSCAPE MODE connector on left
	Lcd_Command(0x03,0x1028);		//set GRAM write direction and BGR=1 LANDSCAPE MODE connector on right
//	Lcd_Command(0x03,0x1038);		//set GRAM write direction and BGR=1 PORTRAIT MODE
	
	Lcd_Command(0x04,0x0000);		//resize register
	Lcd_Command(0x08,0x0202);		//set the back porch and front porch
	Lcd_Command(0x09,0x0000);		//set non-display area refresh cycle ISC[3:0]
	Lcd_Command(0x0A,0x0000);		//FMARK function
	Lcd_Command(0x0C,0x0000);		//RGB interface setting
	Lcd_Command(0x0D,0x0000);		//Frame marker Position
	Lcd_Command(0x0F,0x0000);		//RGB interface Polarity

	/*** Power On sequence ***/
	Lcd_Command(0x10,0x0000);		//SAP, BT[3:0], AP, DSTB, SLP, STB
	Lcd_Command(0x11,0x0007);		//DC1[2:0], DC0[2:0], VC[2:0]
	Lcd_Command(0x12,0x0000);		//VREG1OUT voltage
	Lcd_Command(0x13,0x0000);		//VDV[4:0] for VCOM amplitude, discharge capacitor power voltage
	Delay10KTCYx(40); // 200ms       
	
	Lcd_Command(0x10,0x1690);		
	Lcd_Command(0x11,0x0220);
	Delay10KTCYx(10); // 50ms       

	Lcd_Command(0x12,0x0091);		
	Delay10KTCYx(10); // 50ms       

	Lcd_Command(0x13,0x1700);		//Set VDV[4:0] for VCOM amplitude
	Lcd_Command(0x29,0x001A);		//Set VCM[5:0]
	Delay10KTCYx(10); // 50ms       
	
	Lcd_Command(0x2B,0x000B);		//Set frame rate
	Delay10KTCYx(10); // 50ms       
// changed to make landscape	
//	Lcd_Command(0x20,0x0000);		//set GRAM horizontal address
//	Lcd_Command(0x21,0x0000);		//set GRAM vertical address
	
	Lcd_Command(0x20,0x00EF);		//set GRAM horizontal address
	Lcd_Command(0x21,0x0000);		//set GRAM vertical address
	
	/*** adjust gamma curve ***/
	Lcd_Command(0x30,0x0007);
	Lcd_Command(0x31,0x0507);
	Lcd_Command(0x32,0x0006);
	Lcd_Command(0x35,0x0001);
	Lcd_Command(0x36,0x0709);	
	Lcd_Command(0x37,0x0104);
	Lcd_Command(0x38,0x0502);
	Lcd_Command(0x39,0x0706);
	Lcd_Command(0x3C,0x0500);
	Lcd_Command(0x3D,0x000C);

	/*** Set GRAM area ***/
    Lcd_Command(0x50,0x0000);			//Horizontal Address Start Position
    Lcd_Command(0x51,0x00EF);           //Horizontal Address end Position (239)
    Lcd_Command(0x52,0x0000);           //Vertical Address Start Position
    Lcd_Command(0x53,0x013F);           //Vertical Address end Position (319)
	Lcd_Command(0x60,0xA700);			//Gate scan line
	Lcd_Command(0x61,0x0001);			//NDL,VLE,REV
	Lcd_Command(0x6A,0x0000);			//set scrolling line
	
	/*** Partial Display Control***/
	Lcd_Command(0x80, 0x0000);
	Lcd_Command(0x81, 0x0000);
	Lcd_Command(0x82, 0x0000);
	Lcd_Command(0x83, 0x0000);
	Lcd_Command(0x84, 0x0000);
	Lcd_Command(0x85, 0x0000);

	/*** Panel control ***/
	Lcd_Command(0x90, 0x0010);
	Lcd_Command(0x92, 0x0000);
	Lcd_Command(0x93, 0x0003);
	Lcd_Command(0x95, 0x0110);
	Lcd_Command(0x97, 0x0000);
	Lcd_Command(0x98, 0x0000);
	Lcd_Command(0x07, 0x0173);
	return;
}
	
void scroll(DIRECTION direction, unsigned char speed, unsigned int steps){
	unsigned int col,i,j;
	j=0;
	col = Lcd_ReadReg(0x6A);
	
	for(i=0;i<steps;i++){	
		if(direction == DOWN) {
			if(col-j<1){col=319; j=0;}
			Lcd_Command(0x6A,col-j);			//NDL,VLE,REV	
		}else if(direction == UP){		
			if(col+j>319){col=0; j=0;}
			Lcd_Command(0x6A,col+j);			//NDL,VLE,REV
		}	
			Delay10KTCYx(speed); 
			j++;
		}
	return;		
}
	
void write_bmp(far const rom int *Bitmap, unsigned short x, unsigned short y, unsigned short w,  unsigned short h ) {
	unsigned int i,f;
	Lcd_SetBox(x,y,w,h);
	for(i=0;i<(w*h);i++)
		Lcd_Data(Bitmap[i]);
   	return;
}

	
	
	
void LCD_write_char(unsigned char data,unsigned int color,unsigned int xcolor, unsigned char size)
{
	unsigned char chr,i,n,m,k;
	for (i=0;i<(16);i++) 
	{ 

// 	chr= prog_char[data-32][i]; placed here for dashed font		
	for(k=0;k<size;k++)
	{
		chr= prog_char[data-32][i];
		for (n=0;n<8;n++) 
		{		
			for (m=0;m<size;m++)
			{
				if(chr&0x80) 
					Lcd_Data(color);
				else 
					Lcd_Data(xcolor);
			}
			chr<<=1;	
		}
	}			
	}
	return;
}


void LCD_write_string(unsigned int x,unsigned int y,static const rom char *s,unsigned int color,unsigned int xcolor, unsigned char size)
{
	unsigned int k=0;
	while (*s) {
		Lcd_SetBox(x,y+k,15*size,8*size);
		LCD_write_char( *s,color,xcolor,size);
		k=k+(8*size);
		s++;
	}
	return;
} 

void LCD_write_string_var(unsigned int x,unsigned int y,char *s,unsigned int color,unsigned int xcolor,	unsigned char size)
{
	unsigned int k=0;
		while (*s) {
		Lcd_SetBox(x,y+k,15*size,8*size);
		LCD_write_char( *s,color,xcolor,size);
		k=k+(8*size);
		s++;
	}
	return;
} 
	
	
void Lcd_SetBox(unsigned int xStart,unsigned int yStart,unsigned int xLong,unsigned int yLong)
{
	Lcd_SetPosition(xStart,yStart);
	Lcd_Command(0x0050,239 - xStart- xLong + 1);
	Lcd_Command(0x0051,239- xStart);
	Lcd_Command(0x0052,yStart);
	Lcd_Command(0x0053,yStart+yLong-1);
	Lcd_Reg(0x0022);
	return;
}

	
void Lcd_Clear(unsigned int color)
{
	unsigned int i;
	unsigned char j;
	
	Lcd_SetPosition(0,0);
	
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;
	LCD_CS = 0;
	
	DATA_LOW = color & 0x00ff;
	DATA_HIGH = ((color >> 8)&0x00ff);
	
	for(i=0;i<320;i++)
		for(j=0;j<240;j++) 
		{
			LCD_WR = 0;
			LCD_WR = 1;
		}
	LCD_CS = 1;
	return;
}

void Lcd_fill(unsigned int color)
{
	unsigned int i,j;
	
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;
	LCD_CS = 0;
	
	DATA_LOW = color & 0x00ff;
	DATA_HIGH = ((color >> 8)&0x00ff);
	
	for(i=0;i<320;i++)
	{
		for(j=0;j<8;j++) 
		{
			LCD_WR = 0;
			LCD_WR = 1;
		}
	}
	LCD_CS = 1;
	return;
}
	
void Lcd_Reg(unsigned int reg)
{
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 0;
	DATA_LOW = reg & 0x00ff;
	DATA_HIGH = ((reg >> 8)&0x00ff);
	LCD_CS = 0;
	LCD_WR = 0;
	
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;
	return;
}
	
void Lcd_Data(unsigned int data)
{
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;
	DATA_LOW = data & 0x00ff;
	DATA_HIGH = ((data >> 8)&0x00ff);
	LCD_CS = 0;
	LCD_WR = 0;
	
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;///////
	LCD_RS = 1;///////
	return;
}
	
void Lcd_Command(unsigned int reg,unsigned int cmd)
{
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 0;
		
	DATA_LOW = reg & 0x00ff;
	DATA_HIGH = ((reg >> 8)&0x00ff);
	
	LCD_CS = 0;
	LCD_WR = 0;
	
	LCD_WR = 1;
	LCD_RS = 1;
	
	DATA_LOW = cmd & 0x00ff;
	DATA_HIGH = ((cmd >> 8)&0x00ff);
	
	LCD_WR = 0;
	
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;
	return;
}
	
	
	
void Lcd_SetPosition(unsigned int x,unsigned int y)
{
	Lcd_Command(0x0020,239 - x);
	Lcd_Command(0x0021,y);
	Lcd_Reg(0x0022);
	return;
}
	
	
unsigned int Lcd_ReadReg(unsigned int reg)
{
	unsigned char dh, dl;
	unsigned int dat;
	
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 0;
	DATA_LOW = reg & 0x00ff;
	DATA_HIGH = ((reg >> 8)&0x00ff);

	LCD_CS = 0;
	LCD_WR = 0;
	
	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;

	TRISD = 0xFF;
	TRISB = 0xFF;
	
	LCD_CS = 0;
	LCD_RD = 0;

	dl = PORTD;
	dh = PORTB;

	LCD_CS = 1;
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RS = 1;

	TRISD = 0x00;
	TRISB = 0x00;
	return(((unsigned int)dh)<<8)|(dl);
}


