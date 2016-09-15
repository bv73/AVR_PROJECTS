#include "matematic.h"
// #include "divide.h"

/*
void ClearSRAM (void)
{
  memset (BUFF,0x00,sizeof(BUFF));
}
*/

//============================================================================
// Процедура вычисления температуры по сопротивлению (100П & 50П)
// На входе: unsigned long - сопротивление, для 50-омного ТСП уже удвоенное (х)
// На выходе: unsigned int - температура. Если =0 то ошибка.
// (Proc_TSP0)

unsigned int Pt_3910 (unsigned long resistance)
{
  unsigned long result;
  result = (resistance * 29153UL) / 8UL;
  result = (result + 2206179461UL) / 65536UL;
  result = (result * resistance) /  64UL;
  result = (result + 630046652UL) / 16384UL;
  result = (result * resistance);
  result = (result + 197216040UL) / 65536UL;
  return (unsigned int) result;
}

//============================================================================
// Процедура вычисления температуры по сопротивлению (100М)
// На входе: unsigned long - сопротивление, для 50-омного ТСП уже удвоенное (х)
// На выходе: unsigned int - температура. Если =0 то ошибка
// (Proc_TSP1)

unsigned int Cu_4280 (unsigned long resistance)
{
 
  unsigned long result;

  if (resistance >= 38289UL) // ok
    {
      result = (76599UL * resistance) / 131072UL;
      result = (result + 3939UL);
      return (unsigned int) result;
    }

  if (resistance < 38285UL)
    {
      result = (resistance * 1815UL) / 65536UL;
      result = (result + 36033UL);
      result = (result * resistance) / 65536UL;
      result = (result + 4647UL);
      return (unsigned int) result;
    }

  return (26315); // Код 38285...38288 263.15 градусов (-10 градусов Цельсия)
}


//==========================================================================
// Процедура вычисления температуры по сопротивлению (Pt100 & Pt50)
// На входе: unsigned long - сопротивление, для 50-омного ТСП уже удвоенное (х)
// На выходе: unsigned int - температура. Если =0 то ошибка.
// (Proc_TSP2)

unsigned int Pt_3850 (unsigned long resistance)
{
  unsigned long result;
  result = (resistance * 57080UL) / 4UL;
  result = (result + 902791633UL) / 65536UL;
  result = (result * resistance) /  64UL;
  result = (result + 653227277UL) / 16384UL;
  result = (result * resistance + 161076900UL) / 65536UL;
  return (unsigned int) result;
}

//==========================================================================
// Процедура вычисления температуры по сопротивлению (Cu100 & Cu50)
// На входе: unsigned long - сопротивление, для 50-омного ТСП уже удвоенное (х)
// На выходе: unsigned int - температура. Если =0 то ошибка.
// (Proc_TSP3)

unsigned int Cu_4260 (unsigned long resistance)
{
  unsigned long result;
  result = (resistance * 38446UL) / 65536UL;
  result = (result + 3850UL);
  return (unsigned int) result;
}


//=====================================================================
// Процедура вычисления скользящего среднего по каналу тока
// unsigned int InputData - входные данные АЦП
// unsigned char channel - канал 0...3
// volatile unsigned char AverageKI[] - по скольки значениям усредняем (0...15)
// Внутренние переменные:
// volatile unsigned char AverageIndexI[] - сколько значений записано в буфере на данный момент
// volatile unsigned char BufFullI[] - флаг-указатель заполненности буфера
// На выходе - unsigned int, среднее значение

unsigned int MovingAverage_I (unsigned int InputData, unsigned char channel)
{

  if (BSI_Settings.AverageKI[channel]==0 || BSI_Settings.AverageKI[channel]==1)
    {
      return(InputData);  // Если количество элементов усреднения = 0 или 1 то сразу отдаем входные данные
    }

  unsigned char index=AverageIndexI[channel]; // Index - указатель на текущее значение
  unsigned long dividend=AverageSumI[channel]; // Загружаем в делимое сумму
  unsigned long divisor=AverageIndexI[channel]; // На сколько делим
  divisor++;

  if (AverageBufFullI[channel]) // Если буфер полон, то сначала вычитаем старое значение
    {
      divisor = BSI_Settings.AverageKI[channel]; // Делитель равен максимальному значению кол-ва элементов
      dividend = dividend - AverageBufI[channel][index]; // Вычитаем старое значение
    }

  AverageBufI[channel][index] = InputData; // Сохраняем
  dividend = dividend + InputData;
  AverageSumI[channel] = dividend;

  unsigned long result = (dividend / divisor); // В лонге работаем только с лонгом
  
  AverageIndexI[channel]++;
  
  unsigned char bb = BSI_Settings.AverageKI[channel] & 0x0F; // Если в установках было ошибочно большое значение, то отсеивает
  if ( AverageIndexI[channel] >= bb ) // Проверка, заполнился ли буфер
    {
      AverageIndexI[channel] = 0; // Сбрасываем указатель
      AverageBufFullI[channel] = 0xFF; // Устанавливаем флаг о том, что буфер заполнен
    }

  return (unsigned int) result;

}


