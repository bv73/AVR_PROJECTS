// MC2PA8201 Routine
// ********************************************************************
// Writen by (R)soft 12.11.2013 - 15.12.2013
// Version 1.0
// Testing on Mega2561
// Testing on Nokia N93 LCD Module. Resolution 320x240 pixels. 
// Built-in MC2PA8201 controller with parallel 8-bit interface.
// Reference manual for controller - MC2PA8201.pdf
// ********************************************************************
// Прекрасно работает без delay
// CSX - всегда ноль
//=============================================================
// Запись команды в ЖКИ

void N93_write_command (unsigned char byte)
{
  DCX_0; // Режим команды

  PDATA=byte; // выставляем байт на шину
  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер

  DCX_1; // возвращаем основной режим - режим записи данных
}


//=============================================================
// Запись байта данных в ЖКИ

void N93_write_data (unsigned char byte)
{
  PDATA=byte; // выставляем байт на шину
  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер
}

//=============================================================
// Чтение байта из ЖКИ
// DCX при чтении всегда 1
// Эта функция писалась исключительно для отладки кода и
// отображения отклика MC2PA8201 - чтение регистров.

unsigned char N93_read_data(void)
{
  unsigned char data;

  DDR_DAT=0x00; // шину данных на ввод
  PDATA=0x00; // подтяжка

  RDX_0;  // RDX=0 - Read mode  Строб чтения - данные выставились

  delay_us(15); // Задержка обязательна! Иначе не успевают зафиксироваться данные на шине

  // Читаем с шины данных байт
  data=PIN_DAT;

  RDX_1;  // Возврат RDX в исходное состояние
  DDR_DAT=0xFF; // возврат шины данных на вывод

  return (data);
}


//--------------------------------------------
// Установка окна доступа в память дисплея

void N93_Window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{

  if(x2>=N93_WIDTH) { x2 = N93_WIDTH-1; }
  if(y2>=N93_HEIGHT){ y2 = N93_HEIGHT-1; }

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(x1>>8);
  N93_write_data(x1);
  N93_write_data(x2>>8);
  N93_write_data(x2);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(y1>>8);
  N93_write_data(y1);
  N93_write_data(y2>>8);
  N93_write_data(y2);
}


//-----------------------------------------------------
// Очистка дисплея

void N93_Clear(unsigned long color)
{
  unsigned int x,y;
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1); // При задании окна важна ориентация ЖКИ
  N93_write_command(N93_MEMORY_WRITE);

  for (x = N93_PIXELX; x; x--) 
   {
       for (y = N93_PIXELY; y; y--) 
       {
         N93_write_data((unsigned char)(color>>16));  // RED
         N93_write_data((unsigned char)(color>>8));  // GREEN
         N93_write_data((unsigned char)(color)); // BLUE
       }
   }
  N93_write_command(N93_NOP);
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1);
    
}

//=============================================================
// Инициализация LCD и выводов порта данных/управления

void N93_init ()   
{        
  DDR_RESX=1; // Инициализация всех используемых ног
  DDR_DCX=1;
  DDR_RDX=1;
  DDR_WRX=1;
  DDR_DAT=0xFF;
  
  RDX_1;  // Исходное состояние Read=1
  WRX_1;  // Исходное состояние Write=1
  DCX_1; // Исходно DCX всегда 1 (запись данных)
  
  RESX_0;  // Hardware Reset LCD
  delay_ms(2);
  RESX_1;  // Return Reset OFF
  delay_ms(150);

  N93_write_command(N93_SOFTWARE_RESET); // Soft Reset
  delay_ms(150);

  N93_write_command(N93_SLEEP_OUT);
  delay_ms(50);

  N93_write_command(N93_DISPLAY_INVERSION_OFF);
  N93_write_command(N93_IDLE_MODE_OFF);
  N93_write_command(N93_NORMAL_DISPLAY_MODE_ON);

  
  N93_write_command(N93_INTERFACE_PIXEL_FORMAT);
  N93_write_data(0x05); // 07 - 24bit Color, 05 - 16bit Color

  N93_write_command(N93_MEMORY_ACCESS_CONTROL);
  // D7         D6         D5              D4        D3       D2        D1  D0
  // направл Y, направл X, поменять X & Y, RefreshY, RGB/BGR, RefreshX, x,  x
  N93_write_data(LCD_ORIENTATION);

  delay_ms(125);
  N93_write_command(N93_DISPLAY_ON);
  N93_write_command(N93_NOP);

  N93_Clear(WHITE);

}   

