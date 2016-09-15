// I use SED15B1 taken from a cell phone (Nokia 1200). It's on SPI. 
// This is my initialization subroutine. 

// ======================= SED15B1 Routine ============================
// 3-Wire SPI (/CS SCL SI)
// ********************************************************************
// Written by (R)soft 17.10.2011-2013 Last Edition: 4 Nov 2013
// Version 1.3
// Tested on Mega2561.
// Testing on Nokia 1200 LCD Module. Resolution 96x65 pixels.
// Built-in SED15B1 controller with 3-Wite SPI interface.
// Reference manual for controller - sed15b1.pdf
// ********************************************************************

// Контроллер SED15B1 совместим по командам с PCF8814.
// Также подмечено, что возможна совместимость с SSD1818. Но не проверено.
// Последний раз писано под ИАР 5.51 с Мегой2561@14.318

//=============================================================
// Запись байта в ЖКИ
// Сначала MSB (первым идет старший бит)

void sed15b1_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // Анализируется самый старший бит в байте
      {
        SI_1; // и выставляется SI в соответствии
      }
    else  
      {
        SI_0;
      }
//    delay_us(10);
    SCL_0; // Запись по срезу SCLK
//    delay_us(10);
    SCL_1; // Возвращаем SCLK обратно в исходное состояние
//    delay_us(10);
    ch<<=1; // Сдвигаем байт влево
  }
}

//===========================================
// Отправка команды в ЖКИ

void sed15b1_cmd(unsigned char cmd)
{
  SI_0; // Признак команды A0=0
  SCL_0;  // Передаем первый бит
  SCL_1;
  sed15b1_write(cmd);
}

//=============================================
// Отправка данных в ЖКИ

void sed15b1_data(unsigned char dat)
{
  SI_1; // Признак команды A0=1
  SCL_0;  // Передаем первый бит
  SCL_1;
  sed15b1_write(dat);
}

/*
//===========================================================================
// Установка адреса в ЖКИ

void sed15b1_set_addr(unsigned char x, unsigned char y)
{
  if (y > (SED15B1_ROW-1)) 
    {
      y = (SED15B1_ROW-1); // Проверка максимального значения строки (0..8)
    }
  if (x > (SED15B1_PIXELX-1)) 
    {
      x = (SED15B1_PIXELX-1); // Проверка максимального значения столбца
    }
  sed_x = x;
  sed_y = y;
  sed15b1_cmd(SED15B1_SETYADDR | y);
  sed15b1_cmd(SED15B1_SETXADDR_LOWER | (x&0x0F));
  sed15b1_cmd(SED15B1_SETXADDR_UPPER | ((x>>4)&0x0F));
}
*/

/*
//============================================================
// Установка координат вывода x,y

void sed15b1_gotoxy(unsigned char x, unsigned char y)
{
  sed15b1_set_addr(((x<<1)+x),y); // set_addr(x*3,y)
}
*/

//==========================================================================
// Вывод символа на ЖКИ 5x7

void sed15b1_putchar(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  addr=ch;
  addr=addr*5; // *5

  // Вывод пяти байт
  for (ch=0; ch<5; ch++)  
    {  
      sed15b1_data(font5x7[addr++]); 
    }
  sed15b1_data(0); // Вывод промежутка между символами
}


/*
//==========================================================================
// Вывод символа на ЖКИ 11x16
// Ширина 11 байт, высота - 2 байта (2*8)

void sed15b1_putchar16(unsigned char ch)
{
  unsigned int addr;
  unsigned char lowadd, hiadd; // Переменные вычисления х адреса
  unsigned char len; // Ширина символа в пикселах
  if (ch==' ') { ch=0x3A; }
  if (ch=='.') { ch=0x3B; }
  if (ch=='A') { ch=0x3C; }
  if (ch=='V') { ch=0x3D; }

  ch=ch-0x30; // Используется таблица символов без знаков 0x00..0x2F
  addr=ch;
  addr=addr*33; // Умножение на длину символа в байтах
  len=(arial16x16[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      sed15b1_cmd(SED15B1_SETYADDR|y); // Установка Y=0
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial16x16[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+1); // Установка Y=1
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial16x16[addr++]); 
    }
  x=x+len;
}
*/