//=============================================================================
// Процедура вычисления скользящего среднего по каналу температуры
// unsigned int InputData - входные данные АЦП
// unsigned char channel - канал 0...1
// volatile unsigned char AverageKT[] - по скольки значениям усредняем (0...15)
// Внутренние переменные:
// volatile unsigned char AverageIndexT[] - сколько значений записано в буфере на данный момент
// volatile unsigned char BufFullT[] - флаг-указатель заполненности буфера
// На выходе - unsigned int, среднее значение

unsigned int MovingAverage_T (unsigned int InputData, unsigned char channel)
{

  if (BSI_Settings.AverageKT[channel]==0 || BSI_Settings.AverageKT[channel]==1)
    {
      return(InputData);  // Если количество элементов усреднения = 0 или 1 то сразу отдаем входные данные
    }

  unsigned char index = AverageIndexT[channel]; // Index - указатель на текущее значение
  unsigned long dividend = AverageSumT[channel]; // Загружаем в делимое сумму
  unsigned long divisor = AverageIndexT[channel]; // На сколько делим
  divisor++;

  if (AverageBufFullT[channel]) // Если буфер полон, то сначала вычитаем старое значение
    {
      divisor=BSI_Settings.AverageKT[channel]; // Делитель равен максимальному значению кол-ва элементов
      dividend = dividend - AverageBufT[channel][index];
    }

  AverageBufT[channel][index] = InputData; // Сохраняем
  dividend = dividend + InputData;
  AverageSumT[channel] = dividend;

  unsigned long result = (dividend / divisor);

  AverageIndexT[channel]++;
  AverageIndexT[channel]&=0x0F; // Ограничиваем индекс не более 15

  unsigned char bb = BSI_Settings.AverageKT[channel] & 0x0F;
  if ( AverageIndexT[channel] >= bb ) // Проверка, заполнился ли буфер
    {
      AverageIndexT[channel] = 0; // Сбрасываем указатель
      AverageBufFullT[channel] = 0xFF; // Устанавливаем флаг о том, что буфер заполнен
    }

  return (unsigned int) result;
  
}


//========================================================================
// Инициализация переменных для скользящего среднего, очистка буферов
// и необходимых переменных. Канал I.
// Перед вызовом процедур MovingAvegage необходимо очистить некоторые
// переменные.

void InitMovingAverage_I (void)
{
  // Очистка двумерного массива значений усреднения
  // 16*6=96 слов AverageBuf[BSI_N_ICHANNEL+BSI_N_TCHANNEL][MAX_AVERAGE_WORDS];

  unsigned char i;
  for (i=0;i<MAX_AVERAGE_WORDS;i++)
    {
      AverageBufI[0][i]=0;
      AverageBufI[1][i]=0;
      AverageBufI[2][i]=0;
      AverageBufI[3][i]=0;      
    }

  for (i=0;i<(BSI_N_ICHANNEL);i++)
    {
      AverageIndexI[i]=0;   // Начальное кол-во элементов в буфере = 0
      AverageSumI[i]=0;  // Сумма по каналу изначально = 0
      AverageBufFullI[i]=0;     // Флаг заполнения буфера = 0
    }

}

//========================================================================
// Инициализация переменных для скользящего среднего, очистка буферов
// и необходимых переменных. Канал T.
// Перед вызовом процедур MovingAvegage необходимо очистить некоторые
// переменные.

void InitMovingAverage_T (void)
{
  // Очистка двумерного массива значений усреднения
  // 16*6=96 слов AverageBuf[BSI_N_ICHANNEL+BSI_N_TCHANNEL][MAX_AVERAGE_WORDS];

  unsigned char i;
  for (i=0;i<MAX_AVERAGE_WORDS;i++)
    {
      AverageBufT[0][i]=0;
      AverageBufT[1][i]=0;
    }

  for (i=0;i<(BSI_N_TCHANNEL);i++)
    {
      AverageIndexT[i]=0;   // Начальное кол-во элементов в буфере = 0
      AverageSumT[i]=0;  // Сумма по каналу изначально = 0
      AverageBufFullT[i]=0;     // Флаг заполнения буфера = 0
    }

}


