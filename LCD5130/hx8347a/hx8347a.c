// HX8347A Routine
// ********************************************************************
// Writen by (R)soft 22.5.2013
// Version 1.0
// Tested on Mega2561
// Testing on HR24CMP18(for wisky) LCD Module. Resolution 320x240 pixels. 
// Built-in HX8347A controller with parallel 8-bit interface.
// Reference manual for controller - HX8347.pdf
// ********************************************************************


//=============================================================
// Запись команды в ЖКИ
void hx8347a_write_command (unsigned char byte)
{
  DNC_0; // Режим команды
  NCS_0; // Включаем кристалл

  DDRD_LO=~(byte); // инвертируем и выставляем байт на шину

  NWR_0; // WRX=0 - Write mode
//  delay_us(HX8347A_DELAY); // Задержка
  NWR_1; // байт передается в контроллер
  NCS_1; // Отключили кристалл
}


//=============================================================
// Запись слова данных в ЖКИ

void hx8347a_write_data (unsigned int word)
{
  DNC_1; // Режим записи данных
  NCS_0; // Включаем кристалл
  // Выставление на шину данных
  DDRD_HI=~((unsigned char)(word>>8)); // Выделяем старший байт и инвертируем  
  DDRD_LO=~((unsigned char)word); // Выделяем младший байт и инвертируем
  NWR_0; // WRX=0 - Write mode
//  delay_us(HX8347A_DELAY); // Задержка
  NWR_1; // Latch - байт передается в контроллер
  NCS_1; // Отключили кристалл
}


//=============================================================
// Чтение байта из ЖКИ
// DNC при чтении всегда 1
// Эта функция писалась исключительно для отладки кода и
// отображения отклика HX8347A - чтение регистров.

unsigned char hx8347a_read (void)
{
  DNC_1;
  NCS_0; // Включаем кристалл
  DDRD_LO=0; // перевод шины данных в Z состояние
  NRD_0;  // RDX=0 - Read mode  Строб чтения - данные выставились
  
  delay_us(HX8347A_DELAY); // Задержка обязательна! Иначе не успевают зафиксироваться данные на шине
  
  // Читаем с шины данных младший байт
  unsigned char lo=PIN_LO;
  
  NRD_1;  // Возврат RDX в исходное состояние
  NCS_1; // Отключили кристалл
  return (lo);
}


//===========================================
// Установка регистров 8-битным значением
void hx8347a_set_register (unsigned char cmd, unsigned char byte)
{
  hx8347a_write_command(cmd);
  hx8347a_write_data(byte);
}


//=======================================
// Заполнение n строк одним цветом
void hx8347a_fill (unsigned int lines, unsigned int color)
{    
 unsigned int a,b;   
 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(240);b++)
         {
           hx8347a_write_data(color);
         }
   }
}


//=======================================
// Очистка экрана одним цветом
void hx8347a_set_xy (unsigned int x, unsigned int y)
{    
  hx8347a_set_register(0x02,(y>>8));     //High Column Start   
  hx8347a_set_register(0x03,(y&0xFF));  //Low Column Start   
       
  hx8347a_set_register(0x06,(x>>8));     //High Row Start   
  hx8347a_set_register(0x07,(x&0xFF));  //Low Row Start   
}

//------------------------------------------
// Draw a Pixel on the Screen by x & y
void putpixel(unsigned short x, unsigned short y, unsigned short color)
{
  hx8347a_set_register(0x02,(y>>8));     //High Column Start   
  hx8347a_set_register(0x03,(y&0xFF));  //Low Column Start   
       
  hx8347a_set_register(0x06,(x>>8));     //High Row Start   
  hx8347a_set_register(0x07,(x&0xFF));  //Low Row Start   
       
//    cursor_pos_x = x;cursor_pos_y = y;   
       
  hx8347a_write_command(0x22);  // Command - Write to GRAM
       
  hx8347a_write_data(color); //Draw the Pixel   
}

//=======================================
// Очистка экрана одним цветом
void hx8347a_clr_screen (unsigned int color)
{    
 unsigned int x,y;   
 for(y=0;y<(240);y++) // Одна строка
   {
      for(x=0;x<(320);x++)
         {
           putpixel(x,y,color);
         }
   }
}

