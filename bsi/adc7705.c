// #include "adc7705.h"


/*
void SPI_Send (void)
{
  SPDR=0xFF;
  while(!SPSR_SPIF);
}
*/


void Reset_ADC1 (void)
{
  ADC_PORT&=~(1<<RST1);
  delay_us(4);
  ADC_PORT|=(1<<RST1);
}

void Reset_ADC2 (void)
{
  ADC_PORT&=~(1<<RST2);
  delay_us(4);
  ADC_PORT|=(1<<RST2);
}

void Reset_ADC3 (void)
{
  ADC_PORT&=~(1<<RST3);
  delay_us(4);
  ADC_PORT|=(1<<RST3);
}

/*
****************************************************************************
**************                                             *****************
**************                 Первый АЦП                  *****************
**************                                             *****************
****************************************************************************
*/

//===============================================================
// Запуск АЦП 1 Канал 1

void StartConversion_AD11 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 Включаем АЦП1
//  delay_us(1);
  
  SPDR=Command7705_Clk+0; // Clock Register
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[0]; // 0..3
  tmp&=0x03; // на всякий случай убираем лишние биты маской
  tmp|=Command7705_C_Clk; // 0x04
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 Отключаем АЦП1
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS1);  // CS1=0 Включаем АЦП1
//  delay_us(1);
  
  SPDR=Command7705_SetT+0;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[0]<<3);
  tmp&=0x38; // на всякий случай убираем лишние биты маской
  tmp|=(BSI_Settings.BiUniPolar.bit.BU0<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 Отключаем АЦП1  

  //-----------------------------------------------
}

//====================================================================
// Чтение из АЦП 1 канал 1
// Возвращает unsigned int - результат в сыром виде

unsigned int ReadData_AD11 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 Включаем АЦП1
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // Читаем старший байт результата

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // Читаем младший байт результата

  ADC_PORT|=(1<<CS1); // CS1=1 Отключаем АЦП1  

  //--------------------------------------------

  return (tmpH<<8|tmpL); // Превращение двух байт в слово и сохранение
}


//===============================================================
// Запуск АЦП 1 Канал 2

void StartConversion_AD12 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 Включаем АЦП1
//  delay_us(1);
  
  SPDR=Command7705_Clk+1;
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[1];
  tmp&=0x03; // на всякий случай убираем лишние биты маской
  tmp|=Command7705_C_Clk;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 Отключаем АЦП1
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS1);  // CS1=0 Включаем АЦП1
//  delay_us(1);
  
  SPDR=Command7705_SetT+1;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[1]<<3);
  tmp&=0x38; // на всякий случай убираем лишние биты маской
  tmp|=(BSI_Settings.BiUniPolar.bit.BU1<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS1); // CS1=1 Отключаем АЦП1  

  //-----------------------------------------------
}

//====================================================================
// Чтение из АЦП 1 канал 2
// Возвращает unsigned int - результат в сыром виде

unsigned int ReadData_AD12 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS1);  // CS1=0 Включаем АЦП1
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // Читаем старший байт результата

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // Читаем младший байт результата

  ADC_PORT|=(1<<CS1); // CS1=1 Отключаем АЦП1  

  //--------------------------------------------

  return (tmpH<<8|tmpL); // Превращение двух байт в слово и запись
}

//===================================================
// Перезапуск преобразования токового АЦП 1

void Conversion_I1 (void)
{
  Channel_NI12=0;  // Выбор канала 1 (=0)
  Reset_ADC1();
  StartConversion_AD11();
}

//===================================================
// Перезапуск преобразования токового АЦП 2

void Conversion_I2 (void)
{
  Channel_NI12=1;  // Выбор канала 2 (=1)
  Reset_ADC1();
  StartConversion_AD12();
}

//=========================================================================
// Вызывается после проверки готовности АЦП по ноге /DRDY.
// Процедура забирает данные из заданного битом NUMKAN.NI12 канала АЦП1,
// производит проверку на допустимые пределы измерения (максимум, минимум),
// фильтрует результат алгоритмом скользящего среднего,
// делает необходимые математические расчеты и устанавливает соотв. флаги
// готовности и/или ошибок.
// На выходе процедура запускает другой канал того же АЦП, чтобы при
// следующем опросе сделать вычисления в другом канале.
// Таким образом при каждом вызове процедуры считываются и вычисляются
// попеременно два канала АЦП1.

