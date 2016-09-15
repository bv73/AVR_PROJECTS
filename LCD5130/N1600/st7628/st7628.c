// ======================= ST7628 Routine ============================
// 3-Line SPI (/CS SCLK SDATA)
// ********************************************************************
// Writen by (R)soft 12.12.2013 Last Edition: 12 Dec 2013
// Version 1.0
// Tested on Mega2561 @ 14.318 MHz
// Testing on Nokia 1600 LCD Module. Resolution 96x68 pixels. 
// ********************************************************************


/*

По большинству команд совместим с ...

Добавлено 12.12.2013: минимальное напряжение питания для Nokia 1600 - ?.? V

*/


//=============================================================
// Запись байта в ЖКИ
// Сначала MSB (первым идет старший бит)

void st7628_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // Анализируется самый старший бит в байте
      {
        SI_1; // и выставляется SDA в соответствии
      }
    else  
      {
        SI_0;
      }
//    delay_us(PCF8814_DELAY);
    SCL_1; // Запись по нарастанию SCLK
//    delay_us(PCF8814_DELAY);
    SCL_0; // Возвращаем SCLK обратно в исходное состояние
//    delay_us(PCF8814_DELAY);
    ch<<=1; // Сдвигаем байт влево
  }
}

//===========================================
// Отправка команды в ЖКИ

void st7628_cmd(unsigned char cmd)
{
  SI_0; // Признак команды D/C=0
  SCL_1; // Передаем первый бит
  SCL_0; // Возврат клока в исходное состояние
  st7628_write(cmd); // Передача байта
}

//=============================================
// Отправка данных в ЖКИ

void st7628_data(unsigned char dat)
{
  SI_1; // Признак данных D/C=1
  SCL_1; // Передаем первый бит
  SCL_0; // Возврат клока в исходное состояние
  st7628_write(dat);
}



//===========================================================================
// Установка адреса в ЖКИ

void st7628_set_addr(unsigned char x, unsigned char y)
{
  if (y > (ST7628_ROW-1)) 
    {
      y = (ST7628_ROW-1); // Проверка максимального значения строки (0..8)
    }
  if (x > (ST7628_PIXELX-1)) 
    {
      x = (ST7628_PIXELX-1); // Проверка максимального значения столбца
    }
//  sed_x = x;
//  sed_y = y;

  st7628_cmd(ST7628_CASET); // Установка X=0 Col address set
  st7628_data(x);
  st7628_data(ST7628_PIXELX-1);

  st7628_cmd(ST7628_RASET); // Y=0 Row address set
  st7628_data(y);
  st7628_data(ST7628_PIXELY-1);
}



//============================================================
// Установка координат вывода x,y

void st7628_gotoxy(unsigned char x, unsigned char y)
{
  st7628_set_addr((x*6),y); // set_addr(x*6,y)
}



//==========================================================================
// Вывод символа на ЖКИ 

void st7628_putchar(unsigned char ch)
{
  unsigned int addr;

  // Вычисление адреса начала символа
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5
  // Вывод пяти байт
  for (ch=0; ch<5; ch++)  
    {  
      st7628_data(font5x7[addr++]); 
    }
  st7628_data(0); // Вывод промежутка между символами
}

/*
//==========================================================================
// Вывод символа на ЖКИ 20x32
// Ширина 20 байт, высота - 4 байта (4*8)

void pcf8814_putchar32(unsigned char ch, unsigned char x, unsigned char y)
{
  unsigned int addr;
  unsigned char len; // Ширина символа в пикселах
  unsigned char lowadd, hiadd; // Переменные вычисления х адреса
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x2F
  addr=ch;
  addr=addr*81; // Умножение на длину символа в байтах
  len=(arial32[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      pcf8814_cmd(PCF8814_SETYADDR|y); // Установка Y=0
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+1); // Установка Y=1
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+2); // Установка Y=2
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+3); // Установка Y=3
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
    }
}

*/


//====================================
// Очистка экрана

void st7628_cls(unsigned char colour)
{
  unsigned int i;
  st7628_cmd(ST7628_CASET); // Установка X=0 Col address set
  st7628_data(0x00);
  st7628_data(ST7628_PIXELX-1);

  st7628_cmd(ST7628_RASET); // Y=0 Row address set
  st7628_data(0x00);
  st7628_data(ST7628_PIXELY-1);

  st7628_cmd(ST7628_RAM_WR);
  
  for (i=0; i<(ST7628_PIXELX * (ST7628_ROW+1)); i++) // Цикл для записи нулей
    {
      st7628_data(colour); // Запись в ОЗУ LCD нулей
    }

  // После очистки экрана, стандартно - позиция вывода вверху слева

}

/*
//=========================================
// Вывод строки (строка в ОЗУ)

void st7628_puts (unsigned char *s)
{
  while (*s) // до тех пор пока не ноль - выводим
    st7628_putchar(*s++);
}

*/

/*
//=========================================
// Задать значение инверсии

void pcf8814_set_inverse(unsigned char inv)
{
//  sed_inverse=inv; // значение инверсии запоминаем в глобальной переменной
  if (inv)
    {
      pcf8814_cmd(PCF8814_DISPLAY_REVERSE);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_NORMAL);
    }
}
*/


/*
//=========================================
// Включить/выключить дисплей

void pcf8814_set_power(unsigned char power)
{
  if (power)
    {
      pcf8814_cmd(PCF8814_DISPLAY_ON);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_OFF);
    }
}
*/

//=============================================================
// Инициализация LCD

void st7628_init (void)
{
  DDR_SCL=1;
  DDR_SI=1;
  DDR_XRST=1;
  SCL_0;

  delay_ms(50); // For Power Stable
  XRST_0;  // Reset LCD
  delay_ms(50);
  XRST_1;  // Return Reset OFF
//  pcf8814_cmd(PCF8814_SOFTWARE_RESET);
  delay_ms(50);
  
  st7628_cmd(ST7628_SW_RESET);  // Software reset
  delay_ms(50);

  st7628_cmd(ST7628_WR_CNTR);  // Contrast set
  st7628_data(0x39);

  st7628_cmd(ST7628_SLEEP_OUT);  // Sleep out

  st7628_cmd(ST7628_NOR_ON); 

  st7628_cmd(ST7628_INV_OFF); 

  st7628_cmd(ST7628_IDM_OFF); 

  st7628_cmd(ST7628_MADCTR);  // MAD set
  st7628_data(0x80);

  st7628_cmd(ST7628_DISPLAY_ON); 

  
//  st7628_cls(0x00); // Чистим видео-ОЗУ и устанавливаем адрес вывода в начало
  
}


//====================================================================
// Выводит из буфера консоли CON на ЖКИ

void st7628_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON7628;

  for (row=0; row<ST7628_ROW; row++)
  {
    st7628_cmd(ST7628_CASET); // Установка X=0 Col address set
    st7628_data(0x00);
    st7628_data(ST7628_PIXELX-1);

    st7628_cmd(ST7628_RASET); // Y=0 Row address set
    st7628_data(0x00);
    st7628_data(ST7628_PIXELY-1);

    for (col=0; col<ST7628_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        st7628_cmd(ST7628_RAM_WR);
        st7628_putchar(c);
      }
   }
}


//==================================
// Чистим буфер консоли требуемым байтом
void st7628_clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON7628;
 i=sizeof(CON7628);
 do
   {
     *p++=byte;
   }
  while (--i);
}

