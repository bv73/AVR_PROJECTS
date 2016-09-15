//============= ST7558 I2C Routines ========================
// i2c interface Address 0x78
// Written by (R)soft 10.5.2013
// v1.0
// Tested on Mega2561
// Testing on Motorola C115 LCD Module. Resolution 96x65 pixels.
// Сигнал RESET можно не подключать к контроллеру, проверено,
// заводим его на +VCC
// При изменении VCC с 2,8V на 3,3V контрастность значительно не меняется

// Built-in ST7558 Controller with I2C interface
// Reference Manual - ST7558.pdf

//-------------------------------------
// Set Y Coordinate
void st7558_set_y(unsigned char row)
{
   i2c_start();
   i2c_byte_write(ST7558_I2C_ADDR);
   i2c_byte_write(0x00); // Control Byte (A0=0 - Command Mode)
   i2c_byte_write(ST7558_FUNCTIONSET0);
   i2c_byte_write(ST7558_SET_YADDR|row);
}

//-------------------------------------
// Set X Coordinate
void st7558_set_x(unsigned char col)
{                                                       
   i2c_start();
   i2c_byte_write(ST7558_I2C_ADDR);
   i2c_byte_write(0x00); // Control Byte (A0=1 - Data Writing in RAM)
   i2c_byte_write(ST7558_FUNCTIONSET0);
   i2c_byte_write(ST7558_SET_XADDR|col);
}

//-----------------------------------------
// 1 - инверсный режим, 0 - обычный режим
void st7558_inverse(unsigned char mode)
{
   i2c_start();                              
   if(mode)                
    {
     i2c_byte_write(ST7558_I2C_ADDR);
     i2c_byte_write(0x00); // Control Byte (A0=0)
     i2c_byte_write(ST7558_FUNCTIONSET0);
     i2c_byte_write(ST7558_DISP_CONTROL|0x05); // D=1, E=1 Display Control
    }
   else
     {
     i2c_byte_write(ST7558_I2C_ADDR);
     i2c_byte_write(0x00); // Control Byte (A0=0 Command mode)
     i2c_byte_write(ST7558_FUNCTIONSET0);
     i2c_byte_write(ST7558_DISP_CONTROL|0x04); // D=1, E=0 Diplay Control
     }
}

//-------------------------------------
// Clear All LCD RAM
void st7558_clear_lcd(void)
{
  unsigned int i; 
  i2c_start();
  i2c_byte_write(ST7558_I2C_ADDR);
  i2c_byte_write(0x40); // Control Byte (A0=1)
  for (i = 0; i < (102*9); i++) // 9 строк потому, что нужно стереть последнюю 65-линию
    {
      i2c_byte_write(0x00); 
    }  
} 

//------------------------------
// Инициализация дисплея
void st7558_init(void)
  {
//  RESETN_0;
//  delay_ms(10);
//  RESETN_1;
  i2c_stop();
  delay_ms(10);
  //---------------------
  i2c_start();
  i2c_byte_write(ST7558_I2C_ADDR);
  i2c_byte_write(0x00); // Control Byte (A0=0)
  i2c_byte_write(ST7558_EXT_CONTROL|0x06); // MX=1, MY=1 (зеркалирование)

  i2c_byte_write(ST7558_FUNCTIONSET1);
  i2c_byte_write(ST7558_BIAS_SYSTEM|0x02); // BS=2
  i2c_byte_write(ST7558_SET_VOP|0x40); // VOP=0x40
  i2c_byte_write(ST7558_BOOST_STAGE|0x03); // Boost Multiplication

  i2c_byte_write(ST7558_FUNCTIONSET0);
  i2c_byte_write(ST7558_VLCD_RANGE|0x01); // PRS=1
  i2c_byte_write(ST7558_DISP_CONTROL|0x04); // D=1, E=0
  i2c_byte_write(ST7558_SET_YADDR); // Reset Y=0
  i2c_byte_write(ST7558_SET_XADDR); // Reset X=0
  //------------------
  i2c_start();
  i2c_byte_write(ST7558_I2C_ADDR);
  i2c_byte_write(0x00); // Control Byte (A0=0)
  i2c_byte_write(ST7558_FUNCTIONSET0);
  i2c_byte_write(ST7558_DISP_CONTROL); // D=0, E=0
   
  st7558_inverse(0); // Без инверсии
  st7558_clear_lcd(); // Чистим память ЖКИ
  }

//==========================================================================
// Вывод символа на ЖКИ, запись в ОЗУ данных
void st7558_putchar(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5

  i2c_start();
  i2c_byte_write(ST7558_I2C_ADDR);
  i2c_byte_write(0x40); // Control Byte (A0=1 - Write Data to RAM)
  
  // Вывод пяти байт данных
  for (ch=0; ch<5; ch++)  
    {  
      i2c_byte_write(font5x7[addr++]); 
    }
  i2c_byte_write(0x00); // Вывод промежутка между символами
}

//====================================================================
// Весь буфер консоли CON выводится на LCD
void st7558_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON7558;

  for (row=0; row<ST7558_ROW; row++)
  {
  st7558_set_y(row); // Установка Y
  st7558_set_x(0x00); // Установка X=0
  for (col=0; col<ST7558_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        st7558_putchar(c);
      }
   }
}