void Calc_ADC1 (void)
{
  unsigned int tmp; // Переменная для проверки пределов АЦП
  if (Channel_NI12) // Если бит канала установлен то это второй канал
    {
      // ====== Канал 2 of ADC1 ========

      tmp=ReadData_AD12();  // Чтение из АЦП1 канал 2
      BSI_ResultEx.CodeI[1]=tmp; // результат берем из ADC_I[1]
      ReadyDataADC.DI2=1; // Установка флага "данные готовы" по каналу 2
      
      // В tmp - результат АЦП для последующей проверки пределов
      if ( (BSI_Settings.UPHI[1] >= tmp) && (tmp >= BSI_Settings.UPLI[1]) ) // проверка на допустимый диапазон АЦП (пределы "больше" и "меньше")
        {
          BSI_ResultEx.CodeIAverage[1]=MovingAverage_I(tmp,1); // Если код АЦП в пределах допуска, то фильтруем скользящим средним
          BSI_ResultEx.BSI_Results.I[1]=Matematic_I(1);  // И делаем необходимые вычисления по каналу 2
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI2=1;         // Set OK Flag
        }
      else  // иначе (при выходе кода за допустимый предел) при необходиомсти чистим буфер скользящего среднего
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI2) // Если бит готовности =1 то чистим буфер
            {
              ClearMovingAverage_I(1); // Чистим буфер скользящего среднего канал 2
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI2=0;   // Потом сбрасываем бит DI2
        }
    }
  else  // иначе первый канал
    {
      // ====== Канал 1 of ADC1 ========
      
      tmp=ReadData_AD11();
      BSI_ResultEx.CodeI[0]=tmp; // Для первого канала индекс массива =0, сохраняем результат
      ReadyDataADC.DI1=1; // Установка флага "данные готовы" по каналу 1
//======
//      ADC_I[0]=0x28E1; Debug Value
//======

      if ( (BSI_Settings.UPHI[0] >= tmp) && (tmp >= BSI_Settings.UPLI[0]) )
        {
          BSI_ResultEx.CodeIAverage[0]=MovingAverage_I(tmp,0); // Если код АЦП в пределах допуска, то фильтруем скользящим средним
          BSI_ResultEx.BSI_Results.I[0]=Matematic_I(0);  // Вычисления тока по первому каналу (индекс ноль)
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI1=1; // Установить флаг ОК
        }
      else
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI1) // Если бит DI1=1 то чистим буфер
            {
              ClearMovingAverage_I(0); // Чистим буфер скользящего среднего канал 1
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI1=0;   // Сбрасываем бит DI1 (данные готовы)
        }
    }

  //----------------
  if (Channel_NI12) // Если был выбран канал 2, то запускаем первый
    {
      Conversion_I1();
    }
  else            // иначе был выбран первый, запускаем другой канал
    {
      Conversion_I2();
    }
}

/*
****************************************************************************
**************                                             *****************
**************                 Второй АЦП                  *****************
**************                                             *****************
****************************************************************************
*/


//===============================================================
// Запуск АЦП 2 Канал 1 (третий токовый канал)

void StartConversion_AD21 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 Включаем АЦП2
//  delay_us(1);
  
  SPDR=Command7705_Clk+0; // Clock Register
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[0+2]; // Добавка 2 - второй АЦП (значения 0...3)
  tmp&=0x03; // на всякий случай убираем лишние биты маской
  tmp|=Command7705_C_Clk; // 0x04
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 Отключаем АЦП2
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS2);  // CS2=0 Включаем АЦП2
//  delay_us(1);
  
  SPDR=Command7705_SetT+0;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[0+2]<<3); // Добавка 2 - второй АЦП
  tmp&=0x38; // на всякий случай убираем лишние биты маской
  tmp|=(BSI_Settings.BiUniPolar.bit.BU2<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 Отключаем АЦП2

  //-----------------------------------------------
}

//====================================================================
// Чтение из АЦП 2 канал 1
// Возвращает unsigned int - результат в сыром виде

unsigned int ReadData_AD21 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 Включаем АЦП2
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // Читаем старший байт результата

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // Читаем младший байт результата

  ADC_PORT|=(1<<CS2); // CS2=1 Отключаем АЦП2

  //--------------------------------------------

  return (tmpH<<8|tmpL); // Превращение двух байт в слово и сохранение
}


//===============================================================
// Запуск АЦП 2 Канал 2 (четвертый токовый канал)