//=====================================================================
// Очистка и инициализация переменных процедуры скользящего среднего
// для конкретного токового канала.
// unsigned char channel - обнуляемый канал 0...3

void ClearMovingAverage_I (unsigned char channel)
{
  AverageIndexI[channel]=0;           // Начальное кол-во элементов в буфере = 0
  AverageSumI[channel]=0;  // Сумма по каналу изначально = 0
  AverageBufFullI[channel]=0;     // Флаг заполнения буфера = 0

  unsigned char i;
  for (i=0;i<(MAX_AVERAGE_WORDS);i++)
    {  
      AverageBufI[channel][i];
    }

}

//=====================================================================
// Очистка и инициализация переменных процедуры скользящего среднего
// для конкретного температурного канала.
// unsigned char channel - обнуляемый канал 0...1

void ClearMovingAverage_T (unsigned char channel)
{
  AverageIndexT[channel]=0;           // Начальное кол-во элементов в буфере = 0
  AverageSumT[channel]=0;  // Сумма по каналу изначально = 0
  AverageBufFullT[channel]=0;     // Флаг заполнения буфера = 0

  unsigned char i;
  for (i=0;i<(MAX_AVERAGE_WORDS);i++)
    {
      AverageBufT[channel][i];
    }
}

//=====================================================================
// Температурная компенсация по одному из токовых каналов
// На входе: unsigned char channel - номер канала, по которому делается термокомпенсация
// 0...3 - токовые каналы
// На выходе: вычисленный результат unsigned int

unsigned int Temperature_Compensation_I (unsigned char channel)
{
  signed char Tk;
  if (BSI_ResultEx.BSI_Results.Status.Flag.signTintern==1) // Если знак термокомпенсации установлен, то
    {
      Tk=BSI_Settings.KHI[channel]; // Выбираем коэффициент Kh (20...60 градусов)
    }
  else
    {
      Tk=BSI_Settings.KLI[channel]; // Иначе выбираем коэффициент Kl (-40...20 градусов)
    }

  unsigned long ads=BSI_ResultEx.CodeIAverage[channel];
  unsigned long result;
  result = ads * TerIn_2dT; // Сначала среднее умножаем на удвоенную дельту Т
  result = (result / 65536);
  result = (result * Tk);
  result = (result / 64);
  result = (result + ads);  // Окончательный результат вычисляем с учетом выбранного Tk
  return (unsigned int) result;
}

//=====================================================================
// Температурная компенсация по одному из температурных каналов
// На входе: unsigned char channel - номер канала, по которому делается термокомпенсация
// 0...1 - темературные каналы
// На выходе: вычисленный результат unsigned int

unsigned int Temperature_Compensation_T (unsigned char channel)
{
  signed char Tk;
  if (BSI_ResultEx.BSI_Results.Status.Flag.signTintern==1) // Если знак термокомпенсации установлен, то
    {
      Tk=BSI_Settings.KHT[channel]; // Выбираем коэффициент Kh (20...60 градусов)
    }
  else
    {
      Tk=BSI_Settings.KLT[channel]; // Иначе выбираем коэффициент Kl (-40...20 градусов)
    }

  unsigned long ads=BSI_ResultEx.CodeTAverage[channel];
  unsigned long result; // Результат 32-битный

  result = ads * TerIn_2dT; // Сначала среднее умножаем на удвоенную дельту Т
  result = (result / 65536);  // Окончательный результат вычисляем с учетом выбранного Tk
  result = (result * Tk);
  result = (result / 64);
  result = (result + ads);
  return (unsigned int) result; // Результат на выходе приводится к 16 битам
}


//======================================================================
// Производит расчет токового канала
// На входе - unsigned char channel 0..3
// Данные возвращаются как unsigned int

unsigned int Matematic_I (unsigned char channel)
{
  unsigned int corrected_ADC_value=Temperature_Compensation_I(channel);

  unsigned long result;
  unsigned long a=BSI_Settings.AI[channel];
  signed char b=BSI_Settings.BI[channel];
  result = (corrected_ADC_value + b);  // Результат 32-битный
  result = (result * a); // При умножении/делении работаем с лонгом
  result = (result / 65536UL);
  return (unsigned int) result;
}


//=================================================
// Расчет сопротивления R и Т (температуры)

