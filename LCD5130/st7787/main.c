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



#include "font/font5x7.h"

#include "sed15b1_3WIRE\sed15b1.h"
#include "sed15b1_3WIRE\sed15b1.c"

#include "st7787\st7787.h"
#include "st7787\st7787.c"

void main( void )
{

//  unsigned int type = bmp_file.bfh.bfType;
//  unsigned int Width = bmp_file.bih.biWidth; // Ширина картинки
//  unsigned int Height = bmp_file.bih.biHeight;  // Высота картинки
//  unsigned int main_offset = bmp_file.bfh.bfOffBits; // отступ от начала файла где лежат данные RGB
//  unsigned int file_size = bmp_file.bfh.bfSize;  // размер файла

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

  unsigned char command=0x22;
  unsigned char id1,id2,id3,id4,id5,id6,id7,id8,id9,id10,id11,id12,id13,id14;
  
  st7787_init();
//  st7787_fill(0x0000);

//  st7787_write_command(0x22);
//  st7787_write_data(0x55AA);
//  st7787_write_data(0xEEDD);
  
  st7787_write_command(command);
  id1=st7787_read();
  id2=st7787_read();
  id3=st7787_read();
  id4=st7787_read();
  id5=st7787_read();
  id6=st7787_read();
  id7=st7787_read();
  id8=st7787_read();
  id9=st7787_read();
  id10=st7787_read();
  id11=st7787_read();
  id12=st7787_read();
  id13=st7787_read();
  id14=st7787_read();  
  //**************************************************************  
  sed15b1_init();
  sed15b1_clear_console(0x20);
  
//  sprintf(LIN1,"Type:%04X",type);
//  sprintf(LIN2,"Width:%d",Width);
//  sprintf(LIN3,"Height:%d",Height);
//  sprintf(LIN4,"Offset:%02X",main_offset);
//  sprintf(LIN5,"Filesize:%X",file_size);
  sprintf(LIN1,"ID1:%02X ID8 :%02X",id1,id8);
  sprintf(LIN2,"ID2:%02X ID9 :%02X",id2,id9);
  sprintf(LIN3,"ID3:%02X ID10:%02X",id3,id10);
  sprintf(LIN4,"ID4:%02X ID11:%02X",id4,id11);
  sprintf(LIN5,"ID5:%02X ID12:%02X",id5,id12);
  sprintf(LIN6,"ID6:%02X ID13:%02X",id6,id13);
  sprintf(LIN7,"ID7:%02X ID14:%02X",id7,id14);  
  sprintf(LIN8,"Command:%02X",command);
  
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

//  hd66766_bmp_output(Width,Height,bmp_file.data_BGR);
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input


  while (1)
  {


  if (!KEY1)
      {
        command--;
//        command&=0x7F;
        st7787_write_command(command);
        id1=st7787_read();
        id2=st7787_read();
        id3=st7787_read();
        id4=st7787_read();
        id5=st7787_read();
        id6=st7787_read();
        id7=st7787_read();
        id8=st7787_read();
        id9=st7787_read();
        id10=st7787_read();
        id11=st7787_read();
        id12=st7787_read();
        id13=st7787_read();
        id14=st7787_read();  
        sprintf(LIN1,"ID1:%02X ID8 :%02X",id1,id8);
        sprintf(LIN2,"ID2:%02X ID9 :%02X",id2,id9);
        sprintf(LIN3,"ID3:%02X ID10:%02X",id3,id10);
        sprintf(LIN4,"ID4:%02X ID11:%02X",id4,id11);
        sprintf(LIN5,"ID5:%02X ID12:%02X",id5,id12);
        sprintf(LIN6,"ID6:%02X ID13:%02X",id6,id13);
        sprintf(LIN7,"ID7:%02X ID14:%02X",id7,id14);  
        sprintf(LIN8,"Command:%02X",command);
        sed15b1_update_console();

        delay_ms(150);
      }

    if (!KEY2)
      {
        command++;
        //        command&=0x7F;
        st7787_write_command(command);
        id1=st7787_read();
        id2=st7787_read();
        id3=st7787_read();
        id4=st7787_read();
        id5=st7787_read();
        id6=st7787_read();
        id7=st7787_read();
        id8=st7787_read();
        id9=st7787_read();
        id10=st7787_read();
        id11=st7787_read();
        id12=st7787_read();
        id13=st7787_read();
        id14=st7787_read();  
        sprintf(LIN1,"ID1:%02X ID8 :%02X",id1,id8);
        sprintf(LIN2,"ID2:%02X ID9 :%02X",id2,id9);
        sprintf(LIN3,"ID3:%02X ID10:%02X",id3,id10);
        sprintf(LIN4,"ID4:%02X ID11:%02X",id4,id11);
        sprintf(LIN5,"ID5:%02X ID12:%02X",id5,id12);
        sprintf(LIN6,"ID6:%02X ID13:%02X",id6,id13);
        sprintf(LIN7,"ID7:%02X ID14:%02X",id7,id14);  
        sprintf(LIN8,"Command:%02X",command);
        sed15b1_update_console();

        delay_ms(150);
      }





  }
}
