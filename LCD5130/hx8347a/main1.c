#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include <string.h> // for memset & memcpy

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

#include "hx8347a\hx8347a.h"
#include "hx8347a\hx8347a.c"

#include "..\TJpgDec\tjpgd.h"
#include "..\TJpgDec\integer.h"
#include "..\TJpgDec\tjpgd.c"
#include "hx8347a\jpeg1.h"
#include "hx8347a\jpeg2.h"
#include "hx8347a\jpeg3.h"
#include "hx8347a\jpeg4.h"
#include "hx8347a\jpeg.c"

#include "font\font5x7.h"

#include "sed15b1_3WIRE\sed15b1.h"
#include "sed15b1_3WIRE\sed15b1.c"


void main( void )
{
//  unsigned int type = bmp_file.bfh.bfType;
//  unsigned int width = bmp_file.bih.biWidth; // Ширина картинки
//  unsigned int height = bmp_file.bih.biHeight;  // Высота картинки
//  unsigned long main_offset = bmp_file.bfh.bfOffBits; // отступ от начала файла где лежат данные RGB
//  unsigned long file_size = bmp_file.bfh.bfSize;  // размер файла
  unsigned char value=0x4D;

  hx8347a_init();
  hx8347a_clr_screen(WHITE_COLOR);
//  hx8347a_setxy(100,240,0,320);
//  hx8347a_fill(32,WHITE_COLOR);
//  hx8347a_fill(32,BLUE_COLOR);
//  hx8347a_fill(32,PURPLE_COLOR);  
//  hx8347a_fill(32,LIGHTBLUE_COLOR);    
//  hx8347a_fill(32,YELLOW_COLOR);    
//  hx8347a_fill(32,RED_COLOR);
//  hx8347a_fill(32,GREEN_COLOR);
//  hx8347a_fill(32,BLACK_COLOR);  

  //**************************************************************  
  sed15b1_init();
  sed15b1_clear_console(0x20);
  
//  sprintf(LIN1,"Addr:%02X",adr);
//  sprintf(LIN2,"Width:%d",width);
//  sprintf(LIN3,"Height:%d",height);
//  sprintf(LIN4,"Offset:0x%02X",main_offset);
//  sprintf(LIN5,"Size:%lu",file_size);
  sprintf(LIN8,"Contrast:%02X",value);  
  sed15b1_update_console();

//  hx8347a_bmp_output(width,height,bmp_file.data_BGR);
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

//----------- JPEG DECODING ----------------------  
  JDEC jd;		// TJDEC decompression object
  IODEV iodev;	// Identifier of the decompression session (depends on application)
  JRESULT rc;		// Result
  
  iodev.jpic = MARINA1;
  iodev.jsize = sizeof(MARINA1);
  iodev.joffset = 0;

  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//----------------------------------------------------
  iodev.jpic = MARINA2;
  iodev.jsize = sizeof(MARINA2);
  iodev.joffset = 0;
  jpeg_type=HORIZONTAL;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//----------------------------------------------------
  iodev.jpic = MARINA3;
  iodev.jsize = sizeof(MARINA3);
  iodev.joffset = 0;
  jpeg_type=VERTICAL;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//----------------------------------------------------
  iodev.jpic = MARINA4;
  iodev.jsize = sizeof(MARINA4);
  iodev.joffset = 0;
  jpeg_type=VERTICAL;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//----------------------------------------------------
  
  while (1)
  {

  if (!KEY1)
      {
        value--;
        value&=0x7F;
        hx8347a_set_register(HX8347A_VCOM_CTRL2, value);
        sprintf(LIN8,"Contrast:%02X",value);
        sed15b1_update_console();

        delay_ms(150);
      }

    if (!KEY2)
      {
        value++;
        value&=0x7F;
        hx8347a_set_register(HX8347A_VCOM_CTRL2, value);
        sprintf(LIN8,"Contrast:%02X",value);
        sed15b1_update_console();
        delay_ms(150);
      }

  }
}