//-----------------------------------------
// Установка точки 24 bit/pixel

void N93_putpixel24(unsigned int x, unsigned int y, unsigned long color) 
{ 
  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data(x>>8);
  N93_write_data(x);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data(y>>8);
  N93_write_data(y);

  N93_write_command(N93_MEMORY_WRITE);

  N93_write_data((unsigned char)(color>>16));  // RED
  N93_write_data((unsigned char)(color>>8));  // GREEN
  N93_write_data((unsigned char)(color)); // BLUE

  N93_write_command(N93_NOP);
     
} 

//-----------------------------------------
// Установка точки 16 bit/pixel

void N93_putpixel16(unsigned int x, unsigned int y, unsigned int color) 
{ 
  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data(x>>8);
  N93_write_data(x);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data(y>>8);
  N93_write_data(y);

  N93_write_command(N93_MEMORY_WRITE);

  N93_write_data((unsigned char)(color>>8));
  N93_write_data((unsigned char)(color));

  N93_write_command(N93_NOP);
     
} 

//=======================================
// Заполнение n строк одним цветом 24 bit/pixel
void N93_fill24 (unsigned int lines, unsigned long color)
{    
 unsigned int a,b;   

// N93_write_command(N93_MEMORY_WRITE);

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(N93_PIXELX);b++)
         {
            N93_write_data((unsigned char)(color>>16));  // RED
            N93_write_data((unsigned char)(color>>8));  // GREEN
            N93_write_data((unsigned char)(color)); // BLUE
         }
   }
// N93_write_command(N93_NOP);
}

//=======================================
// Заполнение n строк одним цветом 16 bit/pixel
void N93_fill16 (unsigned int lines, unsigned int color)
{    
 unsigned int a,b;   

// N93_write_command(N93_MEMORY_WRITE);

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(N93_PIXELX);b++)
         {
            N93_write_data((unsigned char)(color>>8));
            N93_write_data((unsigned char)(color));
         }
   }
// N93_write_command(N93_NOP);
}


//-----------------------------
// Вывод радуги  в 24-битном цвете
void N93_rainbow24 (void)
{
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1);
  N93_write_command(N93_MEMORY_WRITE);
  N93_fill24(32,DEEPPINK);
  N93_fill24(32,BLUE);
  N93_fill24(32,PURPLE);  
  N93_fill24(32,BLUEVIOLET);    
  N93_fill24(32,YELLOW);    
  N93_fill24(32,RED);
  N93_fill24(32,GREEN);
  N93_fill24(32,BLACK);  
  N93_fill24(32,ORANGE);
  N93_fill24(32,CYAN);  
}


//-----------------------------
// Вывод радуги  в 16-битном цвете
void N93_rainbow16 (void)
{
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1);
  N93_write_command(N93_MEMORY_WRITE);
  N93_fill16(32,OLIVE_COLOR);
  N93_fill16(32,BLUE_COLOR);
  N93_fill16(32,PURPLE_COLOR);  
  N93_fill16(32,CYAN_COLOR);  
  N93_fill16(32,YELLOW_COLOR);    
  N93_fill16(32,RED_COLOR);
  N93_fill16(32,GREEN_COLOR);
  N93_fill16(32,BLACK_COLOR);  
  N93_fill16(32,ORANGE_COLOR);
  N93_fill16(32,PINK_COLOR);    
}

//-------------------------------------------------
// LCDSetLine.c 
// Draws a line in the specified color from (x0,y0) to (x1,y1) 
// Inputs: x = row address (0 .. 320) 
// y = column address (0 .. 320) 
// color = 16-bit color value

void N93_draw(unsigned int x0, unsigned int y0, unsigned int x1, 
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
 
 N93_putpixel16(x0, y0, color); 

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
     N93_putpixel16(x0, y0, color); 
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
     N93_putpixel16(x0, y0, color); 
    } 
  } 
} 

//------------------------------------------------------------------
// LCDSetRect.c 
// Draws a rectangle in the specified color from (x1,y1) to (x2,y2) 
// Rectangle can be filled with a color if desired 
// 
// Inputs: x = row address (0 .. 320) 
// y = column address (0 .. 320) 
// fill = 0=no fill, 1-fill entire rectangle 
// color = 16-bit color value
 
