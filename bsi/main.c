// BSI measuring system
// Version 1.0
// By (R)soft Jun-Jul 2011

#include <intrinsics.h>
#include <iom162.h>
#include <iomacro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pgmspace.h>
#include <math.h>

#include "bsi.h"
#include "adc7705.h"
#include "ds1621.h"
#include "eeprom.h"
#include "uart.c"
#include "i2c.c"
#include "matematic.c"
#include "adc7705.c"
#include "ds1621.c"
#include "modbus.c"
#include "eeprom.c"
#include "interrupt.c"



// ==================================================================
//        PROCS
// ==================================================================

void InitTimers (void)
{
  // ========================== TIMER/COUNTER 1 (Capture Mode)
  // (0x01 by Default 0000 0001)
  // TCCR1B = ICNC1, ICES1, -, WGM13, WGM12, CS12, CS11, CS10
  // ICNC - Input Capture Noise Canceler
  // ICES - Input Capture Edge Select 0=Falling 1=Rising
  // WGM - Waveform Generation Mode
  // CS10..12 - Clock Selection 0,1,8,64,256,1024,Ext
  TCCR1B=(1<<CS10); // Prescaler = 1/1

  // ========================== TIMER/COUNTER 2 (Timer Mode)
  // (0x03 by Default 0000 0011)
  // f=8.0 MHz/32/256 = 977 Hz (976,5625)
  // TCCR2 = FOC2=0, WGM20, COM21, COM20, WGM21, CS22, CS21, CS20
  // FOC2 - Force Output Compare (для PWM режма), ставим в ноль
  // WGM20 (PWM2) - Mode Select
  // COM21 - Compare Match Output Mode (Выбор режима управления выходной ногой OC2)
  // COM20 - Compare Match Output Mode, в обычном режиме COM20=COM21=0
  // WGM21 (CTC2) - Mode Select
  // CS22 CS21 CS20 - Clock Select Prescaler
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:32
  //    100 - 1:64
  //    101 - 1:128
  //    110 - 1:256
  //    111 - 1:1024
  // CS21=1 & CS20=1 - Prescaler 32x

  TCCR2=(0<<FOC2)|(0<<WGM20)|(0<<COM21)|(0<<COM20)|(0<<WGM21)|(0<<CS22)|(1<<CS21)|(1<<CS20);
  
  // ========================== TIMER/COUNTER 3 (Capture Mode)
  // (0x01 by Default 0000 0001)
  // TCCR3B = ICNC3, ICES3, -, WGM33, WGM32, CS32, CS31, CS30
  TCCR3B=(1<<CS30); // No Prescaling

  // ========================== TIMERS/COUNTERS INTERRUPT SETUP
  // Timer Counter Interrupt Mask Register
  // (0x8C by Default 1000 1100)
  // TIMSK = TOIE1, OCIE1A, OCIE1B, OCIE2,   TICIE1, TOIE2, TOIE0, OCIE0
  // TOIE1=1 Timer1 Interrupt Enable
  // TICIE=1 Input1 Capture Interrupt Enable
  // TOIE2=1 Timer2 Interrupt Enable
  TIMSK = (1<<TOIE1)|(1<<TICIE1)|(1<<TOIE2);
  
  // ETIMSK (0x24 By Default 0010 0100)
  //  ETIMSK = -, -, TICIE3, OCIE3A,     OCIE3B, TOIE3, -, -
  // TICIE3=1 Input3 Capture Interrupt Enable
  // TOIE3=1  Timer3 Interrupt Enable
  ETIMSK = (1<<TICIE3)|(1<<TOIE3);
}

