
// ===================== EEPROM Functions & Procedures ===============================

// Статус прочитанной конфигурации из EEPROM хранится в BSI_ResultEx.ArchiveStatus
// =00 - основная копия в EEPROM,
// =01 - вторая копия в EEPROM, 
// =02 - резервная копия из памяти программ.
// Алгоритм такой - читаем из EEPROM основную копию системных настроек, проверяем КС.
// Если КС не совпала, то считаем что архив битый и читаем из второй копии, проверяем КС.
// Если КС опять не совпала, то считаем что EEPROM убита/полуживая и читаем копию из памяти программ,
// проверяем КС. Перед каждой процедурой подсчета КС устанавливаем байт статуса (0,1 или 2).
// Этим мы сигнализируем о том, какие копии битые.

//__root __eeprom unsigned int OFFSET_ADDR @0x1FC = (unsigned int)&BSI_ResultEx.BSI_Results.Status.word;
//__root __eeprom sBSI_Settings EEP_Settings@ 0x180 ={

// 0x3FFF - адрес последнего байта памяти программ
__root __flash unsigned int ChekSum_EEPROM @ 0x3FFE = 0x55AA;
__root __flash char Version[] @ 0x3FF0 = "BSI_Ver1.0";

// Системные переменные и константы по умолчанию
__root __flash sBSI_Settings PGM_Settings@ PGM_SETTINGS_ADDR ={
  .Dummy=0, // Dummy
  .on_off.bit.I1=1, .on_off.bit.I2=1, .on_off.bit.I3=1, .on_off.bit.I4=1, .on_off.bit.T1=1, .on_off.bit.T2=1, .on_off.bit.Tint=1, // Enable channels
  .AverageKI[0]=AVERAGE_FACTOR_I,
  .AverageKI[1]=AVERAGE_FACTOR_I,
  .AverageKI[2]=AVERAGE_FACTOR_I,
  .AverageKI[3]=AVERAGE_FACTOR_I,
  .AverageKT[0]=AVERAGE_FACTOR_T,
  .AverageKT[1]=AVERAGE_FACTOR_T,
  .typeT[0]=1,
  .typeT[1]=1,
  .AI[0]=50174,
  .AI[1]=50193,
  .AI[2]=50194,
  .AI[3]=50181,
  .AT[0]=54932,
  .AT[1]=54917,
  .BI[0]=1,
  .BI[1]=1,
  .BI[2]=0,
  .BI[3]=0,
  .BT[0]=-14,
  .BT[1]=-27,
  .KHI[0]=0,
  .KHI[1]=0,
  .KHI[2]=0,
  .KHI[3]=0,
  .KHT[0]=0,
  .KHT[1]=0,
  .KLI[0]=0,
  .KLI[1]=0,
  .KLI[2]=0,
  .KLI[3]=0,
  .KLT[0]=0,
  .KLT[1]=0,
  .UPHI[0]=64928,
  .UPHI[1]=64928,
  .UPHI[2]=64928,
  .UPHI[3]=64928,
  .UPHT[0]=52480,
  .UPHT[1]=52480,
  .UPLI[0]=608,
  .UPLI[1]=608,
  .UPLI[2]=608,
  .UPLI[3]=608,
  .UPLT[0]=608,
  .UPLT[1]=608,
  .gainADC[0]=1,
  .gainADC[1]=1,
  .gainADC[2]=1,
  .gainADC[3]=1,    
  .gainADC[4]=6,
  .gainADC[5]=6,
  .fsADC[0]=0,
  .fsADC[1]=0,
  .fsADC[2]=0,
  .fsADC[3]=0,
  .fsADC[4]=0,
  .fsADC[5]=0,
  .BiUniPolar.bit.BU0=1, .BiUniPolar.bit.BU1=1, .BiUniPolar.bit.BU2=1, .BiUniPolar.bit.BU3=1, .BiUniPolar.bit.BU4=1, .BiUniPolar.bit.BU5=1, 
  .FICP.bit.Front1=0, .FICP.bit.Filter1=0, .FICP.bit.Front2=0, .FICP.bit.Filter2=0,
  .nZ_errors_max[0]=2,
  .nZ_errors_max[1]=2,
  .ZTime[0].max=1000000, // 0,1 Гц
  .ZTime[0].min=1000, // 1000 Гц
  .ZTime[1].max=1000000,
  .ZTime[1].min=1000,
  .ZTimeMeasure[0]=53255,
  .ZTimeMeasure[1]=53255,
  .CheckSum=0x1234  // Обязательная контрольная сумма конфигурации (здесь записана отфонарно, при копировании из PGM в EEPROM считается правильно)
};