void N93_setrect(unsigned int x0, unsigned int y0, unsigned int x1, 
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
  // Row address set (command 0x2B) 
  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(xmin>>8);
  N93_write_data(xmin);
  N93_write_data(xmax>>8);
  N93_write_data(xmax);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(ymin>>8);
  N93_write_data(ymin);
  N93_write_data(ymax>>8);
  N93_write_data(ymax);

  N93_write_command(N93_MEMORY_WRITE);
 
  // loop on total number of pixels / 2 
  for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 130); i++) 
    { 
      // use the color value to output three data bytes covering two pixels 
      N93_write_data((unsigned char)(color>>8));
      N93_write_data((unsigned char)(color));
    } 
 
   } 
 else 
 { 
  // best way to draw un unfilled rectangle is to draw four lines 
  N93_draw(x0, y0, x1, y0, color); 
  N93_draw(x0, y1, x1, y1, color); 
  N93_draw(x0, y0, x0, y1, color); 
  N93_draw(x1, y0, x1, y1, color); 
 }
} 

// ************************************************************************************* 
// LCDSetCircle.c 
// 
// Draws a line in the specified color at center (x0,y0) with radius 
// 
// Inputs: x0 = row address (0 .. 131) 
// y0 = column address (0 .. 131) 
// radius = radius in pixels 
// color = 12-bit color value rrrrggggbbbb 
// 
// Returns: nothing 
// 
// Author: Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// 
// Note: taken verbatim Wikipedia article on Bresenham's line algorithm 
// http://www.wikipedia.org 
// 
// ************************************************************************************* 
 
void N93_circle(unsigned int x0, unsigned int y0, 
                    unsigned char radius, unsigned int color) 
{ 
 int f = 1 - radius; 
 int ddF_x = 0; 
 int ddF_y = -2 * radius; 
 int x = 0; 
 int y = radius; 
 
 N93_putpixel16(x0, y0 + radius, color); 
 N93_putpixel16(x0, y0 - radius, color); 
 N93_putpixel16(x0 + radius, y0, color); 
 N93_putpixel16(x0 - radius, y0, color); 
 
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
   N93_putpixel16(x0 + x, y0 + y, color); 
   N93_putpixel16(x0 - x, y0 + y, color); 
   N93_putpixel16(x0 + x, y0 - y, color); 
   N93_putpixel16(x0 - x, y0 - y, color); 
   N93_putpixel16(x0 + y, y0 + x, color); 
   N93_putpixel16(x0 - y, y0 + x, color); 
   N93_putpixel16(x0 + y, y0 - x, color); 
   N93_putpixel16(x0 - y, y0 - x, color); 
 } 
} 


//------------------------------------------------
// Печать символа
// Size: 0,1,2

void N93_putchar(unsigned char c, unsigned int x, unsigned int y, 
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
    // Page (Row) address set
    N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
    N93_write_data(y>>8);
    N93_write_data(y);
    N93_write_data((y + nRows -1)>>8);
    N93_write_data(y + nRows -1);

    // Column address set
    N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
    N93_write_data(x>>8);
    N93_write_data(x);
    N93_write_data((x + nCols - 1)>>8);
    N93_write_data(x + nCols - 1);

    // WRITE MEMORY 
    N93_write_command(N93_MEMORY_WRITE);

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
          N93_write_data(color); 
       } 
    } 
    // terminate the Write Memory command 
    N93_write_command(N93_NOP); 

} 


//-------------------------------------------------------------
//

void N93_putstr(char *pString, unsigned int x, unsigned int y, 
                    unsigned char Size, unsigned int fColor, unsigned int bColor) 
{ 
    // loop until null-terminator is seen 
    while (*pString != '\0') 
    { 
       // draw the character 
       N93_putchar(*pString++, x, y, Size, fColor, bColor); 
       // advance the y position 
       if (Size == 0) // if SMALL
          x = x + 6; 
       else 
          x = x + 8; 
       // bail out if y exceeds 131 
       if (y > 131) break; 
    } 
} 

void print_command (unsigned char command)
{
  unsigned char id1,id2,id3,id4,id5;
  N93_write_command(command);
  id1=N93_read_data();
  id2=N93_read_data();
  id3=N93_read_data();
  id4=N93_read_data();
  id5=N93_read_data();
  sprintf(LN1,"ID1=%02X",id1);  // FF 26 
  sprintf(LN2,"ID2=%02X",id2);  // 83 83
  sprintf(LN3,"ID3=%02X",id3);  // 8A 8A
  sprintf(LN4,"ID4=%02X",id4);  // 27 27
  sprintf(LN5,"ID5=%02X",id5);  // 00 00
  //  pcf8814_putchar16(0x30);
  pcf8814_update_console();
}