void InitSPI (void)
{
  // ========================= SPI SETUP
  // (0x53 by Default 0101 0011)
  // SPIE=0 SPE=1 DORD=0 MSTR=1  CPOL=0 CPHA=0 SPR1=1 SPR0=1
  // SPIE=0 Interrupt Disable
  // SPE=1 SPI Enable
  // DORD=0 Data Order (MSB First)
  // MSTR=1 SPI Mode (Master)
  // CPOL=0 Clock Polarity
  // CPHA=0 Clock Phase
  // SPR1=SPR0=1 Clock Rate 1/128 (период такта = 16 us)
  // Здесь поставил SPR1=SPR0=0 для ускорения (период такта = 500 нс)
  
  SPCR=(1<<SPE)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(1<<SPR0);
}

void InitPorts (void)
{
  //=========================== PORT A Definition
  // PORTA Direction (0xC0 by Default 1100 0000)
  LED_DIR  = (1<<LED_TX)|(1<<LED_CLC); // Порт светодиодов определен как выходной
  // PORTA Setup Bits (0x80 by Default 1000 0000)
  LED_PORT = (1<<LED_TX)|(0<<LED_CLC); // 1-Светодиод выключен, 0-Включен

  // ========================== PORT B
  // PORTB Direction (0xB4 by Default 1011 0100)
  I2C_DIR = (1<<SCK)|(0<<MISO)|(1<<MOSI)|(1<<4)|(0<<WP)|(1<<TPW)|(0<<SDA)|(0<<SCL);
//           SCK-Out  MISO-In   MOSI-Out         WP-In   TPW-Out  SDA-In   SCL-In

  // PORTB Setup Bits (0xF4 by Default 1111 0100)
  // В предустановках SCL & SDA равны нулю.
  // Когда соответствующе порты перенаправляются на ввод или на вывод,
  // то таким образом устанавливаются лог 0 или 1.
  // WP установили подтяжку к +VCC.
  I2C_PORT = (1<<SCK)|(1<<MISO)|(1<<MOSI)|(1<<4)|(1<<WP)|(1<<TPW)|(0<<SDA)|(0<<SCL);
  
  // =========================== PORT C
  // PORTC Direction (0x3F by Default 0011 1111)
  ADC_DIR = (1<<RST3)|(1<<RST2)|(1<<RST1)|(1<<CS3)|(1<<CS2)|(1<<CS1);
//           All For Output

  // PORTC Setup Bits (0x07 by Default 0000 0111)
  ADC_PORT = (0<<RST3)|(0<<RST2)|(0<<RST1)|(1<<CS3)|(1<<CS2)|(1<<CS1);

  // ========================== PORT D
  // PORTD Direction (0x16 by Default 0001 0110)
  DDRD = (0<<DRDY3)|(0<<DRDY2)|(0<<DRDY1)|(1<<4)|(0<<CAP2)|(1<<WDI)|(1<<TXD)|(0<<RXD);
//        DRDY3-In   DRDY2-In   DRDY1-In   Out    CAP2-In   WDI-Out  TXD-Out  RXD-In

  // PORTD Setup Bits (0xFF by Default 1111 1111)
  PORTD = (1<<DRDY3)|(1<<DRDY2)|(1<<DRDY1)|(1<<4)|(1<<CAP2)|(1<<WDI)|(1<<TXD)|(1<<RXD);

  // ========================== PORT E
  // PORTE Direction (0x00 by Default)
  DDRE = (0<<ADDR)|(0<<CAP1);
//        All for Inputs
  
  // PORTE Setup Bits (0x07 by Default 0000 0111)
  PORTE = (1<<2)|(1<<ADDR)|(1<<CAP1);
}

