/*----------------------------------------------------------------------------
 * Name:    GLCD.c
 * Purpose: MCBSTM32C low level Graphic LCD (320x240 pixels) functions
 * Version: V1.00
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#pragma diag_suppress=550
#include "english_16x8.h"
#include "stm32f10x.h"
#include "GLCD.h"
#include "font.h"
#include "delay.h"
/* SPI_SR - bit definitions. */
#define RXNE    0x01
#define TXE     0x02
#define BSY     0x80




/*********************** Hardware specific configuration **********************/

/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N    18

/*---------------------- Graphic LCD size definitions ------------------------*/

#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#define BPP         16                  /* Bits per pixel                     */
#define BYPP        ((BPP+7)/8)         /* Bytes per pixel                    */

/*--------------- Graphic LCD interface hardware definitions -----------------*/

#define SPI_START   (0x70)              /* Start byte for SPI transfer        */
#define SPI_RD      (0x01)              /* WR bit 1 within start              */
#define SPI_WR      (0x00)              /* WR bit 0 within start              */
#define SPI_DATA    (0x02)              /* RS bit 1 within start byte         */
#define SPI_INDEX   (0x00)              /* RS bit 0 within start byte         */
 
/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
static volatile unsigned short TextColor = Black, BackColor = White;


/************************ Local auxiliary functions ***************************/

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/

//static void delay (int cnt) {

 // cnt <<= DELAY_2N;
//  while (cnt--);
//}

/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

__inline void wr_dat (unsigned short c) {

	 Set_nRd;
     Set_Rs;
     Clr_Cs;  
     Clr_nWr;
  	GPIOE->ODR = c;
	Set_nWr;
    Set_Cs;
  
}

/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

__inline void wr_cmd (unsigned char c) {

	 Set_nRd;
     Clr_Rs;
     Clr_Cs;  
     Clr_nWr;
	 GPIOE->ODR = c;
	 Set_nWr;
     Set_Cs;
}

/*******************************************************************************
* Read data from LCD controller                                                *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

__inline unsigned short rd_dat (void) {
  unsigned short val = 0;

	Set_Rs;
	Set_nWr;
	Clr_nRd;
    GPIOE->CRH = 0x44444444;
	GPIOE->CRL = 0x44444444;
	val = GPIOE->IDR;
	val = GPIOE->IDR;
	GPIOE->CRH = 0x33333333;
	GPIOE->CRL = 0x33333333;
	Set_nRd;
	return val;
}

/*******************************************************************************
* Write to LCD register                                                        *
*   Parameter:    reg:    register to be read                                  *
*                 val:    value to write to register                           *
*******************************************************************************/

__inline void wr_reg (unsigned char reg, unsigned short val) {

  	//Clr_Cs;
	wr_cmd(reg);      
	wr_dat(val);    
//	Set_Cs; 
}


/*******************************************************************************
* Read from LCD register                                                       *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from register                             *
*******************************************************************************/

unsigned short rd_reg (unsigned short reg) {

  	Clr_Cs;
	wr_cmd(reg);     
	reg = rd_dat();      	
	Set_Cs;
	return reg;
}