//=========================================================
// Инициализания дисплея
void hx8347a_init(void)
{
  PIN_NRESET=0; // Инициализация всех используемых ног в ноль
  PIN_NCS=0;
  PIN_NRD=0;
  PIN_NWR=0;
  PD_LO=0;
  PD_HI=0;
  
  NCS_1; // Выбор кристалла исходно - кристалл выключен
  NRD_1;  // Исходное состояние Read=1
  NWR_1;  // Исходное состояние Write=1
  DNC_0;
  
  NRESET_0;  // Reset LCD
  delay_ms(1);
  NRESET_1;  // Return Reset OFF
  delay_ms(50); 

  hx8347a_set_register(HX8347A_GAMMA_CTRL1,0xA4);
  hx8347a_set_register(HX8347A_GAMMA_CTRL2,0x53);
  hx8347a_set_register(HX8347A_GAMMA_CTRL3,0x00);
  hx8347a_set_register(HX8347A_GAMMA_CTRL4,0x44);
  hx8347a_set_register(HX8347A_GAMMA_CTRL5,0x04);
  hx8347a_set_register(HX8347A_GAMMA_CTRL6,0x67);
  hx8347a_set_register(HX8347A_GAMMA_CTRL7,0x33);
  hx8347a_set_register(HX8347A_GAMMA_CTRL8,0x77);
  hx8347a_set_register(HX8347A_GAMMA_CTRL9,0x12);
  hx8347a_set_register(HX8347A_GAMMA_CTRL10,0x4C);
  hx8347a_set_register(HX8347A_GAMMA_CTRL11,0x46);
  hx8347a_set_register(HX8347A_GAMMA_CTRL12,0x44);

  //240x320 window setting
  hx8347a_set_register(HX8347A_COLSTARTHIGH,0x00); // Column address start2
  hx8347a_set_register(HX8347A_COLSTARTLOW,0x00); // Column address start1
  hx8347a_set_register(HX8347A_COLENDHIGH,0x00); // Column address end2
  hx8347a_set_register(HX8347A_COLENDLOW,0xef); // Column address end1
  hx8347a_set_register(HX8347A_ROWSTARTHIGH,0x00); // Row address start2
  hx8347a_set_register(HX8347A_ROWSTARTLOW,0x00); // Row address start1
  hx8347a_set_register(HX8347A_ROWENDHIGH,0x01); // Row address end2
  hx8347a_set_register(HX8347A_ROWENDLOW,0x3f); // Row address end1

  // Display Setting
  hx8347a_set_register(HX8347A_DISPL_MODE_CTRL,0x06); // IDMON=0, INVON=1, NORON=1, PTLON=0
  hx8347a_set_register(0x16,0xC8); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0   0048
  hx8347a_set_register(0x23,0x95); // N_DC=1001 0101
  hx8347a_set_register(0x24,0x95); // PI_DC=1001 0101
  hx8347a_set_register(0x25,0xFF); // I_DC=1111 1111
  hx8347a_set_register(0x27,0x02); // N_BP=0000 0010
  hx8347a_set_register(0x28,0x02); // N_FP=0000 0010
  hx8347a_set_register(0x29,0x02); // PI_BP=0000 0010
  hx8347a_set_register(0x2a,0x02); // PI_FP=0000 0010
  hx8347a_set_register(0x2C,0x02); // I_BP=0000 0010
  hx8347a_set_register(0x2d,0x02); // I_FP=0000 0010

  hx8347a_set_register(0x3a,0x01); // N_RTN=0000, N_NW=001    0001
  hx8347a_set_register(0x3b,0x00); // P_RTN=0000, P_NW=001
  hx8347a_set_register(0x3c,0xf0); // I_RTN=1111, I_NW=000
  hx8347a_set_register(0x3d,0x00); // DIV=00
  delay_ms(1);
  hx8347a_set_register(0x35,0x38); // EQS=38h
  hx8347a_set_register(0x36,0x78); // EQP=78h
  hx8347a_set_register(0x3E,0x38); // SON=38h
  hx8347a_set_register(0x40,0x0F); // GDON=0Fh
  hx8347a_set_register(0x41,0xF0); // GDOFF

  // Power Supply Setting
  hx8347a_set_register(HX8347A_OSC_CTRL1,0x49); // CADJ=0100, CUADJ=100, OSD_EN=1 ,60Hz
  hx8347a_set_register(HX8347A_OSC_CTRL3,0x0F); // RADJ=1111, 100%
  delay_ms(1);
  hx8347a_set_register(0x20,0x40); // BT=0100
  hx8347a_set_register(0x1D,0x07); // VC1=111   0007
  hx8347a_set_register(0x1E,0x00); // VC3=000
  hx8347a_set_register(0x1F,0x04); // VRH=0011

  //VCOM SETTING
  hx8347a_set_register(HX8347A_VCOM_CTRL2,0x40); // VCM=101 0000  4D
  hx8347a_set_register(HX8347A_VCOM_CTRL3,0x0E); // VDV=1 0001   0011
  delay_ms(1);
  hx8347a_set_register(0x1C,0x04); // AP=100
  delay_ms(2);
  hx8347a_set_register(0x1B,0x18); // GASENB=0, PON=0, DK=1, XDK=0, VLCD_TRI=0, STB=0
  delay_ms(1);
  hx8347a_set_register(0x1B,0x10); // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
  delay_ms(1);
  hx8347a_set_register(HX8347A_VCOM_CTRL1,0x80); //set VCOMG=1
  delay_ms(2);

  // Display ON Setting
  hx8347a_set_register(HX8347A_DISPLAY_CTRL8,0x7F); // SAP=0111 1111
  hx8347a_set_register(0x26,0x04); //GON=0, DTE=0, D=01
  delay_ms(1);
  hx8347a_set_register(0x26,0x24); //GON=1, DTE=0, D=01
  hx8347a_set_register(0x26,0x2C); //GON=1, DTE=0, D=11
  delay_ms(1);
  hx8347a_set_register(0x26,0x3C); //GON=1, DTE=1, D=11

  // INTERNAL REGISTER SETTING
  hx8347a_set_register(0x57,0x02); // TEST_Mode=1: into TEST mode
  hx8347a_set_register(0x95,0x01); // SET DISPLAY CLOCK AND PUMPING CLOCK TO SYNCHRONIZE
  hx8347a_set_register(0x57,0x00); // TEST_Mode=0: exit TEST mode

  hx8347a_write_command(0x22);   // Set Write to GRAM & Set x&y=0
}

