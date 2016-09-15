
// ======================= PCF8814 Routine ============================

// ********************************************************************
// Writen by (R)soft 21.10.2011 Last Edition: 21 Oct 2011
// Version 1.0
// Tested on Mega2561 & Mega162.
// Testing on Nokia 6510 LCD Module. Resolution 96x65 pixels. 
// Built-in PCF8814 controller with 3-line SPI interface.
// Reference manual for controller - PCF8814.pdf
// ********************************************************************


//=============================================================
// Запись байта в ЖКИ
// Сначала MSB (первым идет старший бит)

void pcf8814_write (unsigned char ch)
{
  unsigned char i;
  for (i=8;i;i--)
  {
    if (ch&0x80) // Анализируется самый старший бит в байте
      {
        SDIN_1; // и выставляется SDA в соответствии
      }
    else  
      {
        SDIN_0;
      }
    SCLK_1; // Запись по нарастанию SCLK
    SCLK_0; // Возвращаем SCLK обратно в исходное состояние
    ch<<=1; // Сдвигаем байт влево
  }
}

//===========================================
// Отправка команды в ЖКИ

void pcf8814_cmd(unsigned char cmd)
{
  SDIN_0; // Признак команды D/C=0
  SCE_0; // Начало - выбор кристалла
  SCLK_1; // Передаем первый бит
  SCLK_0; // Возврат клока в исходное состояние
  pcf8814_write(cmd); // Передача байта
  SCE_1; // Выбор кристалла в исходное состояние - конец передачи
}

//=============================================
// Отправка данных в ЖКИ

void pcf8814_data(unsigned char dat)
{
  SDIN_1; // Признак данных D/C=1
  SCE_0; // Начало - выбор кристалла
  SCLK_1; // Передаем первый бит
  SCLK_0; // Возврат клока в исходное состояние
  pcf8814_write(dat);
  SCE_1; // Выбор кристалла в исходное состояние - конец передачи  
  if (++lcd8814_x > (PCF8814_PIXELX-1)) // Модифицируем позицию в экранном буфере
  {
    lcd8814_x=0; // Если был достигнут конец экрана то следующая строка
    lcd8814_y++;
  }
}

//===========================================================================
// Установка адреса в ЖКИ

void pcf8814_set_addr(unsigned char x, unsigned char y)
{
  if (y > (PCF8814_ROW-1)) 
    {
      y = (PCF8814_ROW-1); // Проверка максимального значения строки
    }
  if (x > (PCF8814_PIXELX-1)) 
    {
      x = (PCF8814_PIXELX-1); // Проверка максимального значения столбца
    }
  lcd8814_x = x;
  lcd8814_y = y;
  pcf8814_cmd(PCF8814_SETYADDR | y);
  pcf8814_cmd(PCF8814_SETXADDR | x);
}

//============================================================
// Переход к координатам x,y

void pcf8814_gotoxy(unsigned char x, unsigned char y)
{
  pcf8814_set_addr(((x<<1)+x),y); // nokia_set_addr(x*3,y)
}


//==========================================================================
// Вычисление начала символа в таблице шрифта
// Раскоментировать если эта процедура отсутствует

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

void pcf8814_putchar(unsigned char ch)
{
  unsigned int addr = char_to_addr(ch); // Вычисление адреса начала символа
  // Вывод пяти байт
  for (ch=0; ch<5; ch++)  
    {  
      pcf8814_data(font5x7[addr++]); 
    }
  pcf8814_data(0); // Вывод промежутка между символами
}

//====================================
// Очистка экрана

void pcf8814_cls(void)
{
  unsigned int i;
  pcf8814_gotoxy(0,0); // Установка начало адреса вывода в ноль
  for (i=0; i<(PCF8814_PIXELX*(PCF8814_ROW+1)); i++) // Цикл для записи (на одну строку больше)
    {
      pcf8814_data(0); // Запись в ОЗУ LCD нулей
    }
   pcf8814_gotoxy(0,0); // После очистки экрана, стандартно - позиция вывода вверху слева
}

//=========================================
// Вывод строки (строка в ОЗУ)

void pcf8814_puts (unsigned char *s)
{
  while (*s) // до тех пор пока не ноль - выводим
    pcf8814_putchar(*s++);
}