/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_init (void)
 { 
  int deviceid;

  /* Enable clock for GPIOA,B,C,D,E AFIO and SPI3. */
  RCC->APB2ENR |= 0x0000007D;

  /* NCS is PB2, GPIO output set to high. */
  GPIOE->CRL = 0x33333333;	    //设置PE口为50M通用推挽输出模式
  GPIOE->CRH = 0x33333333;

  GPIOD->CRH &= 0x0000ffff;	    //PD12,PD13,14,15设置为50M通用推挽输出模式
  GPIOD->CRH |= 0x33330000;

  GPIOD->BSRR = 0x0000F000;		//cs,rs,wr,rd = 1;

  delay_ms(50);                             /* Delay 50 ms                        */

  
	deviceid = rd_reg(0x67);


wr_reg(0x0019,0x0071);//osc setting //71
delay_ms(10);
wr_reg(0x0093,0x0000);   //0ff
wr_reg(0x001B,0x0018);//exit sleep
delay_ms(10);

wr_reg(0x001B,0x0014);//exit sleep
delay_ms(50);


/***************display setting*******************/

wr_reg(0x0001,0x0006);//display control 3
delay_ms(10);

wr_reg(0x0016,0x00c8);//memory access control //c8
delay_ms(10);

wr_reg(0x0018,0x0000);//memory access control
delay_ms(10);

wr_reg(0x0023,0x0095);//cycle control  //95
delay_ms(10);

wr_reg(0x0024,0x0095);//cycle control
delay_ms(10);

wr_reg(0x0025,0x00ff);//cycle control
delay_ms(10);

wr_reg(0x0027,0x0002);// BP/FP SETTING
delay_ms(10);

wr_reg(0x0028,0x0002);// BP/FP SETTING
delay_ms(10);

wr_reg(0x0029,0x0002);// BP/FP SETTING
delay_ms(10);

wr_reg(0x002a,0x0002);// BP/FP SETTING
delay_ms(10);

wr_reg(0x002c,0x0002);// BP/FP SETTING
delay_ms(10);

wr_reg(0x002d,0x0002);// BP/FP SETTING
delay_ms(10);

wr_reg(0x003a,0x0000);//cycle control 1
delay_ms(10);

wr_reg(0x003b,0x0000);//cycle control 2
delay_ms(10);

wr_reg(0x003c,0x00f0);//cycle control 3
delay_ms(10);

wr_reg(0x003d,0x0000);//cycle control 4
delay_ms(20);


/**********power setting**********************/

wr_reg(0x0042,0x001f);//BGP control
delay_ms(50);

wr_reg(0x001c,0x0006);// AP2-0 SETTING 
delay_ms(30);

wr_reg(0x001D,0x0000);//VLCD SETTING  
delay_ms(30);

wr_reg(0x001E,0x0000);//VREG3 SETTING  
delay_ms(30);

wr_reg(0x001F,0x00e);//VREG1 //00  //0e
delay_ms(30);

wr_reg(0x0020,0x0010);// VGL/VGH SETTING   
delay_ms(30);

wr_reg(0x0021,0x0011);// DC/DC SETTING   
delay_ms(30);

wr_reg(0x0043,0x0080);//vomg control
delay_ms(50);

wr_reg(0x0044,0x0036);  //vcomh control//35
delay_ms(10);

wr_reg(0x0045,0x000a);//vcom control //0a
delay_ms(20);

/******************GAMMA SETTING***********************/

wr_reg(0x0046,0x0094);//GAMMA control 1  

wr_reg(0x0047,0x0021);//GAMMA control 2  

wr_reg(0x0048,0x0000);//GAMMA control 3   

wr_reg(0x0049,0x0033);//GAMMA control 4  

wr_reg(0x004A,0x0023);//GAMMA control 5  

wr_reg(0x004B,0x0045);//GAMMA control 6   

wr_reg(0x004C,0x0044);//GAMMA control 7 

wr_reg(0x004D,0x0077);//GAMMA control  8 

wr_reg(0x004E,0x0012);//GAMMA control 9   

wr_reg(0x004F,0x0001);//GAMMA control 10 

wr_reg(0x0050,0x0001);//GAMMA control 11  

wr_reg(0x0051,0x0001);//GAMMA control 12  
delay_ms(100);
/****************display on setting**************/

wr_reg(0x0026,0x0004);
delay_ms(20);

wr_reg(0x0026,0x0024);
delay_ms(20);

wr_reg(0x0026,0x002c);
delay_ms(20);

wr_reg(0x0026,0x003c);
delay_ms(20);

	 //ili9320_WriteIndex(0x0022);	  
  DrawBlock (0,0,139 ,219,Red);	  

}