//=============================== BMP Output ============================
// width - ширина картинки
// heigth - высота картинки
// *data - указатель на данные RGB в формате 24 бита на цвет и следование цветов BGR
// Данные преобразуются из 24-битного цвета в 16-битным путем отбрасывания младших
// битов (красный, синий - отбрасывается три младшийх бита; зелёный - два бита).

void hx8347a_bmp_output(unsigned int width, unsigned int height, unsigned char __farflash *data)
{
  unsigned int w,h; // макс. размер картинки 65535x65535
  unsigned int index=0; // указатель на текущие данные массива bmp
  unsigned char red, green, blue; // цвета
  unsigned int out_color; // 16-битный цвет для вывода
  
  hx8347a_write_command(0x22);;  // Index Register 22 - Write Data To CGRAM
  
  for (h=0; h<height; h++)  // вложенные циклы перебора столбец-строка
    {
      for (w=0; w<width; w++)
        {
          red=data[index];  // берём последовательно из массива цвета
          index++;
          green=data[index];
          index++;
          blue=data[index];
          index++;

          red=(red>>3)&0x1F;  // в каждом цвете отбрасываем лишние биты путём сдвига и маски
          green=(green>>2)&0x3F;
          blue=(blue>>3)&0x1F;
          out_color=((blue<<11)|(green<<5)|red); // собираем все цвета воедино в одном слове
          
          hx8347a_write_data(out_color); // Установка одного пикселя          
        }
    }
}