//==========================================================================
// Вывод символа на ЖКИ 20x32
// Ширина 20 байт, высота - 4 байта (4*8)
/*
void sed15b1_putchar32(unsigned char ch, unsigned char x, unsigned char y)
{
  unsigned int addr;
  unsigned char len; // Ширина символа в пикселах
  unsigned char lowadd, hiadd; // Переменные вычисления х адреса
  if (ch=='+') { ch=0x3A; }
  if (ch=='-') { ch=0x3B; }
  if (ch=='.') { ch=0x3C; }
  ch=ch-0x30; // Используется таблица символов без знаков 0x00..0x2F
  addr=ch;
  addr=addr*81; // Умножение на длину символа в байтах
  len=(arial32[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      sed15b1_cmd(SED15B1_SETYADDR|y); // Установка Y=0
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+1); // Установка Y=1
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+2); // Установка Y=2
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
      sed15b1_cmd(SED15B1_SETYADDR|y+3); // Установка Y=3
      sed15b1_cmd(SED15B1_SETXADDR_LOWER | lowadd);
      sed15b1_cmd(SED15B1_SETXADDR_UPPER | hiadd);
      sed15b1_data(arial32[addr++]); 
    }
}
*/

//====================================
// Очистка экрана

void sed15b1_cls(void)
{
  unsigned int i;
  sed15b1_cmd(SED15B1_SETYADDR); // Установка Y=0

  // Для стирания видео-ОЗУ никакого сдвига! Чистим усё!!! :)
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); // X=0
  sed15b1_cmd(SED15B1_SETXADDR_LOWER);

  for (i=0; i<(132*8); i++) // Цикл для записи (на одну строку больше)
    {
      sed15b1_data(0); // Запись в ОЗУ LCD нулей
    }

  // После очистки экрана, стандартно - позиция вывода вверху слева
  sed15b1_cmd(SED15B1_SETYADDR); // Установка Y=0
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); // X=0
  sed15b1_cmd(SED15B1_SETXADDR_LOWER);
}

//=========================================
// Вывод строки (строка в ОЗУ)
/*
void sed15b1_puts (unsigned char *s)
{
  while (*s) // до тех пор пока не ноль - выводим
    sed15b1_putchar(*s++);
}
*/

//=========================================
// Задать значение инверсии
/*
void sed15b1_set_inverse(unsigned char inv)
{
//  sed_inverse=inv; // значение инверсии запоминаем в глобальной переменной
  if (inv)
    {
      sed15b1_cmd(SED15B1_DISPLAY_REVERSE);
    }
  else
    {
      sed15b1_cmd(SED15B1_DISPLAY_NORMAL);
    }
}
*/

//=========================================
// Включить/выключить дисплей
/*
void sed15b1_set_power(unsigned char power)
{
  if (power)
    {
      sed15b1_cmd(SED15B1_DISPLAY_ON);
    }
  else
    {
      sed15b1_cmd(SED15B1_DISPLAY_OFF);
    }
}
*/

//=============================================================
// Инициализация LCD
// Stage отмечены кое где, в соответствии с даташитом на SED15B1
// А вообще делалось по даташиту, потому что даташиты - рулез форева (если они есть) :)