void GLCD_putPixel(unsigned int x, unsigned int y) {
  wr_reg(0x20, x);
  wr_reg(0x21, y);
  wr_reg(0x22,TextColor);
}


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_setTextColor(unsigned short color) {
  TextColor = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_setBackColor(unsigned short color) {
  BackColor = color;
}


/*******************************************************************************
* Clear display                                                                *
*   Parameter:    color:  color for clearing display                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_clear (unsigned short color) {
  unsigned int   i;

  wr_reg(0x20, 0);
  wr_reg(0x21, 0);
  Clr_Cs;
  wr_cmd(0x22);
  for(i = 0; i < (WIDTH*HEIGHT); i++)
  {
  	wr_dat(color);
  }
  Set_Cs;  
}


/*******************************************************************************
* Draw character on given position (line, coloum                               *
*   Parameter:     x :        horizontal position                              *
*                  y :        vertical position                                *
*                  c*:        pointer to color value                           *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_drawChar(unsigned int x, unsigned int y, unsigned short *c) {
  unsigned int index = 0;
  int  i = 0;
  unsigned int Xaddress = 0;
   
  Xaddress = x;
  
  wr_reg(0x20, Xaddress);
  wr_reg(0x21, y);

  for(index = 0; index < 24; index++)
  {
  	Clr_Cs;
    wr_cmd(0x22);              /* Prepare to write GRAM */
    for(i = 15; i >= 0; i--)
//    for(i = 0; i < 16; i++)
    {
      if((c[index] & (1 << i)) == 0x00) {
        wr_dat(BackColor);
      } else {
        wr_dat(TextColor);
      }
    }
	Set_Cs;
    Xaddress++;
    wr_reg(0x20, Xaddress);
    wr_reg(0x21, y);
  }
}


/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:     c :        ascii character                                  *
*                  ln:        line number                                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_displayChar(unsigned int ln, unsigned int col, unsigned int  c) {
/*
   unsigned char avl,i,n;
  wr_cmd(0x0022); 
  for (i=0;i<16;i++)
  { 
      avl=english[ln-32][i];
	  for (n=0;n<8;n++)
	   {
	     if(avl&0x80) wr_dat(col);
             else wr_dat(c);
	     avl<<=1;
	   }
	} 
  */
  unsigned char avl,i,n;
  wr_cmd(0x0022);
  for(i=0;i<16;i++)
  {
	avl=english[ln-32][i];
	for (n=0;n<8;n++)
	{
		if(avl&0x80) 
			wr_dat(col);
		else 
			wr_dat(c);
		avl<<=1;
	}
  }
 
 // c -= 32;
// GLCD_drawChar(ln, col, &ASCII_Table[c * 24]);
}


/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:     s*:        pointer to string                                *
*                  ln:        line number                                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_displayStringLn(unsigned int x,unsigned int y,unsigned char *s,unsigned int color,unsigned int xcolor)
 {
 
  unsigned int k=0;
  while (*s) 
  {
     Set_Wingdows(x+k,y,x+7+k,y+15);
     GLCD_displayChar( *s,color,xcolor);
     k=k+8;
     s++;
  
  }
   
}

void Set_Wingdows (unsigned int startx,unsigned int starty,unsigned int endx,unsigned int endy) 
{ 

 wr_reg(0x0002,startx>>8); 
 wr_reg(0x0003,startx); 
 wr_reg(0x0004,endx>>8); 
 wr_reg(0x0005,endx); 
 wr_reg(0x0006,starty>>8); 
 wr_reg(0x0007,starty); 
 wr_reg(0x0008,endy>>8); 
 wr_reg(0x0009,endy); 
 wr_reg(0x0016,0x00C8); 
}

