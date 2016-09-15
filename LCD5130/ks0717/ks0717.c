// ======================= KS0717 Routines ============================

// ********************************************************************
// Writen by (R)soft 8.5.2013 Last Edition: 8 May 2013
// Version 1.0
// Tested on Mega2561 
// Testing on MOTOROLA V50 LCD Module. Resolution 96x54 pixels. 
// Built-in KS0717TB-01B (KS0717 compatible) controller with parallel interface.
// Reference manual for controller - KS0717.pdf
// ********************************************************************
/*
Заметки в процессе написания кода...
Судя по разводке кристалла на шлейфе, выбран интерфейс 6800-system:
Сигнал RW всегда ноль - т.е. можно только писать в контроллер.
ENABLE - строб для записи, положительный фронт.
Сигнал RESET можно не подключать к контроллеру, проверено,
заводим его на +VCC.
При изменении VCC с 2,8V на 3,3V контрастность значительно не меняется
*/

//=============================================================
// Запись байта в ЖКИ
// Если RS=0 то запись команды, если RS=1 то запись данных
// ks0717_write(1,0xff);

void ks0717_write (unsigned char rs, unsigned char byte)
{
  
  // Сначала выставляем RS в соответствии
  if (rs)
    {
      RS_1;
    }
  else
    {
      RS_0;
    }
//  delay_us(25);

  DDRDATA=~(byte); // Перед выдачей байт инвертируем
//  delay_us(25);
  ENABLE_1; // Запись байта
  delay_us(1); // здесь нужна мимнимальная задержка для импульса ENABLE
  ENABLE_0;
//  delay_us(25);
  
}

//=============================================================
// Инициализация LCD по даташиту:
// 1. ADC Select
// 2. SHL Select
// 3. LCD Bias Select
// 4. Voltage Converter ON
// 5. Wait > 1 ms
// 6.Voltage Regulator ON
// 7. Wait > 1 ms
// 8. Voltage Follower ON
// 9. Regulator Resistor Select
// 10. Reference Voltage Register Set (Two Instructions)
//    RAM Addressing Instructions: 
//    11. Initial Display Line (YADDR)
//    12. Set Page Address
//    13. Set Column Address (XADDR)
//    14. Set Static Indicator State (Two Instructions)
//    15. Turn Display ON


void ks0717_init (void)
{
  PIN_RS=0; // Инициализация всех используемых ног в ноль
  PIN_ENABLE=0;
  PIN_RESETN=0;
  PDATA=0;

  ENABLE_0;  // Исходное состояние линии ENABLE
  RESETN_0;  // Reset LCD
  delay_ms(10);
  RESETN_1;  // Return Reset OFF
  delay_ms(10);

  ks0717_write(0,KS0717_ADC_NORMAL); // Зеркалирование по X
  ks0717_write(0,KS0717_SHL_REVERSE); // Зеркалирование по Y
  ks0717_write(0,KS0717_SELECT_BIAS_18);
  ks0717_write(0,KS0717_POWER_CONTROL|0x04); // VC=1, VR=0, VF=0 - Voltage Converter ON
  delay_ms(2);
  ks0717_write(0,KS0717_POWER_CONTROL|0x06); // VC=1, VR=1, VF=0 - Voltage Regulator ON
  delay_ms(2);
  ks0717_write(0,KS0717_POWER_CONTROL|0x07); // VC=1, VR=1, VF=1 - Voltage Follower ON
  ks0717_write(0,KS0717_RESISTOR_SLCT|0x07); // 0...7 - при изменении изображение не меняется

  ks0717_write(0,KS0717_REF_VOLT_MODE); // Two Bytes Instruction
  ks0717_write(0,0x23); // 00...3F Для Bias 1/8 оптим.знач. 0x23, для Bias 1/6 - 0x0F
  ks0717_write(0,KS0717_SETYADDR|0x00);
  ks0717_write(0,KS0717_SET_PAGE_ADDR|0x00); // 0...8
  ks0717_write(0,KS0717_SETXADDR_LO|0x00);
  ks0717_write(0,KS0717_SETXADDR_HI|0x00);

  ks0717_write(0,KS0717_STATIC_IND_ON); // Two Bytes Instruction
  ks0717_write(0,0x03); // 0...3 (3 - Always ON)

//  ks0717_write(0,KS0717_ENTIRE_OFF);
//  ks0717_write(0,KS0717_REVERSE_OFF);

  ks0717_write(0,KS0717_DISPLAY_ON);
  delay_ms(10);
}

