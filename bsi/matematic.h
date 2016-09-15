#define MAX_AVERAGE_WORDS (16)  // Максимальное кол-во значений усреднения по одному каналу
#define AVERAGE_FACTOR_I  (3)   // По скольки значениям усредняем (токовый канал) 1...15
#define AVERAGE_FACTOR_T  (15)  // По скольки значениям усредняем (температурный канал) 1...15
// #define CAPTURE_ERROR     (26) // Значение аддитивной погрешности частотомера

//============== Внутренние перемненные для процедуры Moving Average
// Двумерный массив результатов усреднения
volatile unsigned int AverageBufI[BSI_N_ICHANNEL][MAX_AVERAGE_WORDS];  // Буфер значений скользящего среднего
volatile unsigned int AverageBufT[BSI_N_TCHANNEL][MAX_AVERAGE_WORDS];

unsigned long AverageSumI[BSI_N_ICHANNEL];  // Накопленная сумма по каналу
unsigned long AverageSumT[BSI_N_TCHANNEL];

unsigned char AverageIndexI[BSI_N_ICHANNEL];  // Указатель текущего значения
unsigned char AverageIndexT[BSI_N_TCHANNEL];

unsigned char AverageBufFullI[BSI_N_ICHANNEL];  // Флаг заполнения буфера
unsigned char AverageBufFullT[BSI_N_TCHANNEL];


//======================================================================

// BSI_N_ICHANNEL+BSI_N_TCHANNEL = 6 - размерность массива каждой переменной
//volatile unsigned char AverageKI[BSI_N_ICHANNEL]; // По скольки значениям усредняем (параметр задается конфигуратором)
//volatile unsigned char AverageKT[BSI_N_TCHANNEL];

// Дебужная переменная для контроля
//volatile unsigned int debug;

// ================ Коэффициенты для каналов ====================
// 6 коэффициентов 0..3-токовые, 4..5-температурные
//volatile signed char KHI[BSI_N_ICHANNEL]; // -128...+127
//volatile signed char KHT[BSI_N_TCHANNEL];

//volatile signed char KLI[BSI_N_ICHANNEL]; // -128...+127
//volatile signed char KLT[BSI_N_TCHANNEL];

//volatile unsigned int AI[BSI_N_ICHANNEL];  // 0...65535
//volatile unsigned int AT[BSI_N_TCHANNEL];

//volatile signed char BI[BSI_N_ICHANNEL]; // -128...+127
//volatile signed char BT[BSI_N_TCHANNEL];

//------------------------------------------------------------
// Биты указывают флаг заполненного буфера 
// скользящего среднего шести каналов

/*

typedef union
{
  char byte;
  struct
  {
    char S0:1,
         S1:1,
         S2:1,
         S3:1,
         S4:1,
         S5:1,
         S6:1,
         S7:1;
  };
}sRGKS;
volatile sRGKS RGKS;

*/
