
// ======================= PCD8544 Routine ============================
// Modified by (R)soft 17.9.2011
// Version 2.0
// Testiong on LPH7366 LCD Module. Resolution 84x48 pixel. 
// Built-in PCD8544 controller with SPI interface.
// LPH7366-1, LPH7779, LPH7677 / driver PCD8544
// Reference manual for controller - PCD8544.pdf


//=============================================================
// Запись байта в ЖКИ
// Сначала MSB (первым идет старший бит)

void pcd8544_w (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // Анализируется самый старший бит в байте
      {
        DOUT_1; // и выставляется SDA в соответствии
      }
    else  
      {
        DOUT_0;
      }
    SCK_0; // Запись по срезу SCLK
    SCK_1; // Возвращаем SCLK обратно в исходное состояние
    ch<<=1; // Сдвигаем байт влево
  }
}

//===========================================
// Отправка команды в ЖКИ

void pcd8544_cmd(unsigned char cmd)
{
  DC_0; // Признак команды D/C=0
  pcd8544_w(cmd);
}

//=============================================
// Отправка данных в ЖКИ

void pcd8544_data(unsigned char dat)
{
  DC_1; // Признак данных D/C=0
  pcd8544_w(dat);
  if (++lcd_x > (PCD8544_PIXELX-1)) // Модифицируем позицию в экранном буфере
  {
    lcd_x=0; // Если был достигнут конец экрана то следующая строка
    lcd_y++;
  }
}

//===========================================================================
// Установка адреса в ЖКИ

void pcd8544_set_addr(unsigned char x, unsigned char y)
{
  if (y > 5) 
    {
      y=5; // Проверка максимального значения строки (0..5)
    }
  if (x > (PCD8544_PIXELX-1)) 
    {
      x = (PCD8544_PIXELX-1); // Проверка максимального значения столбца
    }
  lcd_x = x;
  lcd_y = y;
  pcd8544_cmd(PCD8544_SETYADDR | y);
  pcd8544_cmd(PCD8544_SETXADDR | x);
}

//============================================================
// Переход к координатам x,y

void pcd8544_gotoxy(unsigned char x, unsigned char y)
{
  pcd8544_set_addr(((x<<1)+x),y); // nokia_set_addr(x*3,y)
}

//==========================================================================
// Вычисление начала символа в таблице шрифта

unsigned int char_to_addr(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5
  return addr;
}

//==========================================================================
// Вывод символа на ЖКИ

void pcd8544_putchar(unsigned char ch)
{
  unsigned int addr = char_to_addr(ch); // Вычисление адреса начала символа
  for (ch=5; ch; ch--)  
    {  
      pcd8544_data(font5x7[addr++]); 
    } // Вывод 5 байт
  pcd8544_data(0); // Вывод промежутка между символами
}

/*
//===================================
// Запись в LCD сразу двух байт

void pcd8544_ddata(unsigned char b)
{
  pcd8544_data(b);
  pcd8544_data(b);
}
*/

//====================================
// Очистка экрана

void pcd8544_cls(void)
{
  unsigned char i;
  pcd8544_gotoxy(0,0); // Установка адреса позиции в ноль
  for (i=252; i; i--) // Цикл для записи двух байт
    {
      pcd8544_data(0); // Запись в LCD 504 пустых байта
      pcd8544_data(0);
    }
   pcd8544_gotoxy(0,0); // После очистки экрана, стандартно - позиция вывода вверху слева
}

//=========================================
// Вывод строки (строка в ОЗУ)

void pcd8544_puts (unsigned char *s)
{
  while (*s) // до тех пор пока не ноль - выводим
    pcd8544_putchar(*s++);
}

//==========================================
// Задать значение контраста

void pcd8544_set_contrast(unsigned char c)
{
  lcd_contrast=c;
  pcd8544_cmd(PCD8544_FUNCTIONSETEXT);
  pcd8544_cmd(PCD8544_SET_VOP | c);
  pcd8544_cmd(PCD8544_FUNCTIONSET);
}

//=========================================
// Задать значение инверсии

void pcd8544_set_inverse(unsigned char inv)
{
  lcd_inverse=inv; // значение инверсии запоминаем в глобальной переменной
  if (inv)
    {
      pcd8544_cmd(PCD8544_DISPLAYINVERT);
    }
  else
    {
      pcd8544_cmd(PCD8544_DISPLAYNORMAL);
    }
}

//======================================================================
// Инициализания контроллера и выводов

void pcd8544_init (void)
{
//  MCUCR=0x00;
//  XMCRB=0x00;
  PIN_DC=0; // Инициализация всех используемых ног в ноль
  PIN_SCK=0;
  PIN_DOUT=0;
  PIN_RESET=0;
//  PIN_CS=0;
//  PIN_LIGHT=0;
  
  SCK_1;
//  CS_0; // Chip Select
  RESET_0;  // Reset LCD
  delay_us(500);
  RESET_1;  // Return Reset OFF
  delay_us(500);
//  LIGHT_1; // Set Back Light
  pcd8544_cmd(PCD8544_FUNCTIONSETEXT); // Step 2 (Extended instruction Set H=1 Mode)
  // Step 3 Set VOP
  // Step 4 Set Normal Instruction Set H=0 (0x20)
  pcd8544_cmd(PCD8544_TEMPCOEF);
  pcd8544_cmd(PCD8544_SET_BIAS | PCD8544_BIAS_10);
  pcd8544_set_contrast(120);
  pcd8544_set_inverse(0);
  pcd8544_cls();
}

//====================================================================
// Выводит из буфера консоли CON на ЖКИ
// Недостаток процедуры в том, что выводится весь буфер независимо от
// заполненности. Т.е. выводятся все 84 символа на индикатор, 
// что не всегда нужно.
// При желании можно модифицировать процедуру так что при встрече в буфере нуля
// будет происходить выход из процедуры.

void update_console(void)
{
  char *p;
  char i;
  char c;  
  pcd8544_gotoxy(0,0);
  p=CON;
  i=14*6; // How many symbols
  do
  {
    c=*p++;
    if (c==0) 
      {
        c=0x20; // If End of Line (0x00) then "SPACE" character
      }
    pcd8544_putchar(c);
  }
  while (--i);
}

//==================================
// Чистим буфер консоли требуемым байтом
void clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON;
 i=sizeof(CON);
 do
   {
     *p++=byte;
   }
  while (--i);
}

