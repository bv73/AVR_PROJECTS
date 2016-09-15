// ILI9481 code for TFT8K2346 & TFT8K5029 LCMs
// 8-bit version (IM0=1 & IM1=1)
// (R)soft 16/4/2014
// Version 1.0


//=============================================================
// Запись команды в ЖКИ

void ili9481_wr_cmd (unsigned char byte)
{
  RS_0; // Режим команды

  PDATA=byte;

  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер

  RS_1; // возвращаем основной режим - режим записи данных
}

//=============================================================
// Запись данных в ЖКИ

void ili9481_wr_dat (unsigned char byte)
{
  PDATA=byte;

  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер
}

//--------------------------------------------
// Установка окна доступа в память дисплея

void ili9481_Window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{

//  if(x2>=ILI9481_WIDTH) { x2 = ILI9481_WIDTH-1; }
//  if(y2>=ILI9481_HEIGHT){ y2 = ILI9481_HEIGHT-1; }
  
#if (defined PORTRAIT) || (defined PORTRAIT_FLIP)
  ili9481_wr_cmd(ILI9481_COLUMN_ADDRESS_SET); // Set x
  ili9481_wr_dat((x1+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(x1+ILI9481_UNVISIBLE);
  ili9481_wr_dat((x2+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(x2+ILI9481_UNVISIBLE);

  ili9481_wr_cmd(ILI9481_PAGE_ADDRESS_SET);  // Set y
  ili9481_wr_dat(y1>>8);
  ili9481_wr_dat(y1);
  ili9481_wr_dat(y2>>8);
  ili9481_wr_dat(y2);
#else
  ili9481_wr_cmd(ILI9481_COLUMN_ADDRESS_SET); // Set x
  ili9481_wr_dat(x1>>8);
  ili9481_wr_dat(x1);
  ili9481_wr_dat(x2>>8);
  ili9481_wr_dat(x2);

  ili9481_wr_cmd(ILI9481_PAGE_ADDRESS_SET);  // Set y
  ili9481_wr_dat((y1+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(y1+ILI9481_UNVISIBLE);
  ili9481_wr_dat((y2+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(y2+ILI9481_UNVISIBLE);
#endif
  
}

//-----------------------------------------------------
// Очистка дисплея

void ili9481_clear(unsigned long color)
{
  unsigned int x,y;
  ili9481_Window(0,0,ILI9481_WIDTH-1,ILI9481_HEIGHT-1);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (x = ILI9481_PIXELX; x; x--) 
   {
       for (y = ILI9481_PIXELY; y; y--) 
       {
         ili9481_wr_dat((unsigned char)(color>>8));
         ili9481_wr_dat((unsigned char)color);
       }
   }

  ili9481_Window(0,0,ILI9481_WIDTH-1,ILI9481_HEIGHT-1);

}

//-----------------------------------------
// Установка точки 16 bit/pixel

void ili9481_putpixel16(unsigned int x, unsigned int y, unsigned int color) 
{ 
  ili9481_Window(x,y,x,y);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  ili9481_wr_dat((unsigned char)(color>>8));
  ili9481_wr_dat((unsigned char)color);     
} 


//=======================================
// Заполнение n строк одним цветом 16 bit/pixel
void ili9481_fill16 (unsigned int lines, unsigned int color)
{    
 unsigned int a,b;   

// N93_write_command(N93_MEMORY_WRITE);

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(ILI9481_WIDTH);b++)
         {
           ili9481_wr_dat((unsigned char)(color>>8));
           ili9481_wr_dat((unsigned char)color);
         }
   }
// N93_write_command(N93_NOP);
}

//-----------------------------
// Вывод радуги  в 16-битном цвете
// Расчет под PORTRAIT mode
void ili9481_rainbow16 (void)
{
  ili9481_Window(0,0,ILI9481_WIDTH-1,ILI9481_HEIGHT-1);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  ili9481_fill16(32,OLIVE_COLOR);
  ili9481_fill16(32,BLUE_COLOR);
  ili9481_fill16(32,PURPLE_COLOR);  
  ili9481_fill16(32,CYAN_COLOR);  
  ili9481_fill16(32,YELLOW_COLOR);
  ili9481_fill16(32,RED_COLOR);
  ili9481_fill16(32,GREEN_COLOR);
  ili9481_fill16(32,BLACK_COLOR);  
  ili9481_fill16(32,ORANGE_COLOR);
  ili9481_fill16(32,PINK_COLOR);
  ili9481_fill16(32,WHITE_COLOR);
  ili9481_fill16(64,NAVY_COLOR);    
  ili9481_fill16(64,YELLOW_COLOR);  
}

//-------------------------------------------------
// === Draw Line ===
// Draws a line in the specified color from (x0,y0) to (x1,y1) 
// Inputs: x = row address;
// y = column address;
// color = 16-bit color value

void ili9481_draw(unsigned int x0, unsigned int y0, unsigned int x1, 
                     unsigned int y1, unsigned int color) 
{ 
 int dy = y1 - y0; 
 int dx = x1 - x0; 
 int stepx, stepy; 

 if (dy < 0) 
   { dy = -dy; 
      stepy = -1; 
   } 
 else { stepy = 1; } 

 if (dx < 0) { dx = -dx; stepx = -1; } else { stepx = 1; } 
 dy <<= 1; // dy is now 2*dy 
 dx <<= 1; // dx is now 2*dx 
 
 ili9481_putpixel16(x0, y0, color); 

 if (dx > dy) 
  { 
   int fraction = dy - (dx >> 1); // same as 2*dy - dx 
   while (x0 != x1) 
   { 
     if (fraction >= 0) 
     { 
       y0 += stepy; 
       fraction -= dx; // same as fraction -= 2*dx 
     } 
     x0 += stepx; 
     fraction += dy; // same as fraction -= 2*dy 
     ili9481_putpixel16(x0, y0, color); 
    } 
   } 
 else 
  { 
   int fraction = dx - (dy >> 1); 
   while (y0 != y1) 
   { 
     if (fraction >= 0) 
     { 
       x0 += stepx; 
       fraction -= dy; 
     } 
     y0 += stepy; 
     fraction += dx; 
     ili9481_putpixel16(x0, y0, color); 
    } 
  } 
} 

//------------------------------------------------------------------
// === Set Rectangle ===
// Draws a rectangle in the specified color from (x1,y1) to (x2,y2) 
// Rectangle can be filled with a color if desired 
// Inputs: x = row address
// y = column address
// fill = 0=no fill, 1-fill entire rectangle 
// color = 16-bit color value
 
void ili9481_setrect(unsigned int x0, unsigned int y0, unsigned int x1, 
                     unsigned int y1, unsigned char fill, unsigned int color) 
{ 
 int xmin, xmax, ymin, ymax; 
 int i; 
 
 // Если 1, то прямоугольник закрашиваем
 if (fill == 1) 
 { 
  // best way to create a filled rectangle is to define a drawing box 
  // and loop two pixels at a time 
  // calculate the min and max for x and y directions 
  xmin = (x0 <= x1) ? x0 : x1; 
  xmax = (x0 > x1) ? x0 : x1; 
  ymin = (y0 <= y1) ? y0 : y1; 
  ymax = (y0 > y1) ? y0 : y1; 
 
  // specify the controller drawing box according to those limits 

  ili9481_Window(xmin,ymin,xmax,ymax);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);
 
  // loop on total number of pixels
  for (i = 0; i < ((xmax - xmin + 1) * (ymax - ymin + 1)); i++) 
    { 
      // use the color value to output three data bytes covering two pixels 
      ili9481_wr_dat((unsigned char)(color>>8));
      ili9481_wr_dat((unsigned char)color);
    } 
 
   } 
 else 
 { 
  // best way to draw un unfilled rectangle is to draw four lines 
  ili9481_draw(x0, y0, x1, y0, color); 
  ili9481_draw(x0, y1, x1, y1, color); 
  ili9481_draw(x0, y0, x0, y1, color); 
  ili9481_draw(x1, y0, x1, y1, color); 
 }
} 

//------------------------------------------------------------------
// === Circle ===
// Draws a line in the specified color at center (x0,y0) with radius 
// Inputs: x0 = row address
// y0 = column address
// radius = radius in pixels 
// color = 16-bit color value
// Author: Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// Note: taken verbatim Wikipedia article on Bresenham's line algorithm 
// http://www.wikipedia.org 
 
void ili9481_circle(unsigned int x0, unsigned int y0, 
                    unsigned char radius, unsigned int color) 
{ 
 int f = 1 - radius; 
 int ddF_x = 0; 
 int ddF_y = -2 * radius; 
 int x = 0; 
 int y = radius; 
 
 ili9481_putpixel16(x0, y0 + radius, color); 
 ili9481_putpixel16(x0, y0 - radius, color); 
 ili9481_putpixel16(x0 + radius, y0, color); 
 ili9481_putpixel16(x0 - radius, y0, color); 
 
 while(x < y) 
 { 
   if(f >= 0) 
   { 
     y--; 
     ddF_y += 2; 
     f += ddF_y; 
   } 
   x++; 
   ddF_x += 2; 
   f += ddF_x + 1; 
   ili9481_putpixel16(x0 + x, y0 + y, color); 
   ili9481_putpixel16(x0 - x, y0 + y, color); 
   ili9481_putpixel16(x0 + x, y0 - y, color); 
   ili9481_putpixel16(x0 - x, y0 - y, color); 
   ili9481_putpixel16(x0 + y, y0 + x, color); 
   ili9481_putpixel16(x0 - y, y0 + x, color); 
   ili9481_putpixel16(x0 + y, y0 - x, color); 
   ili9481_putpixel16(x0 - y, y0 - x, color); 
 } 
} 

//------------------------------------------------
// Печать символа
// Size: 0,1,2

void ili9481_putchar(unsigned char c, unsigned int x, unsigned int y, 
                     unsigned char size, unsigned int fColor, unsigned int bColor) 
{ 
    unsigned char i,j; 
    unsigned char nCols; 
    unsigned char nRows; 
    unsigned char nBytes; 
    unsigned char PixelRow; 
    unsigned char Mask; 
    unsigned int color; 
    unsigned char pChar[16]; 
    // get pointer to the beginning of the selected font table 
    // & 
    // get the nColumns, nRows and nBytes 
    switch(size) 
    { 
       case 0: 
          nCols = FONT6x8[0][0]; 
          nRows = FONT6x8[0][1]; 
          nBytes= FONT6x8[0][2]; 
          memcpy_P(pChar, FONT6x8[c - 0x1F], nBytes); 
       break; 
       
       case 1: 
          nCols = FONT8x8[0][0]; 
          nRows = FONT8x8[0][1]; 
          nBytes= FONT8x8[0][2]; 
          memcpy_P(pChar, FONT8x8[c - 0x1F], nBytes); 
       break; 
       
       case 2: 
          nCols = FONT8x16[0][0]; 
          nRows = FONT8x16[0][1]; 
          nBytes= FONT8x16[0][2]; 
          memcpy_P(pChar, FONT8x16[c - 0x1F], nBytes); 
       break; 
    } 

  ili9481_Window(x,y,(x + nCols - 1),(y + nRows -1));
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);


    // loop on each row, working backwards from the bottom to the top 
    for (i=0; i < nRows; i++) //for (i = nRows - 1; i != 255; i--) 
    { 
       // copy pixel row from font table and then decrement row 
       PixelRow = pChar[i]; //PixelRow = *pChar--; 
       // loop on each pixel in the row (left to right) 
       // Note: we do two pixels each loop 
       Mask = 0x80; 
       for (j = 0; j < nCols; j++) 
       { 
          // if pixel bit set, use foreground color; else use the background color 
          // now get the pixel color for two successive pixels 
          if ((PixelRow & Mask) == 0) 
             color = bColor; 
          else 
             color = fColor; 
          Mask = Mask >> 1; 
          // use this information to output three data bytes 
          ili9481_wr_dat((unsigned char)(color>>8));
          ili9481_wr_dat((unsigned char)color);
       } 
    } 
    // terminate the Write Memory command 
    ili9481_wr_cmd(ILI9481_NOP); 

} 


//-------------------------------------------------------------
//

void ili9481_putstr(char *pString, unsigned int x, unsigned int y, 
                    unsigned char Size, unsigned int fColor, unsigned int bColor) 
{ 
    // loop until null-terminator is seen 
    while (*pString != '\0') 
    { 
       // draw the character 
       ili9481_putchar(*pString++, x, y, Size, fColor, bColor); 
       // advance the y position 
       if (Size == 0) // if SMALL
          x = x + 6; 
       else 
          x = x + 8; 
       // bail out if y exceeds 131 
       if (y > 131) break; 
    } 
} 


//------------------------------------
// Initialize ILI9481
// Especially Specified for TFT8K2346 LCD

void ili9481_init(void)
{
  DDR_DATA=0xFF;
  
  DDR_RDX=1;
  DDR_RESX=1;
  DDR_WRX=1;
  DDR_RS=1;
  DDR_CSX=1;

  
  RDX_1;
  WRX_1;
  RS_1; // RS держим всегда 1
  CSX_0; // CSX всегда ноль
  
  RESX_0;  // Hardware Reset LCD
  delay_ms(2);
  RESX_1;  // Return Reset OFF
  delay_ms(150);
  
  ili9481_wr_cmd(ILI9481_SLEEP_OUT); // Exit Sleep Mode 0x11
  delay_ms(100);  // Wait Stability
		
  ili9481_wr_cmd(ILI9481_NORMAL_DISPLAY_MODE_ON); // Entering Nomal Displaymode 0x13

  ili9481_wr_cmd(ILI9481_VCOM_CONTROL); // VCOM Control
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x3F); // Значения подбирать опытным путем до естественной цветопередачи
  ili9481_wr_dat(0x0B);
  
  
/*  
  ili9481_wr_cmd(ILI9481_POWER_SETTING); // LCD Power Settings
  ili9481_wr_dat(0x07); // 1.0xVci
  ili9481_wr_dat(0x40); // 40 41
  ili9481_wr_dat(0x1C); // 1c 1e

  ili9481_wr_cmd(ILI9481_POWER_SET_FOR_NOMAL_MODE); // Power_Setting for Normal Mode
  ili9481_wr_dat(0x01);
  ili9481_wr_dat(0x11); // 02 11

  ili9481_wr_cmd(ILI9481_PANEL_DRIVING_SETTINGS); // Panel Driving Settings
  ili9481_wr_dat(0x00); // установить сканирование режим
  ili9481_wr_dat(0x3B);// Установить количество линий 480
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x02); // 5frames
  ili9481_wr_dat(0x11);

  ili9481_wr_cmd(ILI9481_DISPLAY_TIMING_SET_FOR_NORMAL); // Display_Timing_Setting for Normal Mode
  ili9481_wr_dat(0x10);
  ili9481_wr_dat(0x0B); 
  ili9481_wr_dat(0x88);

  ili9481_wr_cmd(ILI9481_FRAME_RATE_AND_INVERSION_CTRL); // Frame Rate and Inversion Control
  ili9481_wr_dat(0x01); // 100 Гц
*/
  
  

  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL); // Set_address_mode 0x36
// B7         B6         B5              B4                B3       B2              B1           B0
// направл Y, направл X, выбор X или Y, Vertical направл, RGB/BGR=1, DisplDataLatch=0, Horiz Flip,  Vert Flip

  ili9481_wr_dat(0x08|LCD_ORIENTATION); // BGR-order (0x88)

  ili9481_wr_cmd(ILI9481_INTERFACE_PIXEL_FORMAT); // Set_pixel_format 0x3A
  ili9481_wr_dat(0x55); // RGB565(16dpp) 16-битный цвет


  ili9481_wr_cmd(ILI9481_DISPLAY_ON); // Display ON

}


//------------------------------------------
//
/*
unsigned int read_lcd_id (void)
{
  // Check Device Code
//  devicetype = ILI9481_rd_cmd(ILI9481_DEVICE_CODE_READ); // Confirm Vaild LCD Controller
  unsigned int lcdid;
  LCD_WR_REG (0xBF);
  LCD_RD_DATA ();         // Dummy Read
  LCD_RD_DATA ();         // читать 0x01
  LCD_RD_DATA ();         // читать 0xD0
  lcdid = LCD_RD_DATA (); // прочитать 0x94
  lcdid << = 8 ;
  lcdid | = LCD_RD_DATA (); // читать 0x81
  return (lcdid);
}
*/

//--------------------------------------------------
// Установка (& подстройка) контрастности и цветопередачи
void vcom_set(unsigned char vcm, unsigned char vdv)
{
  ili9481_wr_cmd(ILI9481_VCOM_CONTROL); // VCOM Control
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(vcm); // (0x3F по умолчанию)
  ili9481_wr_dat(vdv); // (0x0B по умолчанию)
}