//=======================================================================
// Процедура делает рестарт всех устройств и чистит буфера
// Смотрятся биты включенных устройств и делается соотв. запуск
void Restart (void)
{
  Reset_ADC1(); // Аппаратный сброс АЦП1
  Reset_ADC2();
  Reset_ADC3();
  InitMovingAverage_I(); // Очистка и инициализация буферов для скользящего среднего
  InitMovingAverage_T();

  //--------------------
  // Очистка RSF1, RSF0, DAT_I1, DAT_I2, DAT_I3, DAT_I4, DAT_T1, DAT_T2
  // TERH, TERL, T_PER1, T_PER2, N_PUL1, N_PUL2,
  // ADC_I1, ADC_I2, ADC_I3, ADC_I4, ADC_T1, ADC_T2,
  // ADS_I2, ADS_I2, ADS_I3, ADS_I4, ADS_T1, ADS_T2,
  // RES1, RES2
  TerIn_HI=0;
  TerIn_LO=0;
  TerIn_2dT=0;

  unsigned char i;
  for (i=0;i<BSI_N_ICHANNEL;i++)
    {
      BSI_ResultEx.CodeI[i]=0; // Чистим токовые сырые значения АЦП
      BSI_ResultEx.CodeIAverage[i]=0; // Чистим средние значения токовых АЦП
      BSI_ResultEx.BSI_Results.I[i]=0; // Чистим готовые данные токовых АЦП
    }
  for (i=0;i<BSI_N_TCHANNEL;i++)
    {
      BSI_ResultEx.CodeT[i]=0; // Чистим температурные сырые значения АЦП
      BSI_ResultEx.CodeTAverage[i]=0; // Чистим средние значения температурных АЦП
      BSI_ResultEx.BSI_Results.T[i]=0; // Чистим готовые данные температурных АЦП
    }

  BSI_ResultEx.BSI_Results.Status.word=0;  // Чистим флаги RSF1, RSF0
  ReadyDataADC.byte=0; // Очистка флагов готовности данных АЦП (внутренние флаги)

// В связи с измененным алгоритмом опроса внутреннего термометра (выключить/включить питание),
// данная процедура не имеет смысла и отключена
/*  
  if  (CONFIG.TIN)  // Если в конфиге внутренний термометр включен, то запускаем
    {
      Pusk_DS1621();
      if (Error_I2C!=0)
        {
//          LED_PORT&=~(1<<LED_TX); // Если что-то не так в I2C то семафорим
          CONFIG.TIN=0; // Если была ошибка - принудительно отключаем устройство
//          char message[]="Init DS1621 Error!";
//          sprintf (UART_Buf,message); // Дебужный вывод отключить в финальной прошивке
//          Transmit(sizeof(message));
        }
    }
*/

  // Если во время конфигурации меняли флаги FICP, то после рестарта переустанавливаем флаги захвата по каналу Z

  // ICNC (Filter) включает/выключает схему подавления помех. I
  // Если он сброшен в 0, то схема подавления помех выключена и захват производится по первому же активному 
  // фронту на выводе ICP. 
  // Если бит установлен в 1, схема подавления помех включена и захват производится только в случае 4-х 
  // одинаковых выборок, соответствующих  активному фронту сигнала.

  // ICES определяет активный фронт сигнала, то есть фронт по которому блок захвата будет выполнять 
  // сохранение счетного регистра. 
  // Если бит сброшен в 0, активным является спадающий фронт. 
  // Если установлен в 1, то активным будет нарастающий фронт.
  
  if (BSI_Settings.FICP.bit.Filter1==1)
    {
      TCCR1B|=(1<<ICNC1); // Noise Canceler канала Z1
    }
  if (BSI_Settings.FICP.bit.Front1==1)
    {
      TCCR1B|=(1<<ICES1); // Edge Selection канала Z1
    }
  if (BSI_Settings.FICP.bit.Filter2==1)
    {
      TCCR3B|=(1<<ICNC3); // Noise Canceler канала Z2
    }
  if (BSI_Settings.FICP.bit.Front2==1)
    {
      TCCR3B|=(1<<ICES3); // Edge Selection канала Z2
    }

  
  // Для того, чтобы в основном цикле пришли готовности от АЦП (DRDY), нужно
  // произвести запуск АЦП на преобразование.

  if (BSI_Settings.on_off.bit.I1) // Если бит канала I1 установлен, то запускаем
    {
      Conversion_I1();
    }

  if (BSI_Settings.on_off.bit.I2)
    {
      Conversion_I2();
    }

  if (BSI_Settings.on_off.bit.I3)
    {
      Conversion_I3();
    }

  if (BSI_Settings.on_off.bit.I4)
    {
      Conversion_I4();
    }

  if (BSI_Settings.on_off.bit.T1)
    {
      Conversion_T1();
    }

  if (BSI_Settings.on_off.bit.T2)
    {
      Conversion_T2();
    }
}