// Конфиг в SRAM хранится по постоянному абсолютному адресу &BSI_Settings.
// Для работы с несколькими копиями конфигурации в EEPROM,
// достаточно лишь указать начальный адрес конфигурации в EEPROM.
// Длина конфигурации ~ 100 байт, плюс контрольная сумма, выравниваем до 128 байт.
// Адреса конфигурации в EEPROM будут 0x00,0x80,0x100,0x180.
// В EEPROM можно сохранить до четырех копий конфигурации (при необходимости).

//=============================================================================
// Процедура записывает в EEPROM конфигурацию из SRAM. Считает КС.
// В последних двух байтах конфигурации должна быть посчитанная контрольная сумма.
// Чтобы запись не похерилась новыми данными, вставляем функцию __monitor для отключения
// прерываний на время записи.

__monitor void Write_Config_To_EEPROM (unsigned int addr)
{
  
  unsigned char c;
  unsigned char *p=(unsigned char *)(unsigned int)&BSI_Settings;
  unsigned char __eeprom *zp=(unsigned char __eeprom *)addr;
  unsigned char len=sizeof(BSI_Settings);
  while (len--)
    {
      __watchdog_reset();
      PORTD^=(1<<WDI); // Внешний watchdog
      c=*p++;
      *zp++=c;
    };
}

// Процедура читает из EEPROM конфигурацию в SRAM
// таким образом происходит инициализация системных переменных
// и констант прибора. Последние два байта - контрольная сумма.
// Возвращает посчитаную КС прочитанной конфигурации:
// Если =0 то КС в порядке, иначе - конфиг в EEPROM с ошибкой.

__monitor void Read_Config_From_EEPROM (unsigned int addr)
{
  unsigned char c;
  unsigned char *p=(unsigned char *)(unsigned int)&BSI_Settings;
  unsigned char __eeprom *zp=(unsigned char __eeprom *)addr;
  unsigned char len=sizeof(BSI_Settings);
  while (len--)
    {
      __watchdog_reset();
      PORTD^=(1<<WDI); // Внешний watchdog
      c=*zp++;
      *p++=c;
    };
  
}

// Процедура читает из памяти программ конфигурацию в SRAM.
// Последние два байта - контрольная сумма, не учитываются.
// addr фактически равен 0x3F80 (можно и не передавать процедуре).

__monitor void Read_Config_From_PGM (unsigned int addr)
{
  unsigned char c;
  unsigned char *p=(unsigned char *)(unsigned int)&BSI_Settings;
  unsigned char __flash *zp=(unsigned char __flash *)addr;
  unsigned char len=sizeof(BSI_Settings);
  while (len--)
    {
      __watchdog_reset();
      PORTD^=(1<<WDI); // Внешний watchdog
      c=*zp++;
      *p++=c;
    };
}

/*
// Инициализация констант в памяти
// Использовалась до тех пор, пока не была написана инициализация
// констант из EEPROM.

void InitSettings (void)
{
  
  unsigned char i;
  for (i=0;i<(BSI_N_ICHANNEL);i++)
    {
      BSI_Settings.UPLI[i]=608;  // Пределы АЦП, нижний предел.
      BSI_Settings.UPHI[i]=64928; // Верхний предел значений АЦП.
      BSI_Settings.gainADC[i]=1; // Gain=1 для АЦП.
      BSI_Settings.fsADC[i]=0; // Частота опроса АЦП.
    }
  for (i=0;i<(BSI_N_TCHANNEL);i++)
    {
      BSI_Settings.UPLT[i]=608;
      BSI_Settings.UPHT[i]=52480;
      BSI_Settings.gainADC[i+4]=6;  // Температурные каналы - гейн =6
      BSI_Settings.fsADC[i+4]=0;
    }

  // Токовые константы
  BSI_Settings.AI[0]=50174;
  BSI_Settings.AI[1]=50193;  // Задание конкретных коэффициентов для 1 и 2 токового канала
  BSI_Settings.AI[2]=50194;
  BSI_Settings.AI[3]=50181;
  BSI_Settings.BI[0]=1;
  BSI_Settings.BI[1]=1;
  BSI_Settings.BI[2]=0;
  BSI_Settings.BI[3]=0;
  BSI_Settings.KLI[0]=BSI_Settings.KLI[1]=0;
  BSI_Settings.KLI[2]=BSI_Settings.KLI[3]=0;
  BSI_Settings.KHI[0]=BSI_Settings.KHI[1]=0;
  BSI_Settings.KHI[2]=BSI_Settings.KHI[3]=0;

  // Температурные константы
  BSI_Settings.AT[0]=54932;
  BSI_Settings.AT[1]=54917;  
  BSI_Settings.BT[0]=-14; // Было -14
  BSI_Settings.BT[1]=-27; // Было -27
  BSI_Settings.KLT[0]=BSI_Settings.KLT[1]=0;
  BSI_Settings.KHT[0]=BSI_Settings.KHT[1]=0;
  
  BSI_Settings.typeT[0]=0;
  BSI_Settings.typeT[1]=0;

  // Временные констатны
  BSI_Settings.ZTime[0].min=1000;     // Tmin
  BSI_Settings.ZTime[0].max=1000000;  // Tmax
  BSI_Settings.ZTime[1].min=8000;
  BSI_Settings.ZTime[1].max=8000000;
  BSI_Settings.ZTimeMeasure[0]=53255; //TPZ
  BSI_Settings.ZTimeMeasure[1]=53255;
  BSI_Settings.nZ_errors_max[0]=2; // nErr max
  BSI_Settings.nZ_errors_max[1]=2;
  BSI_Settings.FICP.bit.Filter1=0;
  BSI_Settings.FICP.bit.Front1=0;
  BSI_Settings.FICP.bit.Filter2=0;
  BSI_Settings.FICP.bit.Front2=0;

  BSI_Settings.BiUniPolar.byte=0xFF; // All Bipolar
  BSI_Settings.on_off.byte=0xFF; // Включаем все каналы

}
*/