//==================================
// Чистим буфер консоли требуемым байтом
void ks0717_clear_console (unsigned char byte)
{
 unsigned int i;
 char *p;
 p=CON0717;
 i=sizeof(CON0717);
 do
   {
     *p++=byte;
   }
  while (--i);
}

//==========================================================================
// Вывод символа на ЖКИ

void ks0717_putchar(unsigned char ch)
{
  unsigned int addr;
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x1F
  addr=ch;
  addr+=(addr<<2); // *5

  // Вывод пяти байт
  for (ch=0; ch<5; ch++)  
    {  
      ks0717_write(1,font5x7[addr++]); 
    }
  ks0717_write(1,0); // Вывод промежутка между символами
}

//=========================================
// Вывод строки (строка в ОЗУ)

void ks0717_puts (unsigned char *s)
{
  while (*s) // до тех пор пока не ноль - выводим
    ks0717_putchar(*s++);
}

//====================================================================
// Выводит из буфера консоли CON на ЖКИ

void ks0717_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON0717;

  for (row=0; row<KS0717_ROW; row++)
  {
  ks0717_write(0,KS0717_SET_PAGE_ADDR | row); // Установка Y
  ks0717_write(0,KS0717_SETXADDR_HI); 
  ks0717_write(0,KS0717_SETXADDR_LO);
    for (col=0; col<KS0717_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        ks0717_putchar(c);
      }
   }
}

//==========================================================================
// Установка/сброс пиктограмм в верхней строке.
// В индикаторе пиктограммы идут по адресам, кратным четырем,
// причем устанавливать нужно все четыре кратных байта.
// Параметры: номер пиктограммы, байт записи (0 - сброс, 0xFF - установка)
// Адреса пиктограмм имеют нелинейный порядок, ниже преведены
// номера для работы с пиктограммами:
// 02 - антенна
// 06 - первый уровень антенны
// 10 - второй уровень антенны
// 14 - третий уровень антенны
// 18 - четвертый уровень антенны
// 22 - пятый уровень антенны
// 30 - трубка поднята
// 42 - треугольник
// 54 - домик
// 66 - конверт
// 70 - пиктограмма звука
// 72 - колокольчик
// 74 - 3я часть батареи
// 78 - 1ч батареи
// 82 - 2ч батареи
// 86 - корпус батареи

void ks0717_piktogramm (unsigned char num, unsigned char byte)
{
  ks0717_write(0,KS0717_SET_PAGE_ADDR|0x08); // Установка Y для пиктограмм
  ks0717_write(0,KS0717_SETXADDR_HI|(num>>4)); // Выделяем старшую тетраду
  ks0717_write(0,KS0717_SETXADDR_LO|(num&0x0F)); // Выделяем младшую тетраду
  ks0717_write(1,byte);
}

//====================================
// Очистка экрана

void ks0717_cls(void)
{
  unsigned char x,y;

  for (y=0; y<(KS0717_ROW+3); y++)  // Последняя страница 9 - пиктограммы
  {
    ks0717_write(0,KS0717_SET_PAGE_ADDR|y); // Установка Y
    ks0717_write(0,KS0717_SETXADDR_HI); // X=0
    ks0717_write(0,KS0717_SETXADDR_LO);
    for (x=0; x<KS0717_PIXELX; x++) // Цикл для записи нулей
      {
        ks0717_write(1,0x00); // Запись в ОЗУ LCD нулей
      }
  }
  // После очистки экрана, стандартно - позиция вывода вверху слева
//  ks0717_write(0,KS0717_SETYADDR); // Установка Y=0
//  ks0717_write(0,KS0717_SETXADDR_HI); // X=0
//  ks0717_write(0,KS0717_SETXADDR_LO);
}