void sed15b1_init (void)
{
  // Инициализация направлениия всех используемых ног
  DDR_SCL=1;
  DDR_SI=1;
  DDR_RESN=1;
  SCL_1;
  
  RESN_0;  // Reset LCD
  delay_ms(1);
  RESN_1;  // Return Reset OFF
  delay_ms(1);
  
  sed15b1_cmd(SED15B1_DISPLAY_NORMAL);  // Инверсия

// Параметры для переворачивания картинки  
  sed15b1_cmd(SED15B1_SEGMENT_DIR_NORMAL); //Зеркалирование по Х
  sed15b1_cmd(SED15B1_COMMON_DIR_REVERSE); // Зеркалирование по Y

// BIAS 1/7 даёт большее "посинение", т.е. напряжение экрана выше
// BIAS 1/9 как бы исходное...
  sed15b1_cmd(SED15B1_SET_BIAS_19); // Set BIAS

  // Подстройка контрастности дисплея - этими параметрами нужно играться до
  // победного посинения, чтобы добиться вменяемого качества картинки
  // у меня экран полосит как зебра, из-за этого мешает подстроить нормальный
  // уровень контрастности :(
  sed15b1_cmd(SED15B1_RESISTOR_RATIO | 0x04); // V0 Voltage Settings 0...6 (грубо)
  sed15b1_cmd(SED15B1_ELECTRONIC_VOLUME | 0x0F); // 0...1F (точно)

  sed15b1_cmd(SED15B1_ALL_POINTS_OFF); // Stage 9 Нормальное отображение всех пикселей

  // при отключенном voltage follover экран заливает синькой
  // при отключенном voltage booster вообще ничего не видно
  sed15b1_cmd(SED15B1_POWER_CTRL_SET | 0x07); // Всё включено - 0x07

  // Stage 10 Сдвиг начального адреса вывода (для скроллирования изображения вверх)
  sed15b1_cmd(SED15B1_START_LINE_ADDR | 0x00);

//  sed15b1_cmd(SED15B1_SETYADDR); // Stage 11  Установка Y=0
//  sed15b1_cmd(SED15B1_SETXADDR_UPPER | SED15B1_ADD_XHI); // Stage 12 X=0+add
//  sed15b1_cmd(SED15B1_SETXADDR_LOWER | SED15B1_ADD_XLO);

  //  sed15b1_cmd(SED15B1_ALL_POINTS_ON); // Команда засвечивает все пикселы

  sed15b1_cmd(SED15B1_DISPLAY_ON);
  sed15b1_cls(); // Чистим видео-ОЗУ и устанавливаем адрес вывода в начало
  
}


//====================================================================
// Выводит из буфера консоли CON на ЖКИ
// Недостаток процедуры в том, что выводится весь буфер независимо от
// заполненности. Т.е. выводятся все 128 (16*8) символов на индикатор, 
// что не всегда нужно. Но, т.к. контроллер допускает обмен по интерфейсу
// на скорости 4 МБит (т.е. без задержек), общее время вывода всей инфы
// ничтожно мало. При желании можно модифицировать и оптимизоровать
// алгоритм.

void sed15b1_update_console(void)
{
  unsigned char col,row;
  char *p;
  unsigned char c;  
  p=CON15B1;

  for (row=0; row<SED15B1_ROW; row++)
  {
  sed15b1_cmd(SED15B1_SETYADDR | row); // Установка Y
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); 
  sed15b1_cmd(SED15B1_SETXADDR_LOWER); // X=0
    for (col=0; col<SED15B1_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        sed15b1_putchar(c);
      }
   }
}


/*
//---------------------------------------------------------
void sed15b1_update_console16(void)
{
  unsigned char col,row;
  char *p;
  unsigned char c;  
  p=CON15B1;

  for (row=0; row<SED15B1_ROW; row=row+2) // y
  {
  sed15b1_cmd(SED15B1_SETYADDR | row); // Установка Y
  y=row;
  sed15b1_cmd(SED15B1_SETXADDR_UPPER); 
  sed15b1_cmd(SED15B1_SETXADDR_LOWER); // X=0
  x=0;
  for (col=0; col<9; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        sed15b1_putchar16(c);
      }
   }
}

*/

//==================================
// Чистим буфер консоли требуемым байтом
void sed15b1_clear_console (unsigned char byte)
{
 unsigned char i;
 char *p;
 p=CON15B1;
 i=sizeof(CON15B1);
 do
   {
     *p++=byte;
   }
  while (--i);
}