//==========================================
// Задать значение контраста

void pcf8814_set_contrast(unsigned char c)
{
  lcd8814_contrast=c;
  pcf8814_cmd(PCF8814_FUNCTIONSETEXT); // Сначала переключаемся в расширенный набор команд
  pcf8814_cmd(PCF8814_SET_VOP | c); // Установка значения контраста (напряжения LCD)
  pcf8814_cmd(PCF8814_FUNCTIONSET); // Возвращаемся к исходному набору команд
}

//=========================================
// Задать значение инверсии

void pcf8814_set_inverse(unsigned char inv)
{
  lcd8814_inverse=inv; // значение инверсии запоминаем в глобальной переменной
  if (inv)
    {
      pcf8814_cmd(PCF8814_DISPLAY_INVERT);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_NORMAL);
    }
}

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

//======================================================================
// Инициализания контроллера и выводов

void pcf8814_init (void)
{
  PIN_SCE=0; // Инициализация всех используемых ног в ноль
  PIN_SCLK=0;
  PIN_SDIN=0;
  PIN_RST=0;
//  PIN_CS=0;
//  PIN_LIGHT=0;
  
  SCLK_0;
//  CS_0; // Chip Select
  RST_0;  // Reset LCD
  delay_us(500);
  RST_1;  // Return Reset OFF
  delay_us(500);

  // Step 2 (Extended instruction Set H=1 Mode)
  pcf8814_cmd(PCF8814_FUNCTIONSETEXT);             // 001xxxxx

  pcf8814_cmd(PCF8814_TEMPCOEF);                   // 000001xx Temperature Coefficient
  pcf8814_cmd(PCF8814_VOLTAGE_MUL3X);              // 00001xxx Voltage Multiplier
  pcf8814_cmd(PCF8814_SET_BIAS | PCF8814_BIAS_48); // 0001xxxx Set Bias

  // Step 3 Set VOP
  unsigned char c=120;
  lcd8814_contrast=c;
  pcf8814_cmd(PCF8814_SET_VOP | c);                // 1xxxxxxx Voltage Value

  // Step 4 Set Normal Instruction Set H=0 (0x20)
  pcf8814_cmd(PCF8814_FUNCTIONSET);         // 001xMxxx Бит M - Mirror Y

  pcf8814_cmd(PCF8814_SET_VOP_RANGE_HIGH);         // 0001xxxx Set Voltage Range
                                                   // 01xxxxxx Set Y coordinate
                                                   // 1xxxxxxx Set X coordinate
  pcf8814_set_inverse(0); // Displaynormal

  pcf8814_cls(); // Очистка ОЗУ экрана и установка координат в ноль
}

//====================================================================
// Выводит из буфера консоли CON на ЖКИ
// Недостаток процедуры в том, что выводится весь буфер независимо от
// заполненности. Т.е. выводятся все 128 (16*8) символов на индикатор, 
// что не всегда нужно. Но, т.к. контроллер допускает обмен по интерфейсу
// на скорости 4 МБит (т.е. без задержек), общее время вывода всей инфы
// ничтожно мало. При желании можно модифицировать и оптимизоровать
// алгоритм.

void pcf8814_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8814;
  // Из-за особенностей контроллера и реализации дисплея пришлось выводить информацию
  // вверх тормашками :) Команда зеркалирования по Y была найдена, но команда
  // зеркалирования по Х неизвестна в данной реализации и версии контроллера, поэтому
  // решено использовать дисплей по модифицированному алгоритму вывода на дисплей.
  // Счетчик строк здесь уменьшается.
  row=PCF8814_ROW;
  do {
        --row;
        pcf8814_gotoxy(0,row);
        for (col=0 ;col<PCF8814_COL; col++)
          {
            c=*p++;
            if (c==0) 
              {
                c=0x20; // If End of Line (0x00) then "SPACE" character
              }
            pcf8814_putchar(c);
          }
      }
  while (row);
}

//==================================
// Чистим буфер консоли требуемым байтом
void pcf8814_clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON8814;
 i=sizeof(CON8814);
 do
   {
     *p++=byte;
   }
  while (--i);
}

