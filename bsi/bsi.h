// By (R)soft 1.6.2011

typedef unsigned char u08;
typedef signed char s08;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef unsigned long long u64;


#define BVL(bit)	(1LU<<(bit))
#define sbi(reg,bit)	(reg)|=BVL((bit))
#define cbi(reg,bit)	(reg)&=~BVL((bit))

#define TRUE  0xFF
#define FALSE 0x00

#define delay_us(V) __delay_cycles(V*8L)
void delay_ms (int c)
{
  do
  {
    delay_us (1000); // 1 ms
  }
  while (--c);
}

//==========================================================================
// Port Definitions

// ==== PORT A ====
#define LED_PORT    PORTA
#define LED_DIR     DDRA
                    // PA0  nc
                    // PA1  nc
                    // PA2  nc
                    // PA3  nc
                    // PA4  nc
                    // PA5  nc
#define LED_CLC     PA6   // LED1 of CYCLE 0=On 1=OFF
#define LED_TX      PA7   // LED2 of TX Data over UART

// ==== PORT B ====
#define I2C_PORT    PORTB
#define I2C_DIR     DDRB
#define I2C_PIN     PINB

#define SCL   PB0   // PB0  SCL of I2C (Output)
#define SDA   PB1   // PB1  SDA of I2C(Bidirectional)
#define TPW   PB2   // PB2  TPW (Thermo DS1621 Power) (Output) 1=POWER ON
#define WP    PB3   // PB3  WP (Write Protect Switch Input) 0=WP Enable

                    // PB4  nc
#define MOSI  PB5   // PB5  MOSI of SPI interface (Output)
#define MISO  PB6   // PB6  MISO of SPI interface (Input)
#define SCK   PB7   // PB7  SCK of SPI interface (Output)

// ==== PORT C ====
#define ADC_PORT  PORTC
#define ADC_DIR   DDRC
#define CS1       PC0   // PC0  CS1 of First AD7705 ADC (Output) 0=CS (Active LOW)
#define CS2       PC1   // PC1  CS2 of Second AD7705 ADC (Output)
#define CS3       PC2   // PC2  CS3 of Third AD7705 ADC (Output)
#define RST1      PC3   // PC3  RST1 of First AD7705 ADC (Output) 0=RST (Active LOW)
#define RST2      PC4   // PC4  RST2 of Second AD7705 ADC (Output)
#define RST3      PC5   // PC5  RST3 of Third AD7705 ADC (Output)
                        // PC6  nc
                        // PC7  nc

// ==== PORT D ====
#define RXD   PD0   // PD0  RXD of Modbus Interface (Input)
#define TXD   PD1   // PD1  TXD of Modbus Interface (Output)
#define WDI   PD2   // PD2  Типа Watchdog Timer внешний
#define CAP2  PD3   // PD3  CAP2 - CAPTURE1 Input (ICP3)
                    // PD4  nc
#define DRDY1 PD5   // PD5  DRDY1 of First AD7705 ADC (Input)
#define DRDY2 PD6   // PD6  DRDY2 of Second AD7705 ADC (Input)
#define DRDY3 PD7   // PD7  DRDY3 of Third AD7705 ADC (Input)

// ==== PORT E ====
#define CAP1  PE0   // PE0  CAP1 - CAPTURE0 Input (ICP1)
#define ADDR  PE1   // PE1  ADDR of Device (Input)
                    // PE2  nc

//========================================================================
//========================================================================
//========================================================================

#define TK_OFFSET	273.15f
#define BSI_QUARTZ      (8000000)
#define SEK_CONST       (977)     // Значение счетчика для секундного таймера
#define WDT_CONST       (SEK_CONST/20)     // Значение счетчика сторожевого таймера
#define BSI_N_BOARDS    (2)       // Количество приборов БСИ в системе

#define BSI_N_ICHANNEL  (4)       // Кол-во токовых каналов прибора
#define BSI_N_TCHANNEL  (2)       // Кол-во температурных каналов прибора
#define BSI_N_ZCHANNEL  (2)       // Кол-во временных каналов прибора