void StartConversion_AD22 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 Включаем АЦП2
//  delay_us(1);
  
  SPDR=Command7705_Clk+1;
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[1+2]; // Добавка 2 - второй АЦП
  tmp&=0x03;
  tmp|=Command7705_C_Clk;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 Отключаем АЦП2
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS2);  // CS2=0 Включаем АЦП2
//  delay_us(1);
  
  SPDR=Command7705_SetT+1;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[1+2]<<3); // Добавка 2 - второй АЦП
  tmp&=0x38; // на всякий случай убираем лишние биты маской
  tmp|=(BSI_Settings.BiUniPolar.bit.BU3<<B_U);
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS2); // CS2=1 Отключаем АЦП2

  //-----------------------------------------------
}

//====================================================================
// Чтение из АЦП 2 канал 2
// Возвращает unsigned int - результат в сыром виде

unsigned int ReadData_AD22 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS2);  // CS2=0 Включаем АЦП2
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // Читаем старший байт результата

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // Читаем младший байт результата

  ADC_PORT|=(1<<CS2); // CS2=1 Отключаем АЦП2

  //--------------------------------------------

  return (tmpH<<8|tmpL); // Превращение двух байт в слово и запись
}

//===================================================
// Перезапуск преобразования токового канала 3 АЦП2

void Conversion_I3 (void)
{
  Channel_NI34=0;  // Выбор канала 1 (бит=0)
  Reset_ADC2();
  StartConversion_AD21();
}

//===================================================
// Перезапуск преобразования токового канала 4 АЦП2

void Conversion_I4 (void)
{
  Channel_NI34=1;  // Выбор канала 2 (бит=1)
  Reset_ADC2();
  StartConversion_AD22();
}

//=========================================================================
// Вызывается после проверки готовности АЦП по ноге /DRDY.
// Процедура забирает данные из заданного битом NUMKAN.NI34 канала АЦП2,
// производит проверку на допустимые пределы измерения (максимум, минимум),
// фильтрует результат алгоритмом скользящего среднего,
// делает необходимые математические расчеты и устанавливает соотв. флаги
// готовности и/или ошибок.
// На выходе процедура запускает другой канал того же АЦП, чтобы при
// следующем опросе сделать вычисления в другом канале.
// Таким образом при каждом вызове процедуры считываются и вычисляются
// попеременно два канала АЦП2.

void Calc_ADC2 (void)
{
  unsigned int tmp; // Переменная для проверки пределов АЦП
  if (Channel_NI34) // Если бит канала установлен то это второй АЦП
    {
      // ====== Канал 2 of ADC2 ========

      tmp=ReadData_AD22();  // Чтение из АЦП2 канал 2
      BSI_ResultEx.CodeI[1+2]=tmp; // результат берем из ADC_I[] добавка 2 т.к. АЦП2
      ReadyDataADC.DI4=1; // Установка флага "данные готовы" по токовому каналу 4
  
      // В tmp - результат АЦП для последующей проверки пределов
      if ( (BSI_Settings.UPHI[1+2] >= tmp) && (tmp >= BSI_Settings.UPLI[1+2]) ) // проверка на допустимый
                                                // диапазон АЦП (пределы "больше" и "меньше")
        {
          BSI_ResultEx.CodeIAverage[1+2]=MovingAverage_I(tmp,1+2); // Если код АЦП в пределах допуска, то фильтруем скользящим средним
          BSI_ResultEx.BSI_Results.I[1+2]=Matematic_I(1+2);  // И делаем необходимые вычисления по каналу 4
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI4=1;         // Set OK Flag
        }
      else  // иначе (при выходе кода за допустимый предел) при необходиомсти 
            // чистим буфер скользящего среднего
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI4) // Если бит DI4=1 то чистим буфер
            {
              ClearMovingAverage_I(1+2); // Чистим буфер скользящего среднего канал 4
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI4=0;   // Потом сбрасываем бит DI4
        }
    }
  else  // иначе первый АЦП
    {
      // ====== Канал 1 of ADC2 ========
      
      tmp=ReadData_AD21();
      BSI_ResultEx.CodeI[0+2]=tmp; // Для первого канала индекс массива =0, сохраняем результат
      ReadyDataADC.DI3=1; // Установка флага "данные готовы" по каналу 3

      if ( (BSI_Settings.UPHI[0+2] >= tmp) && (tmp >= BSI_Settings.UPLI[0+2]) )
        {
          BSI_ResultEx.CodeIAverage[0+2]=MovingAverage_I(tmp,0+2); // Если код АЦП в пределах допуска, то фильтруем скользящим средним
          BSI_ResultEx.BSI_Results.I[0+2]=Matematic_I(0+2);  // Вычисления тока (индекс ноль)
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI3=1; // Установить флаг ОК
        }
      else
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyI3) // Если бит DI=1 то чистим буфер
            {
              ClearMovingAverage_I(0+2); // Чистим буфер скользящего среднего
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyI3=0;   // Сбрасываем бит DI (данные готовы)
        }
    }

  //----------------
  if (Channel_NI34) // Если был выбран канал 4, то запускаем третий
    {
      Conversion_I3();
    }
  else            // иначе был выбран третий, запускаем канал номер четыре
    {
      Conversion_I4();
    }
}