/*******************************************************************************
* Clear given line                                                             *
*   Parameter:     ln:        line number                                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_clearLn(unsigned int ln) {
  GLCD_displayStringLn(10,46,"STM32 ADC Test",White,Black);
}


/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) {
  unsigned int   i;
  unsigned int   len = w*h;
  unsigned short *bitmap_ptr = (unsigned short *)bitmap;

  wr_reg(0x50, y);                      /* Horizontal GRAM Start Address      */
  wr_reg(0x51, y+h-1);                  /* Horizontal GRAM End   Address (-1) */
  wr_reg(0x52, x);                      /* Vertical   GRAM Start Address      */
  wr_reg(0x53, x+w-1);                  /* Vertical   GRAM End   Address (-1) */

  wr_reg(0x20, y);
  wr_reg(0x21, x);

  Clr_Cs;
  wr_cmd(0x22);
  for (i = 0; i < len; i++) {
    wr_dat(*bitmap_ptr++);
  }
  Set_Cs;
}

void drawInfo(unsigned int txtc, unsigned int bkc)
{

	GLCD_displayStringLn(10,46,"INANBO-STM32-TEST BOARD",txtc,bkc);
	GLCD_displayStringLn(10,66 , "INANBO-T28-8347",txtc,bkc);
	GLCD_displayStringLn(10,86 , "www.inanbo.com",txtc,bkc );
	GLCD_displayStringLn(10,106 , "DRIVER IC:  HX8347",txtc,bkc);
}

/****************************************************************************
* 名    称：void GLCD_Test(void)
* 功    能：测试液晶屏
* 入口参数：无
* 出口参数：无
* 说    明：显示彩条，测试液晶屏是否正常工作
* 调用方法：GLCD_Test();
****************************************************************************/
void GLCD_Test(void)
{
  unsigned int i; 
	u16 RED,GREEN,BLUE,WHITE,BLACK;
	RED = 0xFF<<11|0<<5|0;
	GREEN = 0<<11|0xFF<<5|0;
	BLUE = 0x00<<11|0<<5|0xFF;
	WHITE = 0xFF<<11|0xFF<<5|0xFF;
	BLACK = 0<<11|0<<5|0; 

	
  while(1)
 {
 	GLCD_init();
	DrawBlock(0,0,239,319,BLACK);
	drawInfo(RED,BLACK);
	delay_ms(20000);
	DrawBlock(0,0,239,319,WHITE);
	drawInfo(BLUE,WHITE);
	delay_ms(20000);
	DrawBlock(0,0,239,319,BLUE);
	drawInfo(BLACK,BLUE);
	delay_ms(20000);     
	DrawBlock(0,0,239,319,GREEN);
	drawInfo(BLACK,GREEN);
	delay_ms(20000);
	DrawBlock(0,0,239,319,RED);
	drawInfo(BLACK,RED);
	delay_ms(20000);
 }

//	u16 i,j;
 // wr_reg(0x20, 0);
 // wr_reg(0x21, 0);

 // Clr_Cs; 
//  wr_cmd(0x22);
  
 // for(i=0;i<320;i++)
//    for(j=0;j<240;j++)
 //   {
 //     if(i>279)wr_dat(0x0000);
 //     else if(i>239)wr_dat(0x001f);
  //    else if(i>199)wr_dat(0x07e0);
   //   else if(i>159)wr_dat(0x07ff);
   //   else if(i>119)wr_dat(0xf800);
   //   else if(i>79)wr_dat(0xf81f);
 //     else if(i>39)wr_dat(0xffe0);
 //     else wr_dat(0xffff);
 //   }
	
//  Set_Cs;
//   delay(10);
 }
void DrawBlock (unsigned int startx,unsigned int starty,unsigned int endx,unsigned int endy,unsigned int color) 
{ 
 unsigned int i,j; 

  
 wr_reg(0x0002,startx>>8); 
 wr_reg(0x0003,startx); 
 wr_reg(0x0004,endx>>8); 
 wr_reg(0x0005,endx); 
 wr_reg(0x0006,starty>>8); 
 wr_reg(0x0007,starty); 
 wr_reg(0x0008,endy>>8); 
 wr_reg(0x0009,endy); 
 wr_reg(0x0016,0x00C8); 
 wr_cmd(0x0022); 
  
 for(i=starty;i<=endy;i++) 
   { 
     for(j=startx;j<=endx;j++) 
     { 
        wr_dat(color); 
     } 
   }  
}



/******************************************************************************/