// === Описание канала измерения температуры ===
// Если старший (четвертый) бит установлен, то НЕ умножаем на 2
//#define N_TSENS_TYPES (8)         // Кол-во типов температурных первичных преобразователей прибора

#define TSENS_100P    (0) // 100П Proc_TSP0
#define TSENS_100M    (1) // 100М Proc_TSP1
#define TSENS_Pt100   (2) // Pt100 Proc_TSP2
#define TSENS_Cu100   (3) // Cu100 Proc_TSP3

#define TSENS_50P     (4) // 50П Proc_TSP0
#define TSENS_50M     (5) // 50М Proc_TSP1
#define TSENS_Pt50    (6) // Pt50 Proc_TSP2
#define TSENS_Cu50    (7) // Cu50 Proc_TSP3

#define LO8(var) ((unsigned char)((var)&0xFF))
#define HI8(var) ((unsigned char)((var>>8)&0xFF))

typedef struct
{
  union
  {
    u16 word;
    struct
    { // RSF1
      unsigned char ReadyZ1us:    1;
      unsigned char ReadyZ2us:    1;
      unsigned char LevelZ1:      1;  // Логический уровень (состояние ноги) входа Z1
      unsigned char LevelZ2:      1;
      unsigned char ReadyZmax1:   1;
      unsigned char ReadyZmin1:   1;
      unsigned char ReadyZmax2:   1;
      unsigned char ReadyZmin2:   1;
      // RSF0
      unsigned char ReadyI1:      1;  // Готовность первого токового канала: 1-готов
      unsigned char ReadyI2:      1;
      unsigned char ReadyI3:      1;
      unsigned char ReadyI4:      1;
      unsigned char ReadyT1:      1;
      unsigned char ReadyT2:      1;
      unsigned char ReadyTintern: 1; // Готовность внутреннего термометра
      unsigned char signTintern:  1; // Знак встроенного термометра (относительно 20 градусов)
    }Flag;
  }Status;
u16 I[BSI_N_ICHANNEL];  // ток 0,5 мкА на дискрету
u16 T[BSI_N_TCHANNEL];  // t в Кельвинах, в сотых долях (237,15)
u16 Tin;                // t встроенного термометра в сотых долях градуса Цельсия
u32 Z_Tus[BSI_N_ZCHANNEL];  // Период следования импульса в мкс
u32 Z_cnt[BSI_N_ZCHANNEL];  // Счетчик импульсов
}sBSI_Results;

typedef struct
{
sBSI_Results BSI_Results;
u16 CodeI[BSI_N_ICHANNEL];  // Код токового АЦП в сыром виде
u16 CodeT[BSI_N_TCHANNEL];  // Код температурного АЦП в сыром виде
u16 CodeIAverage[BSI_N_ICHANNEL]; // Усредненный код токового АЦП
u16 CodeTAverage[BSI_N_TCHANNEL]; // Усредненный код температурного АЦП
u16 R[BSI_N_TCHANNEL]; // Сопротивление в Омах
u08 ArchiveStatus; // Статус прочитанной конфигурации из EEPROM =00 - основная копия в EEPROM
                  // =01 - вторая копия в EEPROM, =03 - резервная копия из памяти программ.
u08 Reserved2;
u08 Reserved3;
u08 Reserved4;  // Добавляем 4 резервных байта для выравнивания длины
}sBSI_ResultEx;

sBSI_ResultEx BSI_ResultEx; // Длина структуры 0x40

//========================================================================
// BSI Settings
// Перед работой прибора эти переменные копируются из EEPROM в ОЗУ
// Адрес в ОЗУ 0x0180 - то, что отдается хосту первым запросом по модбасу.