/*
****************************************************************************
**************                                             *****************
**************                 Третий АЦП                  *****************
**************                                             *****************
****************************************************************************
*/


//===============================================================
// Запуск АЦП 3 Канал 1 (первый температурный канал)

void StartConversion_AD31 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 Включаем АЦП3
//  delay_us(1);
  
  SPDR=Command7705_Clk+0; // Clock Register
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[0+4]; // Добавка 4 - третий АЦП (значения 0...3)
  tmp&=0x03; // на всякий случай убираем лишние биты маской
  tmp|=Command7705_C_Clk; // 0x04
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 Отключаем АЦП3
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS3);  // CS3=0 Включаем АЦП3
//  delay_us(1);
  
  SPDR=Command7705_SetT+0;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[0+4]<<3); // Добавка 4 - третий АЦП
  tmp&=0x38; // на всякий случай убираем лишние биты маской
  tmp|=(BSI_Settings.BiUniPolar.bit.BU4<<B_U); // BU4 - ADC3 channel0
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 Отключаем АЦП3

  //-----------------------------------------------
}

//====================================================================
// Чтение из АЦП 3 канал 1
// Возвращает unsigned int - результат в сыром виде

unsigned int ReadData_AD31 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 Включаем АЦП3
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // Читаем старший байт результата

  SPDR=Command7705_RD_Data+0;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // Читаем младший байт результата

  ADC_PORT|=(1<<CS3); // CS3=1 Отключаем АЦП3

  //--------------------------------------------

  return (tmpH<<8|tmpL); // Превращение двух байт в слово и сохранение
}


//===============================================================
// Запуск АЦП 3 Канал 2 (второй температурный канал)

void StartConversion_AD32 (void)
{
  //-------------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 Включаем АЦП3
//  delay_us(1);
  
  SPDR=Command7705_Clk+1;
  while(!SPSR_SPIF);

  unsigned char tmp;  
  tmp = BSI_Settings.fsADC[1+4]; // Добавка 4 - третий АЦП
  tmp&=0x03;
  tmp|=Command7705_C_Clk;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 Отключаем АЦП3
  delay_us(4);
  
  //-------------------------------------------------  

  ADC_PORT&=~(1<<CS3);  // CS3=0 Включаем АЦП3
//  delay_us(1);
  
  SPDR=Command7705_SetT+1;
  while(!SPSR_SPIF);
  
  tmp=(BSI_Settings.gainADC[1+4]<<3); // Добавка 4 - третий АЦП
  tmp&=0x38; // на всякий случай убираем лишние биты маской
  tmp|=(BSI_Settings.BiUniPolar.bit.BU5<<B_U); // BU5 - ADC3 channel 1
  tmp|=Command7705_C_Set;
  SPDR=tmp;
  while(!SPSR_SPIF);
  
  ADC_PORT|=(1<<CS3); // CS3=1 Отключаем АЦП3

  //-----------------------------------------------
}

//====================================================================
// Чтение из АЦП 3 канал 2
// Возвращает unsigned int - результат в сыром виде

unsigned int ReadData_AD32 (void)
{
  unsigned char tmpL;
  unsigned char tmpH;

  //--------------------------------------------

  ADC_PORT&=~(1<<CS3);  // CS3=0 Включаем АЦП3
//  delay_us(1);
  
  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpH=SPDR;  // Читаем старший байт результата

  SPDR=Command7705_RD_Data+1;
  while(!SPSR_SPIF);

  tmpL=SPDR;  // Читаем младший байт результата

  ADC_PORT|=(1<<CS3); // CS3=1 Отключаем АЦП3

  //--------------------------------------------

  return (tmpH<<8|tmpL); // Превращение двух байт в слово и запись
}

