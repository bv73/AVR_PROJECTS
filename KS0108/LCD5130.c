#include "font5x7.h" // Font
#include "LCD5130.h"

// ======================= LCD 5130 Routine ============================
// Modified by (R)soft 20.5.2011
// LPH7366 LCD Module. Resolution 84x48 pixel. Built-in PCD8544 controller with SPI interface.
// LPH7366-1, LPH7779, LPH7677 / driver PCD8544
// Reference manual for controller - PCD8544.pdf

// Запись байта в ЖКИ
// Сначала MSB (первым идет старший бит)
void nokia_w (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) NOKIA_DOUT=1; // Анализируется самый старший бит в байте
    else  NOKIA_DOUT=0;        // и выставляется SDA в соответствии
    NOKIA_SCK=0; // Запись по срезу SCLK
    NOKIA_SCK=1; // Возвращаем SCLK обратно в исходное состояние
    ch<<=1; // Сдвигаем влево
  }
}

// Отправка команды в ЖКИ
void nokia_cmd(unsigned char cmd)
{
  NOKIA_D_C=0; // Признак команды D/C=0
  nokia_w(cmd);
}

// Отправка данных в ЖКИ
void nokia_data(unsigned char dat)
{
  NOKIA_D_C=1; // Признак данных D/C=0
  nokia_w(dat);
  if (++nokia_x>GLCD_PIXELX-1) // Модифицируем позицию в экранном буфере
  {
    nokia_x=0; // Если был достигнут конец экрана то следующая строка
    nokia_y++;
  }
}

// Установка адреса в ЖКИ
void nokia_set_addr(unsigned char x, unsigned char y)
{
  if (y>5) y=5; // Проверка максимального значения строки (не больше 5!)
  if (x>GLCD_PIXELX-1) x=GLCD_PIXELX-1; // Проверка максимального значения столбца
  nokia_x=x;
  nokia_y=y;
  nokia_cmd(GLCD_SETYADDR | y);
  nokia_cmd(GLCD_SETXADDR | x);
}

// Переход к координатам x,y
void nokia_gotoxy(unsigned char x, unsigned char y)
{
  nokia_set_addr(((x<<1)+x),y); // nokia_set_addr(x*6,y)
}

// Вычисление начала символа в таблице шрифта
unsigned int char_to_pos(unsigned char ch)
{
  unsigned int pos;
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  pos=ch;
  pos+=(pos<<2); // *5
  return pos;
}

// Вывод символа на ЖКИ
void nokia_putchar(unsigned char ch)
{
  unsigned int pos = char_to_pos(ch); // Вычисление адреса начала символа
  for (ch=5; ch; ch--)  {  nokia_data(font5x7[pos++]); } // Вывод 5 байт
  nokia_data(0); // Вывод промежутка между символами
}

// Запись в LCD сразу двух байт
void nokia_ddata(unsigned char b)
{
  nokia_data(b);
  nokia_data(b);
}

// Очистка экрана
void nokia_cls(void)
{
  unsigned char i;
  nokia_gotoxy(0,0); // Установка адреса позиции в ноль
  for (i=252; i; i--) // Цикл для записи двойных байт
    nokia_ddata(0); // Запись в LCD 504 пустых байта
  nokia_gotoxy(0,0);
}

// Вывод строки (строка в ОЗУ)
void nokia_puts (unsigned char *s)
{
  while (*s)
    nokia_putchar(*s++);
}

// Задать значение контраста
void nokia_set_contrast(unsigned char c)
{
  nokia_contrast=c;
  nokia_cmd(GLCD_FUNCTIONSETEXT);
  nokia_cmd(GLCD_SET_VOP | c);
  nokia_cmd(GLCD_FUNCTIONSET);
}

// Задать значение инверсии
void nokia_set_inverse(unsigned char inv)
{
  nokia_inverse=inv;
  if (inv)
    nokia_cmd(GLCD_DISPLAYINVERT);
  else
    nokia_cmd(GLCD_DISPLAYNORMAL);
}

// Инициализания контроллера
void nokia_init (void)
{
  MCUCR=0x00;
  XMCRB=0x00;
  DDRA=0xFF; // Все на вывод
  NOKIA_SCK=1;
  NOKIA_CS=0; // Chip Select
  NOKIA_RESET=0;  // Reset LCD
  delay_us(500);
  NOKIA_RESET=1;  // Return Reset OFF
  delay_us(500);
  NOKIA_BLIGHT=1; // Set Back Light
  nokia_cmd(GLCD_FUNCTIONSETEXT); // Step 2 (Extended instruction Set H=1 Mode)
  // Step 3 Set VOP
  // Step 4 Set Normal Instruction Set H=0 (0x20)
  nokia_cmd(GLCD_TEMPCOEF);
  nokia_cmd(GLCD_SET_BIAS | GLCD_BIAS_10);
  nokia_set_contrast(120);
  nokia_set_inverse(FALSE);
  nokia_cls();
}

// Выводит из буфера консоли CON на ЖКИ
void UpdateConsole(void)
{
  char *p;
  char i;
  char c;  
  nokia_gotoxy(0,0);
  p=CON;
  i=14*6; // How many symbols
  do
  {
    c=*p++;
    if (c==0) c=32; // If End of Line then "SPACE" character
    nokia_putchar(c);
  }
  while (--i);
}

// Чистим буфер пробелами
void ClrScr (void)
{
  memset (CON,' ',sizeof(CON));
}

/*
// Вывод точки
void LcdPixel (unsigned char x, unsigned char y)
{
    unsigned char offset;
    unsigned char data;

    if (x > 84) return;
    if (y > 48) return;

    offset = y % 8;
    data = (0x01<<offset);
        
    nokia_gotoxy(x, (y/8));
    nokia_data(data);
} 
 
// Стирание точки
void LcdPixelErase (unsigned char x, unsigned char y)
{
    unsigned char offset;
    unsigned char data;

    if (x > 84) return;
    if (y > 48) return;

    offset = y % 8;
    data = (0x01<<offset);
        
    nokia_gotoxy(x, (y/8));
    nokia_data(~data);
} 

// Стирание колонки
void erase_column (unsigned char x, unsigned char y)
{
   unsigned char line;
   for(line=y; line<5; line++)
   {
      nokia_gotoxy(x, line);
      nokia_data(0x00);
   }
}

*/