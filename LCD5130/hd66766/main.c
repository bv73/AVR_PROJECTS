#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include <string.h> // for memset

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}



#include "font5x7.h"

#include "sed15b1.h"
#include "sed15b1.c"

#include "hd66766.h"
#include "hd66766.c"

float a, b, c;


void main( void )
{


  


  unsigned int type = bmp_file.bfh.bfType;
  unsigned int Width = bmp_file.bih.biWidth; // Ширина картинки
  unsigned int Height = bmp_file.bih.biHeight;  // Высота картинки
//  RGBTRIPLE color;
  
//  memset (Palette,0,1024);
//  memset (&bfh, 0, sizeof(bfh));
//  bfh.bfType = 0x4D42;
  unsigned int main_offset = bmp_file.bfh.bfOffBits; // отступ от начала файла где лежат данные RGB

  unsigned int file_size = bmp_file.bfh.bfSize;  // размер файла

//  unsigned char byte0, byte1, byte2;
//  byte0=bmp_file.massiv[0];
//  byte1=bmp_file.massiv[1];
//  byte2=bmp_file.massiv[2];
//  memset (&bih, 0, sizeof(bih));
//  bih.biSize = sizeof(bih); // Так положено
//  bih.biBitCount = 24;      // 24 бит на пиксель
//  bih.biCompression = BI_RGB;   // Без сжатия
//  bih.biHeight = Height;
//  bih.biWidth = Width;
//  bih.biPlanes = 1;           // Должно быть 1


  
  //-------------------------
  a=54.321;
  b=3.14;
  c=a/b;

  unsigned int contrast=0x2C;
  
  hd66766_init();
  hd66766_clear_console(0x20); // Чистим пробелами

//  unsigned int id0, id1;
//  hd66766_write(0,HD66766_R00);  // Index Register
//  id0=hd66766_read(0); // rs=0 contrast code read
//  id1=hd66766_read(1); // rs=1 device code read
  
  sed15b1_init();
  sed15b1_clear_console(0x20);
  
  sprintf(LIN1,"Type:%04X",type); // Должно быть 0x4D42
  sprintf(LIN2,"Width:%d",Width);
  sprintf(LIN3,"Height:%d",Height);
  sprintf(LIN4,"Offset:%02X",main_offset);
  sprintf(LIN5,"Filesize:%X",file_size);
//  sprintf(LIN6,"Byte0:%02X",byte0);
//  sprintf(LIN7,"Byte1:%02X",byte1);
//  sprintf(LIN8,"Byte2:%02X",byte2);
  
/*  
  sprintf(LINE_01,"Драйвер Hitachi HD66766");
  sprintf(LINE_02,"Colour LCD 160X120 pixels");
  sprintf(LINE_03,"65536 Colors Palette");
  sprintf(LINE_04,"For Alcatel 556/557");
  sprintf(LINE_05,"16-bit Parallel mode");
  sprintf(LINE_06,"Тест %g/%g=%g",a,b,c);
  sprintf(LINE_07,"Research & Coding");
  sprintf(LINE_08,"By (R)soft 2011");
*/
  
  sed15b1_update_console();

  hd66766_bmp_output(Width,Height,bmp_file.data_BGR);
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  
  while (1)
  {


    if (!KEY1)
      {
        contrast--;
        contrast&=0x7F;
        hd66766_write(0,HD66766_R04);  // Index Register 04 - Contrast Control
        hd66766_write(1,0x0600|contrast); // Low Byte - Contrast Value 00..7F

        sprintf(LIN8,"Contrast:%02X",contrast);
        sed15b1_update_console();

        delay_ms(50);
      }

    if (!KEY2)
      {
        contrast++;
        contrast&=0x7F;
        hd66766_write(0,HD66766_R04);  // Index Register 04 - Contrast Control
        hd66766_write(1,0x0600|contrast); // Low Byte - Contrast Value 00..7F

        sprintf(LIN8,"Contrast:%02X",contrast);
        sed15b1_update_console();

        delay_ms(50);
      }





  }
}