typedef struct
{
  u08 Dummy;                // Dummy Byte
  union
  {
   char byte;               // Config
   struct
    {
     char I1:   1,  // Включен ли канал
          I2:   1,
          I3:   1,
          I4:   1,
          T1:   1,
          T2:   1,
          Tint: 1;
    }bit;
  }on_off;
  u08 AverageKI[BSI_N_ICHANNEL]; // По скольки значениям усредняем (BSL)
  u08 AverageKT[BSI_N_TCHANNEL]; // По скольки значениям усредняем (BSL)
  u08 typeT[BSI_N_TCHANNEL]; // Тип первичного термопреобразователя TSP1, TSP2
  u16 AI[BSI_N_ICHANNEL];    // Коэффициенты А
  u16 AT[BSI_N_TCHANNEL];
  signed char BI[BSI_N_ICHANNEL];  // Коэффициенты B
  signed char BT[BSI_N_TCHANNEL];

  signed char KHI[BSI_N_ICHANNEL]; // Коэффициенты Kh
  signed char KHT[BSI_N_TCHANNEL];
  signed char KLI[BSI_N_ICHANNEL]; // Коэффициенты Kl
  signed char KLT[BSI_N_TCHANNEL];

  u16 UPHI[BSI_N_ICHANNEL];  // Пороги АЦП
  u16 UPHT[BSI_N_TCHANNEL];
  u16 UPLI[BSI_N_ICHANNEL];
  u16 UPLT[BSI_N_TCHANNEL];

  u08 gainADC[BSI_N_ICHANNEL+BSI_N_TCHANNEL];  // Gain of ADC
  u08 fsADC[BSI_N_ICHANNEL+BSI_N_TCHANNEL];  // FilterSelection of ADC
  union
   {
     u08 byte;      // BU of ADC
     struct
     {
       unsigned char BU0:1;   // Bit0
       unsigned char BU1:1;
       unsigned char BU2:1;
       unsigned char BU3:1;
       unsigned char BU4:1;
       unsigned char BU5:1;
     }bit;
   }BiUniPolar;
  union
   {
     u08 byte;      // Bits of Capture Mode xxxx3210
     struct       // 3,1  - filtr zaxvata, 2,0 - front zaxvata
     {
       unsigned char Front1:1;  // Bit0
       unsigned char Filter1:1; // Bit1
       unsigned char Front2:1;  // Bit2
       unsigned char Filter2:1; // Bit3
     }bit;
   }FICP;  // FICP of ADC

  u08 nZ_errors_max[BSI_N_ZCHANNEL]; // BRAK1, BRAK2

  struct
   {
    u32 min;
    u32 max;
   }ZTime[BSI_N_ZCHANNEL]; // TMIN1_, TMAX1_, TMIN2_, TMAX2_

  u16 ZTimeMeasure[BSI_N_ZCHANNEL];  // TPZ1_, TPZ2_
  u16 CheckSum; // Контрольная сумма структуры данных BSI_Settings (для EEPROM)
}sBSI_Settings;

sBSI_Settings BSI_Settings; // Пространство для конфигурации

// ******************************************************************************************************

/*
typedef union
{
  unsigned char byte[sizeof(sBSI_ResultEx)];
  sBSI_ResultEx Data;
}sBSI_Data;
*/

//=====================================================================
// BSI Config для восьмой функции

/*
typedef struct
{
  u32 min;
  u32 max;
}sBSI_Tmin_max;

typedef struct
{
  u08 type_T[BSI_N_TCHANNEL];             // type_T[]
  sBSI_Tmin_max period_Z[BSI_N_ZCHANNEL]; // period_Z[].min & period_Z[].max
  u16 time_Z[BSI_N_ZCHANNEL];             // time_Z[]
}sBSI_Config;
*/

// ===========================================================================
// Для определения абсолютного адреса структуры и отступа используем определения
//#define BSI_ABSOLUTE_ADDR 0x0180U  // Абсолютный физический адрес данных BSI_Results

#define BSI_ABSOLUTE_ADDR (unsigned int)&BSI_ResultEx
#define OFFSET  (BSI_ABSOLUTE_ADDR - BSI_RESULTS_OFFSET)


// ***************************************************************************

// То, что прибор отдает хосту по модбасу в первую очередь (конфигурация прибора)

#define BSI_SET_ADDR  0x0180U // CONF,CONFIG, BSL1, BSL2, BSL3, BSL4, BSL5, BSL6

