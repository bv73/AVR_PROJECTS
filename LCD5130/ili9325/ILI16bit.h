/* 

	PIC 18F4520 ILI9325 Driver
	File Name: ILI16bit.h
	File Description: Header File
	Version 1.0
	Language: Microchip C18
	Blog post URL: http://sunbizhosting.co.uk/~spiral/blog/?p=18
	email: spiralbrain@yahoo.com

*/


#include <p18cxxx.h> 

   
                           //RRRRRGGGGGGBBBBB 
#define BLACK              0b0000000000000000
#define BRIGHTBLUE         0b0000000000011111
#define BRIGHTGREEN        0b0000011111100000
#define BRIGHTCYAN         0b0000011111111111
#define BRIGHTRED          0b1111100000000000
#define BRIGHTMAGENTA      0b1111100000011111
#define BRIGHTYELLOW       0b1111111111100000
#define BLUE               0b0000000000010000
#define GREEN              0b0000010000000000
#define CYAN               0b0000010000010000
#define RED                0b1000000000000000
#define MAGENTA            0b1000000000010000
#define BROWN              0b1111110000000000
#define LIGHTGRAY          0b1000010000010000
#define DARKGRAY           0b0100001000001000
#define LIGHTBLUE          0b1000010000011111
#define LIGHTGREEN         0b1000011111110000
#define LIGHTCYAN          0b1000011111111111
#define LIGHTRED           0b1111110000010000
#define LIGHTMAGENTA       0b1111110000011111
#define YELLOW             0b1111111111110000
#define ORANGE			   0b1111101111100000
#define WHITE              0b1111111111111111

#define GRAY0       	   0b1110011100011100
#define GRAY1         	   0b1100011000011000   
#define GRAY2              0b1010010100010100   
#define GRAY3              0b1000010000010000
#define GRAY4              0b0110001100001100
#define GRAY5              0b0100001000001000
#define GRAY6	           0b0010000100000100

typedef enum {UP, DOWN} DIRECTION; 

// Function Declaration	
	void lcd_init(void);
	unsigned int Lcd_ReadReg(unsigned int reg);
	void Lcd_Data(unsigned int data);
	void Lcd_Command(unsigned int reg,unsigned int cmd);
	void Lcd_SetPosition(unsigned int x,unsigned int y);
	void Lcd_Reg(unsigned int reg);
	void Lcd_Clear(unsigned int color);
	void Lcd_fill(unsigned int color);
	void scroll(DIRECTION direction, unsigned char speed, unsigned int steps);
	void Lcd_SetBox(unsigned int xStart,unsigned int yStart,unsigned int xLong,unsigned int yLong);
	void LCD_write_char(unsigned char data,unsigned int color,unsigned int xcolor, unsigned char size);
	void LCD_write_string(unsigned int x,unsigned int y,static const rom char *s,unsigned int color,unsigned int xcolor, unsigned char size);
	void LCD_write_string_var(unsigned int x,unsigned int y,char *s,unsigned int color,unsigned int xcolor, unsigned char size);
	void write_bmp(far const rom int *Bitmap, unsigned short x, unsigned short y, unsigned short w,  unsigned short h );

	