// ===================================================================
//
//                   M A I N
//              
// ===================================================================


void main(void)
{

  // Watchdog timer по умолчанию всегда включен, после установки прескалера нужно
  // всё время давать собаке покушать, чтобы она нас не съела.
  // 111 = 2,1 с - пашет
  // 110 = 1 с - пашет
  // 101 = 0,52 с 
  // 100 = 0,26 c
  // 011 = 0,13 с 
  
  WDT_Prescaler_Change((1<<WDP2) | (0<<WDP1) | (0<<WDP0)); // Старт сторожевой собаки

  Read_Settings(); // Чтение конфига из EEPROM в SRAM

    // Этот кусок рушит один байт конфига, соотв. контрольная сумма не совпадает и происходит
  // периодический рестарт программы.
//  BSI_Settings.nZ_errors_max[0]=BSI_ResultEx.ArchiveStatus; // Debug Code - показывает код статуса архива (0..2)

  
  InitPorts(); // Init Sys
  InitSPI();
  InitUART();
  InitTimers();

  HowMany_TX = 0; // Инициализация переменных, сколько байт передать.
  HowMany_RX = 0; // Сколько байт принято.
  Pointer_TX = 0; // Указатель в буфере передатчика.
  TimeOut_RX = 0; // Таймаут приемника.
  ReceiveOK=0;
  SecundaOK=0;
  Flag_WDT=0;
  DS1621_Stage=0; // Инициализация конечного автомата DS1621
  Register_OneSec = SEK_CONST; // Инициализация секундного счетчика
  Register_WDT = WDT_CONST; // Инициализация счетчика WDT
  
  __enable_interrupt();
  
  unsigned long c;
  c=0x100000;
  while (c--)
    {
      __watchdog_reset(); // Здесь даем собаке покушать
      PORTD^=(1<<WDI); // Подрыгаем ногой WDI в начале
      LED_PORT ^= (1<<LED_CLC); // Блымаем светодиод цикла (изначально порт настроен так что светодиод светит)
    }
  LED_PORT|=(1<<LED_CLC); // Гасим после цикла (далее должен блымать в цикле)
  
  Restart();  // Процедура делает рестарт всех устройств и чистит буфера
              // В этом месте, очевидно, собака успевает проголодаться (интервал увеличить)


// ---------------------------------------------------------
//  M A I N    L O O P
// ---------------------------------------------------------
// Main Loop


//  char message[]="BSI Addr: %X";
//  sprintf (UART_Buf,message,Net_Addr); // Начальное приветствие для отладки
//  Transmit(11);
  
  for(;;)
   {
    if (Flag_WDT & (CheckConfigCRC()==0)) // Если время кормления собаки наступило и контрольная сумма тела коныфига в порядке, то
      {
        Flag_WDT=0; // Сброс флага WDT
        __watchdog_reset(); // Даем собаке косточку в основном цикле
        PORTD^=(1<<WDI); // Дрыгаем ногой WDI в основном цикле
      }
    
    if (SecundaOK) // Если очередная секунда наступила, то каждую секунду включаем конечный автомат DS1621
      {
//          FLAG_ADC=1;
         SecundaOK=0; // Сброс секундного флага
         
         switch (DS1621_Stage) // Реализация конечного автомата для DS1621. DS_Stage - текущее состояние автомата
         {
          case 0: 
            I2C_Power_ON(); // Состояние 0 - вкл. питание и инициализация
//            BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=0; // Сброс флага готовности данных внутр. термометра
            Init_DS1621();
            break;
          case 1: 
            Start_DS1621(); // Состояние 1 - Старт термометра DS1621
            break;
          case 2: 
            Read_Internal_T();  // Состояние 2 - Чтение результата температуры (флаг ставит автоматом)
            I2C_Power_OFF();  // и откл. питание
            break;
//          case 3: // Это состояние может быть пустым.
//            Send_Result_DS1621(); // Состояние 3 - вывод результата температуры (комментим эту строку)
//            break;
          }
          DS1621_Stage++;
          DS1621_Stage&=0x03; // Избавляемся от лишних проверок на переполнение счетчика состояний
      }

     
    if (!(PIND&(1<<DRDY1)))  // Смотрим готовность АЦП1
      {
//         PORTD&=~(1<<WDI); // Опускаем ногу WDI (для отладки)
        Calc_ADC1(); // Если готов, то забираем результат и делаем вычисления (время обработки ~ 1,2 ms)
//         PORTD|=(1<<WDI); // Поднимаем ногу WDI
      }

    if (!(PIND&(1<<DRDY2)))  // Смотрим готовность АЦП2
      {
        Calc_ADC2();
      }

    if (!(PIND&(1<<DRDY3)))  // Смотрим готовность АЦП3
      {
        Calc_ADC3();
      }

     // Наиболее значимое время - время обработки АЦП, каждая процедура Calc_ADC занимает ~ 1,2 мс, умноженное на 3 = 3,6 мс.
     // Для уверенного предотвращения зависания процессора в основном цикле, сторожевого пса нужно дергать с периодичностью
     // хотя бы ~ 10 ms. В реалии прескалер сторожевой собаки установлен на максимум.

    if ((BSI_Settings.on_off.byte & 0x3F) == (ReadyDataADC.byte & 0x3F)) // Если все данные АЦП по включенным каналам готовы, то
      {
        LED_PORT ^= (1<<LED_CLC); // Инвертируем бит светодиода CYCLE (признак работы включенных АЦП)
        ReadyDataADC.byte=0;  // Обнуляем флаги готовности данных АЦП (нужны только для мигания светодиода)
      }

/*
     if (FLAG_ADC && (DS1621_Stage==10)) // Если DS1621_Stage>3 то отключаем дебужный вывод (по умолчанию DS1621_Stage==1)
       {
          FLAG_ADC=0;
          unsigned int adc1=ADC_I[0];
          unsigned int adc2=ADC_I[1];
          unsigned int s1=ADS_I[0];
          unsigned int s2=ADS_I[1];
          unsigned int dat1=DAT_I[0];
          unsigned int dat2=DAT_I[1];
          sprintf(UART_Buf," 1:%X 2:%X S1:%X S2:%X D1:%X D2:%X ==",adc1,adc2,s1,s2,dat1,dat2); // Дебужный вывод значений АЦП
          UART_Buf[47]=0x0D;
          UART_Buf[48]=0x0A;
          Transmit(49);
       }
*/

     if (ReceiveOK) // Если что-то приняли по RS-232
        {
          Calc_Modbus(); // вызов процедуры Modbus (время обработки ~ 400 - 600 мкс в зависимости от длины запроса)
        }

     if (Period1) // Частота появления этого флага зависит от измеряемой частоты (чем выше частота тем чаще появляется)
       {
         Calc_Z1(); // Время обработки ~ 15 мкс (иногда скачет до 25 мкс)
         Period1=0;
       }
     
     if (Period2)
       {
         Calc_Z2();
         Period2=0;
       }
     
     BSI_ResultEx.BSI_Results.Status.Flag.LevelZ1=(PINE&(1<<CAP1)); // Состояние ног канала Z копируем в соотв. флаги ВСЕГДА
     BSI_ResultEx.BSI_Results.Status.Flag.LevelZ2=(PIND&(1<<CAP2));
      
   }
}