// TSP1_, TSP2_, A_I1, A_I2, A_I3, A_I4, A_T1, A_T2
// B_I1, B_I2, B_I3, B_I4, B_T1, B_T2
// KT1H, KT2H, KT3H, KT4H, KT5H, KT6H
// KT1L, KT2L, KT3L, KT4L, KT5L, KT6L
// UPH1, UPH2, UPH3, UPH4, UPH5, UPH6
// UPL1, UPL2, UPL3, UPL4, UPL5, UPL6
// GAIN1, GAIN2, GAIN3, GAIN4, GAIN5, GAIN6
// FLT_SELECT1, FLT_SELECT2, FLT_SELECT3, FLT_SELECT4, FLT_SELECT5, FLT_SELECT6
// BI_UNI_POLAR, BRAK1, BRAK2
// TMIN1_, TMAX1_, TMIN2, TMAX2_, TPZ1_, TPZ2_

// Второй кусок отдается хосту по модбасу (результаты измерения)

#define BSI_RESULTS_OFFSET  0x0140U // RSF1, RSF0, DAT_I1, DAT_I2, DAT_I3, DAT_I4, DAT_T1, DAT_T2

// TERH, TERL, N_PUL1, N_PUL2, ADC_I1, ADC_I2, ADC_I3, ADC_I4, ADC_T1, ADC_T2
// ADS_I1, ADS_I2, ADS_I3, ADS_I4, ADS_T1, ADS_T2, RES1, RES2

//================================================================================

//char BUFF[40];
//char *BUFF=(char*)0x0140;

// глобальные переменные

unsigned int Capture_Reg_HI1; // ICR12, ICR13
unsigned int Capture_Reg_HI2; // ICR32, ICR33
unsigned int Register_OneSec; // Регистр секундного счетчика
unsigned int Register_WDT;    // Регистр сторожевого таймера
unsigned char Flag_WDT;       // Флаг сторожевого таймера: 0xFF - ОК, 0x00 - сброшен (сбрасывается в основном цикле) 

// Декрементальный регистр подсчета TPZ1 - счетчик времени измерения Z (кол-во единиц * на 1,2 мс)
unsigned int Register_TPZ1;
unsigned int Register_TPZ2;
unsigned char Faza1;        // Флаг фазы
unsigned char Faza2;
unsigned char Period1;      // Флаг периода
unsigned char Period2;      // Флаг периода
unsigned long ICR1A;        // Регистр-счетчик при сигнале = HIGH (Канал 1)
unsigned long ICR1B;        // Регистр-счетчик при сигнале = LOW (Канал 1)
unsigned long ICR2A;
unsigned long ICR2B;
unsigned char nError1;      // Временный регистр ошибки измерения (канал 1)
unsigned char nError2;

/*
//--------------------------------------------------------------
// Биты указывают о готовности измерений по каналам
// =1 - Данные готовы, =0 - Error
// DI1=0
// DI2=1
// DI3=2
// DI4=3
// DT1=4
// DT2=5
// DTIN=6
// SIG_TIN=7
typedef union
{
  char byte;
  struct
  {
    char DI1:1,
         DI2:1,
         DI3:1,
         DI4:1,
         DT1:1,
         DT2:1,
         DTIN:1,
         SIG_TIN:1;
  };
}sRSF0;
volatile sRSF0 RSF0;
*/


//--------------------------------------------------------------
// Биты указывают о готовности данных временного канала
// =1 - OK, =0 - Error
// FT1=0 Готовность данных по измерению Т1
// FT2=1 Готовность данных по измерению Т2
// CAP0=2 Состояние ноги захвата 1
// CAP1=3 Состояние ноги захвата 2
// FTMAX1=4 Норма по TMAX1
// FTMIN1=5 Норма по TMIN1
// FTMAX2=6 Норма по TMAX2
// FTMIN2=7 Норма по TMIN2

/*
typedef union
{
  char byte;
  struct
  {
    char FT1:1,
         FT2:1,
         CAP_0:1,
         CAP_1:1,
         FTMAX1:1,
         FTMIN1:1,
         FTMAX2:1,
         FTMIN2:1;
  };
}sRSF1;

volatile sRSF1 RSF1;
*/

