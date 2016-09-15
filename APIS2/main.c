//----------------------------------
// APIS Washer AVTF104 Board
// By (R)soft
// v2.0 19/06/2015
// Для печатной платы ver 2.0
// For ATmega328P or ATmega88(PA)

#define SOUND_ON

#include <iom88pa.h>
#include <intrinsics.h> // for __**** procedures
#include <iomacro.h>

#include "avrlib\i2c.h"
#include "avrlib\i2c.c"

#include "prgwash.h"
#include "eeprom.h"
#include "apis.h"

#include "timers.h"
#include "adc.h"
//#include "nec.h"
#include "pwm.h"


//*********************************************************************

void main( void )
{
  InitPorts();
  InitTimers();
  InitADC();
  InitEEPROM(); // Проверка и инициализация EEPROM

  //========= Init I2C Part
  // initialize i2c function library
  i2cInit();

  MaskProg = 0xFF; // Маска включения индикации по выбранной программе
  PWMCount = 0xFF;
  Timer_I2C = I2C_TIMER_CONST;
  msec = 0;
  sec = 0;
  minute = 0;
  hour = 0;
  timer = 0; // Сброс таймера режимов стирки
//  SecretCode = SECRET_CODE;
  SecretCode = 0;
  SecretMode = 0;

  ProgramCounter = 0;
  ProgramNumber = 0;
  cnt = 0; // Счетчик Do While = 0
  
//  ReceivedFlag=0; // Сброс флага принятой команды

  // Enable INT1 for ZeroCross (rising edge) & INT0 for IRDA (falling edge)
  EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(0<<ISC00);

  // INT0 отключен - прием команд с пульта ДУ
  EIMSK = (1<<INT1)|(0<<INT0);  // Interrupt Mask Register (Enable INT1 & INT0)
  __enable_interrupt();

  // Init Indication Registers - начальное приветствие
  Reg.Seg1 = DIG_A;
  Reg.Seg2 = DIG_P;
  Reg.Seg3 = DIG_I;    
  Reg.Seg4 = DIG_S;    
  Reg.Red1 = 0x00;
  Reg.Red2 = 0x00;
  Reg.Red3 = 0x00;
  Reg.Ris1 = 0x00;
  Reg.Ris2 = 0x00;
  Reg.Ris3 = 0x00;
  Reg.Ris4 = 0x00;
  Reg.Modes = 0x00;
  Reg.Spin = 0x00;
  Reg.Termo = 0x00;
  Reg.Timer = 0x00;
  Reg.Lock = 0x00;
  Reg.Key = 0x00;
  Reg.Points = 0x00;
  Reg.Buzzer = 0x00;
  Reg.Gradus = 0x00;

  i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // Передаем регистры индикации
  Timer_Buzzer = 10;  // Время приветственного писка
  delay_timer(2); // На время покажем приветствие 

  Reg.Modes = 0xFF; // Всегда отображаем режимы

  //========================= Основной цикл ================================  
  while (1)
  {

    adcLine = adc_read(6);
    adcTemperature = adc_read(7);

/*
    // Проверка на ошибки - обрыв датчика температуры
    if (adcTemperature==0)
    {
      Reg.Seg1 = DIG_E; // Error1 - Датчик температуры оборван (значение АЦП 0x000)
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_1;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
      while (1) { __no_operation(); } // Светим номер ошибки и зависаем
    }

    // Проверка на ошибки - датчик температуры закорочен
    if (adcTemperature==0x3FF)
    {
      Reg.Seg1 = DIG_E; // Error2 - Датчик температуры закорочен (значние АЦП 0x3FF)
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_2;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
      while (1) { __no_operation(); } // Светим номер ошибки и зависаем
    }
*/
    
    temperature = TemperatureConvert( adcTemperature );
    
    //-------------- Обработка команд с пульта ДУ и клавиатуры панели --------------------
    if (KeyFlag) // Если флаг принятой команды выставлен или кнопка нажата
    {

      KeyFlag=0;

      // --- Проверка нажатия STOP при любом положении программного переключателя ----
      if (ProgramNumber && Key==kStart && !RepeatFlag) // Если программа запущена и нажата кнопка START/STOP
      {
        ProgramStartStop(8, PROGRAM8); // передаем в ветку стоп (к примеру с параметрами 8 программы)
      }

      if (ProgramNumber==0) // Если программа не запущена
      {
        ManualMode(); // Режим запуска программ и редактирования параметров стирки
      }

      Key=0;  // Сброс кода клавиши для автоповтора и квитирования
    }
    //------------ End of обработка команд с пульта ДУ ------------------

    //--------- Обработка State регистров в основном цикле ----------------

    if (ForwardState==0) FORWARD=0; else FORWARD=1;
    if (ReverseState==0) REVERSE=0; else REVERSE=1;
    // Если реле мотора включено - риска 4 индицирует
    if (ForwardState || ReverseState) Reg.Ris4 = 0xFF;
    else                              Reg.Ris4 = 0;

    // В этой версии сигналы управления симисторами должны инвертироваться,
    // Т.е. лог. 1 соответствует отключению и лог. 0 - вкл.

    // Замок люка
    if (LockState==0) LOCK = 1; // Если ноль, то выключаем
    else              LOCK = 0;

    if (adcLine < LINE_ON_TRESHOLD) Reg.Key = 0xFF;      // Если фаза есть, то индицируем ключиком
    else                            Reg.Key = 0;         // иначе гасим

    // Помпа включена - риска 3 индицирует
    if (PompState==0) { POMP = 1; Reg.Ris3 = 0; } // Если ноль, то выключаем
    else              { POMP = 0; Reg.Ris3 = 0xFF; }

    // Залив воды для предварительной стирки - риска 1 индицирует
    if (WaterState1==0)  { WATER1 = 1; Reg.Ris1 = 0; }  // Если ноль, то выключаем
    else                 { WATER1 = 0; Reg.Ris1 = 0xFF; }

    // Залив воды для основной стирки - риска 2 индицирует
    if (WaterState2==0)  { WATER2 = 1; Reg.Ris2 = 0; }  // Если ноль, то выключаем
    else                 { WATER2 = 0; Reg.Ris2 = 0xFF; }

    if (LEVEL1) Reg.Red1 = 0;
    else        Reg.Red1 = 0xFF;  // Уровни индицируем красными светодиодами

    if (LEVEL2) Reg.Red2 = 0;
    else        Reg.Red2 = 0xFF;

    if (LEVEL3) Reg.Red3 = 0;
    else        Reg.Red3 = 0xFF;

/*
    // Проверка на ошибки - датчик уровня неподключен или в обрыве
    if (LEVEL1==1 && LEVEL2==1 && LEVEL3==1)
    {
      Reg.Seg1 = DIG_E; // Error3 - Датчик уровня неподключен или в обрыве (LEVEL1=LEVEL2=LEVEL3=1)
      Reg.Seg2 = DIG_r;
      Reg.Seg3 = DIG_r;
      Reg.Seg4 = DIG_3;
      i2cMasterSendNI(0x40, 4, Reg.Buf);  // Передаем регистры индикации
      while (1) { __no_operation(); } // Светим номер ошибки и зависаем
    }
*/
    
    // Обработка датчика уровня в основном цикле (для безопасности ТЭНа)
    // Если вдруг уровень переключился на "бак пустой" или запрещенную комбинацию, то 
    // нагрев ТЭНа должен сброситься. Регистр HeatingState можно устанавливать в программе,
    // но проверки в главном цикле могут заблокировать реальный сигнал HEATING, это нужно
    // учитывать при написании программ стирки. Есть нехорошая ситуация, когда в программе
    // стирки цикл дошел до стадии нагрева, но вдруг бельё впитало воду и датчик уровня
    // вернулся в LEVEL1, соответственно нагрев сбросится, чтобы защитить ТЭН. Но, после
    // этого программа "зависнет" т.к. будет ждать нагрева воды, но ТЭН "аварийно" отключен.
    // В этой ситуации можно только нажать кнопку ОТМЕНА, долить воды, и повторить программу.
    if ( (LEVEL1==0)&&(LEVEL2==1) ) HeatingState = 0;
    if ( (LEVEL1==1)&&(LEVEL2==1) ) HeatingState = 0; // Запрещенное состояние (датчик отключен)

    // LEVEL3 включается, когда LEVEL2=0. Т.е. не переключается с LEVEL2 на LEVEL3, а добавляется к LEVEL2.
//    if ( LEVEL3==0 && LEVEL2==0 ) PompState = 0xFF; // Если переполнение бака, то вкл помпу

    // ТЭН
    if (HeatingState==0)
    {
      HEATING = 0; 
      if (TempMode!=8)
      {
        if (TempMode!=9) Reg.Termo = 0;
      }
    }
    else
    {
      HEATING = 1;
      if (TempMode!=8)
      {
        if (TempMode!=9) Reg.Termo = 0xFF; // Нагрев индицируем красным термометром
      }
    }
    
    //------------ End of обработка State регистров --------------------

    //------------ Индикация панели -----------------------------------
    // Ручка выбора программ задает режим индикации
    // MaskProg=0xFF - нормальная индикация в зависимости от положения ручки программ
    // MaskProg=0 - блокировка индикации во всех положениях ручки программ
    //              (полезно при дополнительной индикации в особых режимах программы)
    switch (Reg_Prog & MaskProg)
    {
    // Для отладки, в этом режиме отображается состояние LINE (замок люка)
    case 1:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_L;
                Reg.Seg4 = HEX[LockState&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_1;
              }
              break;
    // Для отладки, в этом режиме отображается меню управления мотором
    case 2:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_S;
                Reg.Seg2 = DIG_P;
                Reg.Seg3 = HEX[(RotorState>>4)&0x0F];
                Reg.Seg4 = HEX[RotorState&0x0F];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_2;
              }
              break;
    // Для отладки, в этом режиме отображается состояние клапана залива воды
    case 3:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_H;
                Reg.Seg2 = DIG_2;
                Reg.Seg3 = DIG_o;
                Reg.Seg4 = HEX[WaterState1&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_3;
              }
              break;
    // Для отладки, в этом режиме отображается состояние помпы
    case 4:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_P;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = HEX[PompState&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_4;
              }
              break;
    // Для отладки, в этом режиме отображается состояние реле ТЭНа
    case 5:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_H;
                Reg.Seg2 = DIG_E;
                Reg.Seg3 = DIG_A;
                Reg.Seg4 = HEX[HeatingState&0x01];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_5;
              }
              break;
    // Для отладки, в этом режиме отображается значение АЦП adcLine
    case 6:
              if (SecretMode==0xFF) { SecretMode = 0; SecretCode = 0; } // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretCode==SECRET_CODE)
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = HEX[(adcLine>>8)&0x0F];
                Reg.Seg3 = HEX[(adcLine>>4)&0x0F];
                Reg.Seg4 = HEX[adcLine&0x0F];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_6;
              }
              break;
    // Для отладки, в этом режиме можно ввести секретный код (Secret Code)
    case 7:
              if (SpinMode==8)  { SpinMode = 0; Reg.Spin = 0; }   // Сброс режимов при повороте ручки
              if (TempMode==8)  { TempMode = 0; Reg.Termo = 0; }  // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (SecretMode==0xFF) // Если установлен секретный режим, то отображаем ввод секретного кода
              {
                Reg.Seg1 = DIG_S;
                Reg.Seg2 = DIG_c;
                Reg.Seg3 = HEX[(SecretCode>>4)&0x0F];
                Reg.Seg4 = HEX[SecretCode&0x0F];
              }
              else
              {
                Reg.Seg1 = DIG_SPACE;
                Reg.Seg2 = DIG_SPACE;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_7;
              }
              break;
    // Программа №8 - продолжительная стирка
    case 8:
              if (SpinMode==9)  { SpinMode = 0; Reg.Spin = 0; }   // Сброс режимов при повороте ручки
              if (TempMode==9)  { TempMode = 0; Reg.Termo = 0; }  // Сброс режимов при повороте ручки
              if (SecretMode==0xFF) { SecretMode = 0; SecretCode = 0; } // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 8)  // Если программа работает то отображаем PC & таймер цикла
              {
                if (HeatingState != 0) // Если включен ТЭН, то отображаем температуру нагрева
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[temperature]>>8; 
                  Reg.Seg4 = DEC[temperature]&0xFF;
                }
                else
                {
                  Reg.Seg1 = DEC[ProgramCounter]>>8;
                  Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                  Reg.Seg3 = DEC[timer]>>8; 
                  Reg.Seg4 = DEC[timer]&0xFF;
                }
              }
              else
              {
                if (TempMode==8)  // Если нажали режим изменения t, то отображаем значение t
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[SetTemperature]>>8; 
                  Reg.Seg4 = DEC[SetTemperature]&0xFF;
                }
                else  // иначе обычный режим или обороты
                {
                  if (SpinMode==8)  // Если нажали режим изменения оборотов, то отображаем значение оборотов
                  {
                    Reg.Seg1 = DIG_SPACE;
                    Reg.Seg2 = DEC[SetSpin]&0xFF;
                    Reg.Seg3 = DIG_0;
                    Reg.Seg4 = DIG_0;
                  }
                  else
                  {
                    Reg.Seg1 = DIG_P;
                    Reg.Seg2 = DIG_r;
                    Reg.Seg3 = DIG_SPACE;
                    Reg.Seg4 = DIG_8;
                  }
                }
              }
              break;
    // Программа №9 - быстрая стирка
    case 9:
              if (SpinMode==8)  { SpinMode = 0; Reg.Spin = 0; }   // Сброс режимов при повороте ручки
              if (TempMode==8)  { TempMode = 0; Reg.Termo = 0; }  // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 9)  // Если программа работает то отображаем PC & таймер цикла
              {
                if (HeatingState != 0) // Если включен ТЭН, то отображаем температуру нагрева
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[temperature]>>8; 
                  Reg.Seg4 = DEC[temperature]&0xFF;
                }
                else
                {
                  Reg.Seg1 = DEC[ProgramCounter]>>8;
                  Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                  Reg.Seg3 = DEC[timer]>>8; 
                  Reg.Seg4 = DEC[timer]&0xFF;
                }
              }
              else
              {
                if (TempMode==9)  // Если нажали режим изменения t, то отображаем значение t
                {
                  Reg.Gradus = 0xFF;
                  Reg.Seg1 = DIG_t;
                  Reg.Seg2 = DIG_SPACE;
                  Reg.Seg3 = DEC[SetTemperature]>>8; 
                  Reg.Seg4 = DEC[SetTemperature]&0xFF;
                }
                else  // иначе обычный режим или обороты
                {
                  if (SpinMode==9)  // Если нажали режим изменения оборотов, то отображаем значение оборотов
                  {
                    Reg.Seg1 = DIG_SPACE;
                    Reg.Seg2 = DEC[SetSpin]&0xFF;
                    Reg.Seg3 = DIG_0;
                    Reg.Seg4 = DIG_0;
                  }
                  else
                  {
                    Reg.Seg1 = DIG_P;
                    Reg.Seg2 = DIG_r;
                    Reg.Seg3 = DIG_SPACE;
                    Reg.Seg4 = DIG_9;
                  }
                }
              }
              break;
    // Для отладки, в этом режиме отображается текущая температура в градусах
    // а также после нажатия на кнопку термометра - HEX значение АЦП термометра.
    case 10:
              if (SpinMode!=0)  { SpinMode = 0; Reg.Spin = 0; }   // Сброс режимов при повороте ручки
              if (TempMode!=0)  { TempMode = 0; Reg.Termo = 0; }  // Сброс режимов при повороте ручки
              Reg.Gradus = 0xFF;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              Reg.Seg1 = DIG_SPACE;
              Reg.Seg2 = DIG_SPACE;
              if (temperature==0xFF) { Reg.Seg3 = DIG_MINUS; Reg.Seg4 = DIG_MINUS; }
              else  { Reg.Seg3 = DEC[temperature]>>8; Reg.Seg4 = DEC[temperature]&0xFF; }
              break;
    case 11:
              if (SpinMode==12)  { SpinMode = 0; Reg.Spin = 0; } // Сброс режимов при повороте ручки
              Reg.Timer =  0xFF;
              Reg.Points = 0x80;
              Reg.Gradus = 0x00;
              if (!ClockMode)
              {
                Reg.Seg1 = DEC[minute]>>8;
                Reg.Seg2 = DEC[minute]&0xFF;
                Reg.Seg3 = DEC[sec]>>8;
                Reg.Seg4 = DEC[sec]&0xFF;
              }
              else
              {
                Reg.Seg1 = DEC[hour]>>8;
                Reg.Seg2 = DEC[hour]&0xFF;
                Reg.Seg3 = DEC[minute]>>8;
                Reg.Seg4 = DEC[minute]&0xFF;
              }
              break;
    // Программа №12 - интенсивное полоскание, слив и деликатный отжим
    case 12:
              if (SpinMode==13)  { SpinMode = 0; Reg.Spin = 0; } // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 12)  // Если программа работает то отображаем PC & таймер цикла
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                if (SpinMode==12)  // Если нажали режим изменения оборотов, то отображаем значение оборотов
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // Отображаем текущие установленные значения оборотов
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // иначе обычный режим
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_2;
                }
              }
              break;
    // Программа №13 - легкое полоскание, слив и деликатный отжим
    case 13:
              if (SpinMode==12 || SpinMode==14)  { SpinMode = 0; Reg.Spin = 0; } // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 13)  // Если программа работает то отображаем PC & таймер цикла
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                if (SpinMode==13)  // Если нажали режим изменения оборотов, то отображаем значение оборотов
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // Отображаем текущие установленные значения оборотов
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // иначе обычный режим
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_3;
                }
              }
              break;
    // Программа №14 - интенсивный отжим
    case 14:
              if (SpinMode==13 || SpinMode==15)  { SpinMode = 0; Reg.Spin = 0; } // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 14)  // Если программа работает то отображаем PC & таймер цикла
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                if (SpinMode==14)  // Если нажали режим изменения оборотов, то отображаем значение оборотов
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // Отображаем текущие установленные значения оборотов
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // иначе обычный режим
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_4;
                }
              }
              break;
    // Программа №14 - деликатный отжим
    case 15:
              if (SpinMode==14)  { SpinMode = 0; Reg.Spin = 0; } // Сброс режимов при повороте ручки
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 15)  // Если программа работает то отображаем 
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else // иначе отображаем обороты или номер выбранной программы
              {
                if (SpinMode==15)  // Если нажали режим изменения оборотов, то отображаем значение оборотов
                {

                  Reg.Seg1 = DIG_SPACE;
                  Reg.Seg2 = DEC[SetSpin]&0xFF; // Отображаем текущие установленные значения оборотов
                  Reg.Seg3 = DIG_0;
                  Reg.Seg4 = DIG_0;
                }
                else  // иначе обычный режим
                {
                  Reg.Seg1 = DIG_P;
                  Reg.Seg2 = DIG_r;
                  Reg.Seg3 = DIG_1;
                  Reg.Seg4 = DIG_5;
                }
              }
              break;
    // Программа №16 - слив воды
    case 16:
              if (SpinMode==15)  { SpinMode = 0; Reg.Spin = 0; }
              Reg.Gradus = 0x00;
              Reg.Timer =  0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 16)  // Если программа работает то отображаем PC & таймер цикла
              {
                Reg.Seg1 = DEC[ProgramCounter]>>8;
                Reg.Seg2 = DEC[ProgramCounter]&0xFF;
                Reg.Seg3 = DEC[timer]>>8; 
                Reg.Seg4 = DEC[timer]&0xFF;
              }
              else
              {
                Reg.Seg1 = DIG_P;
                Reg.Seg2 = DIG_r;
                Reg.Seg3 = DIG_1;
                Reg.Seg4 = DIG_6;
              }
    }
    
    
    
    
    //------------ I2C Section ------------------------
    // Примерно каждые 50 мс передаются регистроы индикации и опрашивается клавиатура
    Timer_I2C--; // Таймер опроса I2C
    switch (Timer_I2C)
    {
    case 0: Timer_I2C = I2C_TIMER_CONST;  // Если счетчик достиг нуля - перезагружаем
            break;

    case 11: 
            i2cMasterReceive(0x41, 3, BufRead); // Принимаем 3 байта (для чтения адрес слэйва на один больше)
            // Фильтрация помехи по I2C. По невыясненным причинам не всегда
            // стабильно читает по I2C. После замены команд с окончанием NI (NonInterrupt) на команды без
            // окончания NI стабильность приема значительно улучшилась.
            if ( (BufRead[0]>0) && (BufRead[0]<17) )  // Если регистр программ в допуст. диапазоне, то записываем
            {
              Reg_Prog = BufRead[0];
              Reg_But1 = BufRead[1];
              Reg_But2 = BufRead[2]; 
            }
            break;

    case 17: 
              i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // Передаем все регистры индикации
              break;
    }
    //----------- End of I2C section ------------------

    // Обработчик программы стирки    
    WashingProcess();

    
  }
}



