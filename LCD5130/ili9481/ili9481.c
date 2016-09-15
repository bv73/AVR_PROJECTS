// ILI9481 code for TFT8K2346 & TFT8K5029 LCMs
// (R)soft 20/2/2014
// Version 1.0


//=============================================================
// Запись команды в ЖКИ

void ili9481_wr_cmd (unsigned int word)
{
  RS_0; // Режим команды

  // Выставление на шину данных 16-битного слова
  DATA_HI=(unsigned char)((word>>8)&0xFF); // Выделяем старший байт и инвертируем  
  DATA_LO=(unsigned char)((word)&0xFF); // Выделяем младший байт и инвертируем

  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер

  RS_1; // возвращаем основной режим - режим записи данных
}

//=============================================================
// Запись данных в ЖКИ

void ili9481_wr_dat (unsigned int word)
{
  // Выставление на шину данных 16-битного слова
  DATA_HI=(unsigned char)((word>>8)&0xFF); // Выделяем старший байт и инвертируем  
  DATA_LO=(unsigned char)((word)&0xFF); // Выделяем младший байт и инвертируем

  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер
}

//--------------------------------------------
// Установка окна доступа в память дисплея

void ili9481_Window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{

  if(x2>=ILI9481_WIDTH) { x2 = ILI9481_WIDTH-1; }
  if(y2>=ILI9481_HEIGHT){ y2 = ILI9481_HEIGHT-1; }

  ili9481_wr_cmd(ILI9481_COLUMN_ADDRESS_SET); // Set x
  ili9481_wr_dat(x1>>8);
  ili9481_wr_dat(x1);
  ili9481_wr_dat(x2>>8);
  ili9481_wr_dat(x2);

  ili9481_wr_cmd(ILI9481_PAGE_ADDRESS_SET);  // Set y
  ili9481_wr_dat(y1>>8);
  ili9481_wr_dat(y1);
  ili9481_wr_dat(y2>>8);
  ili9481_wr_dat(y2);
}

//-----------------------------------------------------
// Очистка дисплея

void ili9481_clear(unsigned long color)
{
  unsigned int x,y;
  ili9481_Window(0,0,ILI9481_WIDTH-1,ILI9481_HEIGHT-1); // При задании окна важна ориентация ЖКИ
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (x = ILI9481_PIXELX; x; x--) 
   {
       for (y = ILI9481_PIXELY; y; y--) 
       {
         ili9481_wr_dat(color);
       }
   }
//  N93_write_command(N93_NOP);
  ili9481_Window(0,0,ILI9481_WIDTH-1,ILI9481_HEIGHT-1);

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
            ili9481_wr_dat(color);
         }
   }
// N93_write_command(N93_NOP);
}

//-----------------------------
// Вывод радуги  в 16-битном цвете
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



//------------------------------------
// Initialize ILI9481

