#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h>      // for sprintf
#include <pgmspace.h>   // for memcpy_P

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

#include "font\font5x7.h" // font for debug display
//#include "font\arial16.h"

#include "N1202\pcf8814.h" // debug display
#include "N1202\pcf8814.c"

#include "ssd1963\fonts.h"
#include "ssd1963\ssd1963_8bit.h"
#include "ssd1963\ssd1963_8bit.c"


void main( void )
{
 
  pcf8814_init(); // debug display init

//  delay_ms(5000);
  
  pcf8814_clear_console(0x20);

  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  sprintf(LN1,"Hello, World!");
  pcf8814_update_console();


  ssd1963_init();
    
  ssd1963_rainbow();

  delay_ms(400);
  ssd1963_clear(BLACK_COLOR);

  ssd1963_putpixel(3,3,WHITE_COLOR);
  ssd1963_setrect(100,50,200,150,1,GREEN_COLOR);
  ssd1963_setrect(200,250,100,200,0,WHITE_COLOR);

  ssd1963_setrect(0,0,271,479,0,WHITE_COLOR);

  ssd1963_draw(200,10,20,240,YELLOW_COLOR);
  ssd1963_draw(210,10,30,240,BLUE_COLOR);
  ssd1963_circle(100,150,85,RED_COLOR);

  ssd1963_setpos(172,46);
  ssd1963_setcolor(PINK_COLOR, CYAN_COLOR);
  fontsize=BIG_FONT;
  ssd1963_putstr("TFT");

  ssd1963_setpos(172,62);
  bColor= TRANSPARENT;
  ssd1963_putstr("LCD 272x480");
  
  ssd1963_setpos(110,46);
  ssd1963_setcolor(RED_COLOR, ORANGE_COLOR);
  fontsize=MEDIUM_FONT;
  ssd1963_putstr("Colour");
  
  ssd1963_setpos(55,86);
  ssd1963_setcolor(BLUE_COLOR,TRANSPARENT);
  fontsize=BIG_FONT;
  ssd1963_putstr("Temperature");

  ssd1963_setpos(102,135);
  fColor = RED_COLOR;
  ssd1963_putstr("Transparent");

  ssd1963_setpos(110,155);
  fColor = CYAN_COLOR;
  ssd1963_putstr("Background");

  ssd1963_setpos(12,24);
  fColor=WHITE_COLOR;
  fontsize=SMALL_FONT;
  ssd1963_putstr("SSD1963 LCD TFT CONTROLLER");

  y=260;
  x=0;
  fontsize = MEDIUM_FONT;
  ssd1963_putstr("ITDB02-4.3 LCD\nBy (R)soft 2015");

  fontsize = BIG_FONT;
  fColor = RED_COLOR;
  ssd1963_putstr("\nmicroSD with Fat32 support.\nJPEG & BMP & & PNG & ZX screens\nfiles output.\n");
  fColor = GREEN_COLOR;  
  ssd1963_putstr("Touchscreen x2046 support.\n\n");
  fColor = YELLOW_COLOR;
  ssd1963_putstr("Touch screen to Continue...");

  unsigned char contrast = 0x47;
  unsigned char brightness = 0x94; // 93
  unsigned char saturation = 0x5D; // 58 (max 0x7F)
  
  while (1)
  {
    if (!KEY1)
      {
        contrast--;
        sprintf(LN7,"Contrast:%02X", contrast);
        ssd1963_wr_cmd(SSD1963_SET_POST_PROC); // Set contrast/brightness (0xBC)
        ssd1963_wr_dat(contrast); // Set contrast value
        ssd1963_wr_dat(brightness); // Set brightness value
        ssd1963_wr_dat(saturation); // Set saturation value
        ssd1963_wr_dat(0x01); // Enable postprocessor

        pcf8814_update_console();
        delay_ms(100);
      }

    if (!KEY2)
      {
        contrast++;
        sprintf(LN7,"Contrast:%02X", contrast);
        ssd1963_wr_cmd(SSD1963_SET_POST_PROC); // Set contrast/brightness (0xBC)
        ssd1963_wr_dat(contrast); // Set contrast value
        ssd1963_wr_dat(brightness); // Set brightness value
        ssd1963_wr_dat(saturation); // Set saturation value
        ssd1963_wr_dat(0x01); // Enable postprocessor

        pcf8814_update_console();
        delay_ms(100);
      }

    if (!KEY3)
      {
        brightness--;
        sprintf(LN8,"Brightness:%02X", brightness);
        ssd1963_wr_cmd(SSD1963_SET_POST_PROC); // Set contrast/brightness (0xBC)
        ssd1963_wr_dat(contrast); // Set contrast value
        ssd1963_wr_dat(brightness); // Set brightness value
        ssd1963_wr_dat(saturation); // Set saturation value
        ssd1963_wr_dat(0x01); // Enable postprocessor

        pcf8814_update_console();
        delay_ms(100);
      }

    if (!KEY4)
      {
        brightness++;
        sprintf(LN8,"Brightness:%02X", brightness);
        ssd1963_wr_cmd(SSD1963_SET_POST_PROC); // Set contrast/brightness (0xBC)
        ssd1963_wr_dat(contrast); // Set contrast value
        ssd1963_wr_dat(brightness); // Set brightness value
        ssd1963_wr_dat(saturation); // Set saturation value
        ssd1963_wr_dat(0x01); // Enable postprocessor

        pcf8814_update_console();
        delay_ms(100);
      }

  }
}