/*
//--------------------------------------------------------------
// Биты указывают о фазе и периоде временных каналов Z (внутренняя переменая)
// FAZA1=0
// EIPER1=1
// x=2
// x=3
// FAZA2=4
// EIPER2=5
// x=6
// x=7
typedef union
{
  char byte;
  struct
  {
    char FAZA1:1,
         EIPER1:1,
         dummy2:1,
         dummy3:1,
         FAZA2:1,
         EIPER2:1,
         dummy6:1,
         dummy7:1;
  };
}sRG_FL;
volatile sRG_FL RG_FL;
*/






// **************************
// *** Прототипы процедур ***
// **************************
// === main.c ===
void InitTimers (void);
void InitSPI (void);
void InitPorts (void);
void Restart (void);

// === uart.c ===
void InitUART (void);
void Transmit (unsigned char howmanytx);

// === i2c.c ===
void I2C_ACK (void);
void I2C_NACK (void);
void I2C_Start (void);
void I2C_Stop (void);
void I2C_Byte_Write (unsigned char byte);
unsigned char I2C_Byte_Read (void);


// === matematic.c ===
unsigned int Pt_3910 (unsigned long resistance);
unsigned int Cu_4280 (unsigned long resistance);
unsigned int Pt_3850 (unsigned long resistance);
unsigned int Cu_4260 (unsigned long resistance);
unsigned int MovingAverage_I (unsigned int InputData, unsigned char channel);
unsigned int MovingAverage_T (unsigned int InputData, unsigned char channel);
void InitMovingAverage_I (void);
void InitMovingAverage_T (void);
void ClearMovingAverage_I (unsigned char channel);
void ClearMovingAverage_T (unsigned char channel);
unsigned int Temperature_Compensation_I (unsigned char channel);
unsigned int Temperature_Compensation_T (unsigned char channel);
unsigned int Matematic_I (unsigned char channel);
unsigned int Matematic_RT (unsigned char channel);
void Calc_Z1(void);
void Calc_Z2(void);
__monitor void WDT_Prescaler_Change(unsigned char prescaler);
unsigned int CheckConfigCRC (void);


// === adc7705.c ===
void Reset_ADC1 (void);
void Reset_ADC2 (void);
void Reset_ADC3 (void);
// --- ADC1 ---
void StartConversion_AD11 (void);
unsigned int ReadData_AD11 (void);
void StartConversion_AD12 (void);
unsigned int ReadData_AD12 (void);
void Conversion_I1 (void);
void Conversion_I2 (void);
void Calc_ADC1 (void);
// --- ADC2 ---
void StartConversion_AD21 (void);
unsigned int ReadData_AD21 (void);
void StartConversion_AD22 (void);
unsigned int ReadData_AD22 (void);
void Conversion_I3 (void);
void Conversion_I4 (void);
void Calc_ADC2 (void);
// --- ADC3 ---
void StartConversion_AD31 (void);
unsigned int ReadData_AD31 (void);
void StartConversion_AD32 (void);
unsigned int ReadData_AD32 (void);
void Conversion_T1 (void);
void Conversion_T2 (void);
void Calc_ADC3 (void);


// === ds1621.c ===
void Start_DS1621 (void);
void Init_DS1621 (void);
void Read_DS1621 (void);
void Read_Internal_T (void);


// === modbus.c ===
unsigned short CRC16(unsigned char *pBuf, unsigned char DataLen);
void CRC16_And_Store(unsigned char *pBuf, unsigned char DataLength);
void Modbus_Function_3 (void);
void Modbus_Function_6 (void);
void Modbus_Function_8 (void);
//void Modbus_Function_16 (void);
void Calc_Modbus (void);


// === eeprom.c ===
__monitor void Write_Config_To_EEPROM (unsigned int addr);
__monitor void Read_Config_From_EEPROM (unsigned int addr);
__monitor void Read_Config_From_PGM (unsigned int addr);
void Read_Settings (void);