void ili9481_init(void)
{
  DDR_DATL=0xFF;
  DDR_DATH=0xFF;
  
  DDR_D17=1;
  DDR_D16=1;
  DDR_RDX=1;
  DDR_RESX=1;
  DDR_WRX=1;
  DDR_RS=1;
  DDR_CSX=1;
  DDR_BL=1;
  
  RDX_1;
  WRX_1;
  RS_1; // RS держим всегда 1
  CSX_0;
  BL_ON;
  D17_1;
  D16_1;
  
  RESX_0;  // Hardware Reset LCD
  delay_ms(2);
  RESX_1;  // Return Reset OFF
  delay_ms(150);
  
  ili9481_wr_cmd(ILI9481_SLEEP_OUT); // Exit Sleep Mode 0x11
  delay_ms(100);  // Wait Stability
		
  ili9481_wr_cmd(ILI9481_NORMAL_DISPLAY_MODE_ON); // Entering Nomal Displaymode 0x13

/*  
  ili9481_wr_cmd(0xD0); // LCD Power Settings
  ili9481_wr_dat(0x07); // 1.0xVci
  ili9481_wr_dat(0x40); // 40 41
  ili9481_wr_dat(0x1C); // 1c 1e

  ili9481_wr_cmd(0xD1); // VCOM Control
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x18);
  ili9481_wr_dat(0x1D);

  ili9481_wr_cmd(0xD2); // Power_Setting for Normal Mode
  ili9481_wr_dat(0x01);
  ili9481_wr_dat(0x11); // 02 11

  ili9481_wr_cmd(0xC0); // Panel Driving Settings
  ili9481_wr_dat(0x00); // установить сканирование режим
  ili9481_wr_dat(0x3B);// Установить количество линий 480
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x02); // 5frames
  ili9481_wr_dat(0x11);

  ili9481_wr_cmd(0xC1); // Display_Timing_Setting for Normal Mode
  ili9481_wr_dat(0x10);
  ili9481_wr_dat(0x0B); 
  ili9481_wr_dat(0x88);

  ili9481_wr_cmd(0xC5); // Frame Rate and Inversion Control
  ili9481_wr_dat(0x01); // 100 Гц
*/
  
/*
  ili9481_wr_cmd(0xC8); // Gamma Settings
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x30); // 32
  ili9481_wr_dat(0x36);
  ili9481_wr_dat(0x45);
  ili9481_wr_dat(0x04); // 06 
  ili9481_wr_dat(0x16);
  ili9481_wr_dat(0x37);
  ili9481_wr_dat(0x75);
  ili9481_wr_dat(0x77);
  ili9481_wr_dat(0x54);
  ili9481_wr_dat(0x0F);
  ili9481_wr_dat(0x00);
*/
  
/*
  ILI9481_wr_cmd(0xE4); // ???
  ILI9481_wr_dat(0xA0);

  ILI9481_wr_cmd(0xF0); // ???
  ILI9481_wr_dat(0x01);

  ILI9481_wr_cmd(0xF3); // ???
  ILI9481_wr_dat(0x40);
  ILI9481_wr_dat(0x0A);

  ILI9481_wr_cmd(0xF7); // ???
  ILI9481_wr_dat(0x80);
*/


  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL); // Set_address_mode 0x36
  // B7         B6         B5              B4                B3       B2              B1           B0
  // направл Y, направл X, поменять X & Y, Vertical Order, RGB/BGR, DisplDataLatch=0, Horiz Flip,  Vert Flip

  ili9481_wr_dat(0x08); // BGR-order (0x88)
  ili9481_wr_cmd(ILI9481_INTERFACE_PIXEL_FORMAT); // Set_pixel_format 0x3A
  ili9481_wr_dat(0x55); // RGB565(16dpp)
/*
  ili9481_wr_cmd (0x002A); // Set column address
  ili9481_wr_dat (0x0000); // Start X HI
  ili9481_wr_dat (0x0000); // Start X LO
  ili9481_wr_dat ((ILI9481_PIXELX>>8)&0XFF); // End X HI
  ili9481_wr_dat (ILI9481_PIXELX&0xFF); // End X LO
     
  ili9481_wr_cmd (0x002B); // Set page address
  ili9481_wr_dat (0x0000); // Start Y HI
  ili9481_wr_dat (0x0000);  // Start Y LO
  ili9481_wr_dat ((ILI9481_PIXELY>>8)&0XFF);  // End Y HI
  ili9481_wr_dat (ILI9481_PIXELY&0xFF); // End Y LO
*/

/*
ILI9481_wr_cmd(0xB3);
ILI9481_wr_dat(0x00);
ILI9481_wr_dat(0x00);
ILI9481_wr_dat(0x00);
ILI9481_wr_dat(0x11);
delay_ms(120);
*/

  ili9481_wr_cmd(0x29); // Display ON
//  ili9481_wr_cmd(0x2C); // Write Data

//  ili9481_clear(RED_COLOR);
}

//------------------------------------------
//
/*
unsigned int read_lcd_id (void)
{
  // Check Device Code
//  devicetype = ILI9481_rd_cmd(0xBF);  	// Confirm Vaild LCD Controller
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