unsigned int Matematic_RT (unsigned char channel)
{
//===============================================
// Производит расчет температурного канала
// На входе - unsigned char channel 0..1
// Данные возвращаются как unsigned int
// === MATEMATIC_R === Вычисление сопротивления R

  unsigned long corrected_ADC_value=Temperature_Compensation_T(channel);
  unsigned long a=BSI_Settings.AT[channel];
  signed char b=BSI_Settings.BT[channel];
  unsigned long result = (corrected_ADC_value * 65536UL) / a; // Результат 32-битный
  
  unsigned int  resultR = (unsigned int)result + b;
  
  BSI_ResultEx.R[channel] = resultR; // Сохраняем результат вычисленного сопротивления

//=========== Matematic_TSP ====================

  unsigned char tsp=BSI_Settings.typeT[channel]; // Тип термопроеобразователя
  
  if (tsp&0x04) // Если старший бит типа ТСП = 1 то 50-омный термопреобразователь
    {
      if (resultR >= 32768) // Если было переполнение то возвращаем 0 - ошибка (умножать на 2 не будем)
        {
          return (0);
        }
      else  // иначе удваиваем
        {
          resultR = resultR * 2; 
        }
    }
  
  switch (tsp&0x03) // Оставляем только типы 0,1,2,3
    {
      case TSENS_100P: // TSP0 100П 1.3910 (TSP0+4 50П 1.3910)
        {
          return Pt_3910(resultR);
        }
      case TSENS_100M: // TSP1 100М 1.4280 (TSP1+4 50М 1.4280)
        {
          return Cu_4280(resultR);
        }
      case TSENS_Pt100: // TSP2 Pt100 1.3850 (TSP2+4 Pt50 1.3850)
        {
          return Pt_3850(resultR);
        }
      case TSENS_Cu100: // TSP3 Cu100 1.4260 (TSP3+4 Cu50 1.4260)
        {
          return Cu_4260(resultR);
        }
    }
return 0;
}

//********************************************************************************************************************************************************
//***********                Z channel            ********************************************************************************************************
//********************************************************************************************************************************************************

//==========================================================================
// Обработка временного канала.
// Проверка на минимум-максимум периода и передача значения результата

void Calc_Z1(void)
{
  unsigned long result;
  if (ICR1A >= ICR1B) // >=
    {
      result = (ICR1A - ICR1B); // Результат равен периоду в мкс, умноженое на 8
    }
  else
    {
      result = (ICR1B - ICR1A);
    }

  if (result < (BSI_Settings.ZTime[0].min<<3)) // проверка Tmin1*8
    {
      if (nError1) // Если не достигло нуля, то
        {
          nError1--;  // счетчик ошибок уменьшаем и выходим
          return;
        }
      else // Brk1==0
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin1=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin1=1; // Установка флага Tmin1 OK

  if (result >= (BSI_Settings.ZTime[0].max<<3)) // проверка Tmax1*8
    {
      if (nError1!=0)
        {
          nError1--;
          return;
        }
      else
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax1=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Z_Tus[0] = result;
  
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax1=1;

  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=1;
  
  nError1 = BSI_Settings.nZ_errors_max[0];
}

//============================================================================

void Calc_Z2(void)
{
  unsigned long result;
  if (ICR2A >= ICR2B)
    {
      result = (ICR2A - ICR2B);
    }
  else
    {
      result = (ICR2B - ICR2A);
    }

  if (result < (BSI_Settings.ZTime[1].min<<3) ) // проверка Tmin2
    {
      if (nError2!=0)
        {
          nError2--;
          return;
        }
      else
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin2=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmin2=1;

  if (result >= (BSI_Settings.ZTime[1].max<<3) ) // проверка Tmax1
    {
      if (nError2!=0)
        {
          nError2--;
          return;
        }
      else
        {
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax2=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=0;
          return;
        }
    }

  BSI_ResultEx.BSI_Results.Z_Tus[1] = result;
  
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZmax2=1;
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=1;

  
  nError2 = BSI_Settings.nZ_errors_max[1];
}

//=================================================================================
// Установка прескалера сторожевой собаки
// prescaler - 0...7
// Watchdog timer по умолчанию всегда включен, после установки прескалера нужно
// всё время давать собаке покушать, чтобы она нас не съела.
// 111 = 2,1  с
// 110 = 1,0  с 
// 101 = 0,52 с 
// 100 = 0,26 c
// 011 = 0,13 с 

__monitor void WDT_Prescaler_Change(unsigned char prescaler)
{
  __watchdog_reset();
  // Start timed equence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // Set new prescaler(time-out) value
  WDTCR = (1<<WDE) | prescaler;
}

//===================================================================
// Проверка целостности конфига путём подсчета КС всего тела конфига
// вместе с КС в конце. Результат должен быть ноль.

unsigned int CheckConfigCRC (void)
{
  return CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings));
}