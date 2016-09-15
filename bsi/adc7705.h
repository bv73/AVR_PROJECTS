// Command AD7705 Definitions
#define Command7705_Clk     0x20
#define Command7705_SetT    0x10

// C_CLK 0000_0100
// ZERO ZERO ZERO CLKDIS=0 _ CLKDIV=0 CLK=1 FS1=? FS0=?
// При тактовой частоте АЦП 2,4576 МГц CLKDIV=0 & CLK=1
#define Command7705_C_Clk   0x04
#define Command7705_C_Set   0x42
#define Command7705_RD_Com  0x08
#define Command7705_RD_Data 0x38
#define Command7705_C_Stop  0x47

// Setup Register of AD7705
#define FSYNC   0
#define BUF     1
#define B_U     2
#define MD0     6
#define MD1     7

// Результат чтения АЦП в сыром виде, как есть (CodeI[],CodeT[])
//volatile unsigned int CodeI[BSI_N_ICHANNEL];
//volatile unsigned int CodeT[BSI_N_TCHANNEL];

// Результат усредненный фильтром скользящего среднего (CodeIAverage[], CodeTAverage[])
//volatile unsigned int CodeIAverage[BSI_N_ICHANNEL];
//volatile unsigned int CodeTAverage[BSI_N_TCHANNEL];

// Результат после математической обработки (I[], T[])
//volatile unsigned int I[BSI_N_ICHANNEL];
//volatile unsigned int T[BSI_N_TCHANNEL];

// Минимально допустимое значение АЦП
//volatile unsigned int UPLI[BSI_N_ICHANNEL];
//volatile unsigned int UPLT[BSI_N_TCHANNEL];

// Максимально допустимое значение АЦП
//volatile unsigned int UPHI[BSI_N_ICHANNEL];
//volatile unsigned int UPHT[BSI_N_TCHANNEL];

// Задание усиления каждого канала АЦП
// Эти переменные должны быть инициализированы из EEPROM
// перед работой прибора
// Gain for AD7705 n=0...7
// Gain=2^n
//volatile char Gain[BSI_N_ICHANNEL+BSI_N_TCHANNEL];

// Задание фильтра для каждого канала АЦП (всего шесть)
// Эти переменные должны быть инициализированы из EEPROM
// Filter Selection for AD7705 0..3
// От этого значения зависит время преобразования:
// CLK всегда=1
// CLK FS1 FS0 / Output Update Rate
//  0   0   0 - 20 Hz
//  0   0   1 - 25 Hz
//  0   1   0 - 100 Hz
//  0   1   1 - 200 Hz
//  1   0   0 - 50 Hz - самый медленный режим
//  1   0   1 - 60 Hz
//  1   1   0 - 250 Hz
//  1   1   1 - 500 Hz - самый быстрый режим
//volatile unsigned char Filter_Selection[BSI_N_ICHANNEL+BSI_N_TCHANNEL];

//------------------------------------------------------------
// Биты указывают, какой текущий канал АЦП выбрать (внутренняя переменная)
// NT12=0 - для термо АЦП (каналы 1 и 2)
// NI12=1 - для токового АЦП1 (каналы тока 1 и 2)
// NI34=2 - для токового АЦП2 (каналы тока 3 и 4)

/*
typedef union
{
  char byte;
  struct
  {
    char NT12:1,    // 0 - T1, 1 - T2 of ADC3
         NI12:1,    // 0 - I1, 1 - I2 of ADC1
         NI34:1,    // 0 - I3, 1 - I4 of ADC2
         dummy3:1,
         dummy4:1,
         dummy5:1,
         dummy6:1,
         dummy7:1;
  };
}sNumChannel;
volatile sNumChannel Channel;
*/

unsigned char Channel_NT12;
unsigned char Channel_NI12;
unsigned char Channel_NI34;

/*
//--------------------------------------------------------
// Биты указывают режим работы каждого канала АЦП.
// Биполярный или униполярный режим.

typedef union
{
  char byte;
  struct
  {
    char BU0:1,   // Bit0
         BU1:1,
         BU2:1,
         BU3:1,
         BU4:1,
         BU5:1,
         dummy6:1,
         dummy7:1;
  };
}BiUniPolar;


//volatile sBI_UNI_POLAR BiUniPolar;
*/

//----------------------------------------------------------
// Биты указывают, какие устройства включены
// I_1=0 Если =1, то токовый канал 1 включен, 0 - выключен
// I_2=1
// I_3=2
// I_4=3
// T_1=4
// T_2=5
// TIN=6 Если =1, то набортный термометр включен, 0 - выключен

/*
typedef union
{
  char byte;
  struct
  {
    char I1:1,
         I2:1,
         I3:1,
         I4:1,
         T1:1,
         T2:1,
         Tint:1,
         dummy7:1;
  };
}sCONFIG;
volatile sCONFIG Config;
*/

//--------------------------------------------------------------
// Биты указывают о готовности данных АЦП (внутренняя переменная)
// Вообще-то эти биты нужны для того чтобы только лишь поблымать светодиодом
// и увидеть что процесс АЦП работает
// =1 - Данные готовы
// FDI1=0
// FDI2=1
// FDI3=2
// FDI4=3
// FDT1=4
// FDT2=5

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
         dummy6:1,
         dummy7:1;
  };
}sRS_FIH;
sRS_FIH ReadyDataADC;