//  BSI_ResultEx.BSI_Results.I[0]=1;
//  BSI_ResultEx.CodeI[0]=0;
//  BSI_ResultEx.BSI_Results.T[0]=0;
//  BSI_Settings.ZTimeMeasure[0]=0;
//  BSI_Settings.ZTime[0].min=0;
//  BSI_Settings.BiUniPolar.bit.BU0=0;
//  BSI_Settings.on_off.bit.Tint=0; // Биты CONFIG
//  BSI_Data.Byte=1;
//  BSI_Results.Status.Flag.LevelZ2=0;
//  BSI_Results.T[0]=0;
//  BSI_Results.Tin




//=============================================================================================
// Процедура читает конфигурацию прибора из EEPROM в SRAM.
// Если КС не совпала в основной копии конфига, то читается вторая резервная копия
// и если КС совпала, то восстанавливается основная копия конфига.
// Если КС резервной копии конфига не совпала, то читается конфиг по умолчанию из
// памяти программ и восстанавливаются основная и резервная копии конфига.
// Алгоритм работы с чтением копий конфигураций примерно следующий:
// 1. Читается основная копия конфигурации, считается КС.
// 2. Если КС не совпало, то читаем вторую копию конфига, считаем КС.
// 3. Если КС не совпало, то читаем аварийный конфиг из памяти программ
// (этим и занимается эта процедура).
// Если программа дошла до этой процедуры, то вариантов может быть несколько -
// прибор только что прошили новой прошивкой или стерли EEPROM. Или во время
// записи в EEPROM был сбой по питанию.
// В алгоритме программы нужно предусмотреть копирование конфига по умолчанию
// в EEPROM с контрольной суммой, если копия разрушена (не совпала КС).

void Read_Settings (void)
{
//  unsigned int CheckSum;

  Read_Config_From_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // Читаем основной конфиг
  // После прочтения конфигурации в SRAM, нужно посчитать КС всего блока (без учета последних двух байт КС)
//  CheckSum = CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings)-2 );

  if (CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings) ) == 0) // Если КС прочитанного блока и его записаный КС совпали, то
    {
//      Write_Config_To_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // записываем живой конфиг на всякий случай во вторую копию
      BSI_ResultEx.ArchiveStatus=0; // Статус=0 - OK (работаем с основной копией конфига)
      return;
    }

// Если основной конфиг похерился, то
  Read_Config_From_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // Читаем второй конфиг
//  CheckSum = CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings)-2 ); // Считаем КС прочитанного

  if (CRC16((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings) ) == 0)
    {
      Write_Config_To_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // записываем живой конфиг в первую копию
      BSI_ResultEx.ArchiveStatus=1; // Статус=1 (работаем с резервной копией конфига)
      return;
    }

  // Если резервный конфиг похерился, то в аварийном режиме копируем конфиг по умолчанию из памяти программ
  Read_Config_From_PGM (PGM_SETTINGS_ADDR); // Читаем конфиг из памяти программ. Заменяет процедуру InitSettings()
  CRC16_And_Store((unsigned char *)(unsigned int)&BSI_Settings, (sizeof(BSI_Settings))-2 ); // Считаем КС с сохранением в конце


  // После восстановления и подсчета КС, пытаемся восстановить основной и резервный конфиги

  Write_Config_To_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // записываем конфиг в первую копию  
  Write_Config_To_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // записываем конфиг в резервную копию

  BSI_ResultEx.ArchiveStatus=3; // Статус=3 (Аварийный режим конфига - работаем с копией конфига по умолчанию)

}