//===================================================
// Перезапуск преобразования температурного канала 1 АЦП3

void Conversion_T1 (void)
{
  Channel_NT12=0;  // Выбор канала 1 (бит=0)
  Reset_ADC3();
  StartConversion_AD31();
}

//===================================================
// Перезапуск преобразования температурного канала 2 АЦП3

void Conversion_T2 (void)
{
  Channel_NT12=1;  // Выбор канала 2 (бит=1)
  Reset_ADC3();
  StartConversion_AD32();
}

//=========================================================================
// Вызывается после проверки готовности АЦП по ноге /DRDY.
// Процедура забирает данные из заданного битом NUMKAN.NT12 канала АЦП3,
// производит проверку на допустимые пределы измерения (максимум, минимум),
// фильтрует результат алгоритмом скользящего среднего,
// делает необходимые математические расчеты и устанавливает соотв. флаги
// готовности и/или ошибок.
// На выходе процедура запускает другой канал того же АЦП, чтобы при
// следующем опросе сделать вычисления в другом канале.
// Таким образом при каждом вызове процедуры считываются и вычисляются
// попеременно два канала АЦП3.

void Calc_ADC3 (void)
{
  unsigned int tmp; // Переменная для проверки пределов АЦП
  if (Channel_NT12) // Если бит канала установлен то это второй АЦП
    {
      // ====== Канал 2 of ADC3 ========

      tmp=ReadData_AD32();  // Чтение из АЦП3 канал 2
//======
//      tmp=34272; // Debug Value
//======
      BSI_ResultEx.CodeT[1]=tmp; // результат берем из tmp
      ReadyDataADC.DT2=1; // Установка флага "данные готовы" по температурному каналу 2
  
      // В tmp - результат АЦП для последующей проверки пределов
      if ( (BSI_Settings.UPHT[1] >= tmp) && (tmp >= BSI_Settings.UPLT[1]) ) // проверка на допустимый диапазон АЦП (пределы "больше" и "меньше")
        {
          BSI_ResultEx.CodeTAverage[1]=MovingAverage_T(tmp,1); // Если код АЦП в пределах допуска, то фильтруем скользящим средним
          BSI_ResultEx.BSI_Results.T[1]=Matematic_RT(1);  // И делаем необходимые вычисления по температурному каналу 2
          if (BSI_ResultEx.BSI_Results.T[1]!=0) // Проверка на 0 (ошибку)
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2=1;         // Set OK Flag
            }
          else
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2=0; // была ошибка переполнения
            }
        }
      else  // иначе (при выходе кода за допустимый предел) при необходиомсти 
            // чистим буфер скользящего среднего
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2) // Если бит DT2=1 то чистим буфер
            {
              ClearMovingAverage_T(1); // Чистим буфер скользящего среднего канал 6
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyT2=0;   // Потом сбрасываем бит DT
        }
    }
  else  // иначе первый АЦП
    {
      // ====== Канал 1 of ADC3 ========
      
      tmp=ReadData_AD31();
      BSI_ResultEx.CodeT[0]=tmp; // Для первого канала индекс массива =0, сохраняем результат
      ReadyDataADC.DT1=1; // Установка флага "данные готовы" по температурному каналу 1

      if ( (BSI_Settings.UPHT[0] >= tmp) && (tmp >= BSI_Settings.UPLT[0]) )
        {
          BSI_ResultEx.CodeTAverage[0]=MovingAverage_T(tmp,0); // Если код АЦП в пределах допуска, то фильтруем скользящим средним
          BSI_ResultEx.BSI_Results.T[0]=Matematic_RT(0);  // Вычисления температуры (индекс ноль)
          if (BSI_ResultEx.BSI_Results.T[0]!=0) // Проверка на 0 (ошибку)
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1=1;         // Set OK Flag
            }
          else
            {
              BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1=0; // была ошибка переполнения
            }
        }
      else
        {
          if (BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1) // Если бит DT1=1 то чистим буфер
            {
              ClearMovingAverage_T(0); // Чистим буфер скользящего среднего
            }
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyT1=0;   // Сбрасываем бит DT (данные готовы)
        }
    }

  //----------------
  if (Channel_NT12)
    {
      Conversion_T1();  // Если был выбран канал 2, то запускаем первый
    }
  else
    {
      Conversion_T2(); // иначе был выбран первый, запускаем канал номер два
    }
}























