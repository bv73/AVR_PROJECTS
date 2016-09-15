#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include <string.h> // for memset & memcpy
#include <pgmspace.h> // for memcpy_P

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
#include "hx8347a\j01.h"
#include "hx8347a\j02.h"
#include "hx8347a\j03.h"
#include "hx8347a\j04.h"
#include "hx8347a\j05.h"
#include "hx8347a\j07.h"
#include "hx8347a\j08.h"
#include "hx8347a\j09.h"
#include "hx8347a\j10.h"
#include "hx8347a\j11.h"
#include "hx8347a\j12.h"
#include "hx8347a\j13.h"
#include "hx8347a\j14.h"
#include "hx8347a\j15.h"
#include "hx8347a\j16.h"
#include "hx8347a\j17.h"
#include "hx8347a\j18.h"
#include "hx8347a\j19.h"
#include "hx8347a\j20.h"
#include "hx8347a\j21.h"
#include "hx8347a\j22.h"
#include "hx8347a\j23.h"
#include "hx8347a\j24.h"
#include "hx8347a\j25.h"
#include "hx8347a\jpeg.c"

#include "font\font5x7.h"

#include "sed15b1_3WIRE\sed15b1.h"
#include "sed15b1_3WIRE\sed15b1.c"


void main( void )
{
  unsigned char value=0x4D;

  hx8347a_init();
//  hx8347a_clr_screen(WHITE_COLOR);
//  hx8347a_set_xy(0,0);
  hx8347a_fill(32,WHITE_COLOR);
  hx8347a_fill(32,BLUE_COLOR);
  hx8347a_fill(32,PURPLE_COLOR);  
  hx8347a_fill(32,LIGHTBLUE_COLOR);    
  hx8347a_fill(32,YELLOW_COLOR);    
  hx8347a_fill(32,RED_COLOR);
  hx8347a_fill(32,GREEN_COLOR);
  hx8347a_fill(32,BLACK_COLOR);  
  hx8347a_fill(32,BLUE_COLOR);
  hx8347a_fill(32,PURPLE_COLOR);  

  //**************************************************************  
//  sed15b1_init();
//  sed15b1_clear_console(0x20);
  
    //  sprintf(LIN1,"Addr:%02X",adr);
    //  sprintf(LIN2,"Width:%d",width);
    //  sprintf(LIN3,"Height:%d",height);
    //  sprintf(LIN4,"Offset:0x%02X",main_offset);
    //  sprintf(LIN5,"Size:%lu",file_size);

//  sprintf(LIN8,"Contrast:%02X",value);  
//  sed15b1_update_console();

//  hx8347a_bmp_output(width,height,bmp_file.data_BGR);
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

//----------- JPEG DECODING ----------------------  1
  JDEC jd;		// TJDEC decompression object
  IODEV iodev;	// Identifier of the decompression session (depends on application)
  JRESULT rc;		// Result
  while (1)
  {
  iodev.jpic = MARINA01;
  iodev.jsize = sizeof(MARINA01);
  iodev.joffset = 0;

  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_set_xy (0,0);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 2
  iodev.jpic = MARINA02;
  iodev.jsize = sizeof(MARINA02);
  iodev.joffset = 0;
  jpeg_type=VERTICAL;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 3
  iodev.jpic = MARINA03;
  iodev.jsize = sizeof(MARINA03);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 4
  iodev.jpic = MARINA04;
  iodev.jsize = sizeof(MARINA04);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 5
  iodev.jpic = MARINA05;
  iodev.jsize = sizeof(MARINA05);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 7
  iodev.jpic = MARINA07;
  iodev.jsize = sizeof(MARINA07);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 8
  iodev.jpic = MARINA08;
  iodev.jsize = sizeof(MARINA08);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 9
  iodev.jpic = MARINA09;
  iodev.jsize = sizeof(MARINA09);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 10
  iodev.jpic = MARINA10;
  iodev.jsize = sizeof(MARINA10);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 11
  iodev.jpic = MARINA11;
  iodev.jsize = sizeof(MARINA11);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 12
  iodev.jpic = MARINA12;
  iodev.jsize = sizeof(MARINA12);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 13
  iodev.jpic = MARINA13;
  iodev.jsize = sizeof(MARINA13);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  

//---------------------------------------------------- 14
  iodev.jpic = MARINA14;
  iodev.jsize = sizeof(MARINA14);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  

//---------------------------------------------------- 15
  iodev.jpic = MARINA15;
  iodev.jsize = sizeof(MARINA15);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 16
  iodev.jpic = MARINA16;
  iodev.jsize = sizeof(MARINA16);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 17
  iodev.jpic = MARINA17;
  iodev.jsize = sizeof(MARINA17);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 18
  iodev.jpic = MARINA18;
  iodev.jsize = sizeof(MARINA18);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 19
  iodev.jpic = MARINA19;
  iodev.jsize = sizeof(MARINA19);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 20
  iodev.jpic = MARINA20;
  iodev.jsize = sizeof(MARINA20);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 21
  iodev.jpic = MARINA21;
  iodev.jsize = sizeof(MARINA21);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 22
  iodev.jpic = MARINA22;
  iodev.jsize = sizeof(MARINA22);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 23
  iodev.jpic = MARINA23;
  iodev.jsize = sizeof(MARINA23);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 24
  iodev.jpic = MARINA24;
  iodev.jsize = sizeof(MARINA24);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//---------------------------------------------------- 25
  iodev.jpic = MARINA25;
  iodev.jsize = sizeof(MARINA25);
  iodev.joffset = 0;
  
  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);

  delay_ms(5000);
  hx8347a_clr_screen(WHITE_COLOR);  
//----------------------------------------------------

  }
}
/*
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


*/