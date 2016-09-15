//----------------------------------
// APIS Washer AVTF104 Board
// By (R)soft
// v1.0 12/4/2015
// v1.1 22/5/2015
// For ATmega328P or ATmega88


//#define DEBUG_DISPLAY // Дебужный дисплей

#define SOUND_ON

#include <iom88.h>
#include <intrinsics.h> // for __delay procedures
#include <iomacro.h>
#include <stdio.h> // for sprintf

#include "avrlib\i2c.h"
#include "avrlib\i2c.c"

#include "prgwash.h"
#include "apis.h"

#ifdef DEBUG_DISPLAY
  #include "n6510\font5x7.h"
  #include "n6510\pcf8814.h"
  #include "n6510\pcf8814.c"
#endif //DEBUG_DISPLAY

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

  ProgramCounter = 0;
  ProgramNumber = 0;
  
/*  ProgramNumber = LoadEEPROM(0x00); // Читаем из eeprom
  // Если после включения питания номер программы = 0xFF, то EEPROM был стерт и нужно
  // установить исходное значение нуля.
  if (ProgramNumber == 0xFF) { StoreEEPROM(0x00, 0); ProgramNumber=0; }
  */
  
#ifdef DEBUG_DISPLAY
  Timer_Display = DISPLAY_CONST; // Инициализация секундного счетчика  
#endif //DEBUG_DISPLAY

//  ReceivedFlag=0; // Сброс флага принятой команды
  FlagTacho = 0;
  i2c_errors=0; // Счетчик ошибок i2c

  // Enable INT1 for IRDA (falling edge) & INT0 for ZeroCross (rising edge)
  EICRA = (1<<ISC11)|(0<<ISC10)|(1<<ISC01)|(1<<ISC00);

  // INT1 отключен - прием команд с пульта ДУ
  EIMSK = (0<<INT1)|(1<<INT0);  // Interrupt Mask Register (Enable INT1 & INT0)
  __enable_interrupt();
  
  //==============================
#ifdef DEBUG_DISPLAY
  pcf8814_init();
  sprintf(LN1,"APIS WASHER V1.0");
  pcf8814_update_console_x();
#endif // DEBUG_DISPLAY
  //==============================

  // Init Indication Registers
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
  delay_timer(1); // На секунду покажем приветствие 

  //========================= Основной цикл ================================  
  while (1)
  {

    adcLine = adc_read(6);
    adcTemperature = adc_read(7);
    temperature = TemperatureConvert( adcTemperature );
    
    //-------------- Обработка команд с пульта ДУ и клавиатуры панели --------------------
    if (KeyFlag) // Если флаг принятой команды выставлен или кнопка нажата
    {

      KeyFlag=0;

      //------------ Запуск 9 программы ---------------------      
      if (Reg_Prog==9 && Key==9 && !RepeatFlag)
      {
        ProgramStartStop(9, PROGRAM9);
      }
      
      //------------ Запуск 14 программы -----------------------      
      if (Reg_Prog==14 && Key==9 && !RepeatFlag)
      {
        ProgramStartStop(14, PROGRAM14);
      }

      //------------ Запуск 15 программы ----------------------      
      if (Reg_Prog==15 && Key==9 && !RepeatFlag)
      {
        ProgramStartStop(15, PROGRAM15);
      }

      //------------ Запуск 16 программы ----------------------      
      if (Reg_Prog==16 && Key==9 && !RepeatFlag)
      {
        ProgramStartStop(16, PROGRAM16);
      }
      
      //Temperature mode gradus/ADC
      if (Reg_Prog==6 && Key==5 && !RepeatFlag)
      {
        TempMode = ~TempMode;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Timer mode minute/hour
      if (Reg_Prog==8 && Key==6 && !RepeatFlag)
      {
        ClockMode = ~ClockMode;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Neutral ON/OFF
      if (Reg_Prog==1 && Key==8 && !RepeatFlag)
      {
        NState = ~NState;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Lock ON/OFF and Line ON/OFF
      if (Reg_Prog==1 && Key==9 && !RepeatFlag)
      {
        LockState = ~LockState;
        Timer_Buzzer=KEY_PRESSED_SHORT;
      }
      
      // Forward ON/OFF     
      if (Reg_Prog==2 && Key==9 && !RepeatFlag)
      {
        // Если НАЗАД выключено и нет автоповтора кнопки, то переключаем ВПЕРЕД
        if (!ReverseState)  { ForwardState = ~ForwardState; Timer_Buzzer=KEY_PRESSED_SHORT; }
      }

      // Reverse ON/OFF
      if (Reg_Prog==2 && Key==8 && !RepeatFlag)
      {
        if (!ForwardState)  { ReverseState = ~ReverseState; Timer_Buzzer=KEY_PRESSED_SHORT; }
      }

      // Speed Increase
      if (Reg_Prog==2 && Key==1)
      {
        if (RotorState<=0xF0) { RotorState = RotorState+5; Timer_Buzzer=KEY_PRESSED_SHORT; }
      }

      // Speed Decrease
      if (Reg_Prog==2 && Key==3)
      {
        if (RotorState>=5)  { RotorState = RotorState-5; Timer_Buzzer=KEY_PRESSED_SHORT; }
      }

      // Speed Reset to Zero
      // middle button
      if (Reg_Prog==2 && Key==2)  
      {
        if (RotorState) { RotorState=0; Timer_Buzzer=KEY_PRESSED_SHORT; }// Если не равен нулю, то обнуляем
      }

      // Speed Set to minimal rotation
      // SPIN button
      if (Reg_Prog==2 && Key==4)  
      {
        if (RotorState!=0x3C) { RotorState=0x3C; Timer_Buzzer=KEY_PRESSED_SHORT; }
      }
      
      // Speed Set to speed rotation 2
      // TERMO button
      if (Reg_Prog==2 && Key==5)  
      {
        if (RotorState!=0x50) { RotorState=0x50; Timer_Buzzer=KEY_PRESSED_SHORT; }
      }
      
      // Pomp ON/OFF
      if (Reg_Prog==4 && Key==9 && !RepeatFlag)
      {
       PompState = ~PompState; 
       Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Water ON/OFF
      if ( Reg_Prog==3 && Key==9 && !RepeatFlag) 
      {
       WaterState = ~WaterState;
       Timer_Buzzer=KEY_PRESSED_SHORT;
      }

      // Heater ON/OFF
      if (Reg_Prog==5 && Key==9 && !RepeatFlag)
      { // Блокировка нагрева, если бак пустой. Включаем только когда уровень2=0
        if ( LEVEL1==1 && LEVEL2==0 ) HeatingState = ~HeatingState;
        else HeatingState=0;  // Если уровень изменился на запрещенный, то сбросить нагрев
        if (HeatingState)  Timer_Buzzer=50; // Предупреждаем о включении нагревателя длинным писком
      }

      Key=0;  // Сброс кода клавиши для автоповтора и квитирования
    }
    //------------ End of обработка команд с пульта ДУ ------------------

    //----------========== Индикация для отладки =============---------------------
#ifdef DEBUG_DISPLAY
    Timer_Display--; // Уменьшаем счетчик дисплея
    if (Timer_Display==0) // Прошла ли секунда?
    {
      Timer_Display = DISPLAY_CONST; // Если секундный счетчик достиг нуля - перезагружаем

      // Если за прошедшую секунду было больше n ошибок, то панель I/O умерла
      if (i2c_errors>7) 
      { 
        i2c_dead = 0xFF; 
        sprintf(LN6,"PANEL DEAD!"); 
      }
      else i2c_dead = 0;

      i2c_errors = 0; // По прошествии секунды и проверок, обнуляем счетчик ошибок

      sprintf(LN2,"ADC6(L)=%03X", adcLine);

      sprintf(LN3,"ADC7(T)=%03X", adcTemperature);
      sprintf(LN4,"ROTOR=%02X", RotorState);        
      sprintf(LN5,   "L1=%01X", LEVEL1);
      sprintf(LN5+5, "L2=%01X", LEVEL2);
      sprintf(LN5+10,"L3=%01X", LEVEL3);
      sprintf(LN7,"P=%02X", Reg_Prog);

      sprintf(LN7+5,"1=%02X", Reg_But1);
      sprintf(LN7+10,"2=%02X", Reg_But2);
      sprintf(LN8,"KEY=%02X", Key);

      if (FlagTacho)  // Если тахогенератор работает
      {
        FlagTacho=0; 
        sprintf(LN6,"TACHO=%04X", Tacho);
      }

      pcf8814_update_console_x();
    }
#endif //DEBUG_DISPLAY
    //------------- End of Индикация section ------------------------------


    //--------- Обработка State регистров в основном цикле ----------------
    // Если реле нейтрали вкл., то индицируем режимами стирки
    if (NState==0)  { NEUTRAL = 0; Reg.Modes = 0; } 
    else            { NEUTRAL = 1; Reg.Modes = 0xFF; }

    if (ForwardState==0) FORWARD=0; else FORWARD=1;
    if (ReverseState==0) REVERSE=0; else REVERSE=1;
    if (ForwardState || ReverseState) Reg.Ris4 = 0xFF;
    else                              Reg.Ris4 = 0;

    if (LockState==0) LOCK=0; else LOCK=1;  // Замок люка
    if (adcLine>0x17F) Reg.Key = 0xFF;      // Если фаза есть, то индицируем ключиком
    else               Reg.Key = 0;         // иначе гасим

    if (PompState==0) { POMP = 0; Reg.Ris2 = 0; } 
    else              { POMP = 1; Reg.Ris2 = 0xFF; }

    if (WaterState==0)  { WATER = 0; Reg.Ris1 = 0; }
    else                { WATER = 1; Reg.Ris1 = 0xFF; }

    if (LEVEL1) Reg.Red1 = 0;
    else        Reg.Red1 = 0xFF;  // Уровни индицируем красными светодиодами

    if (LEVEL2) Reg.Red2 = 0;
    else        Reg.Red2 = 0xFF;

    if (LEVEL3) Reg.Red3 = 0;
    else        Reg.Red3 = 0xFF;

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
    if (HeatingState==0)  { HEATING = 0; Reg.Termo = 0; }
    else                  { HEATING = 1; Reg.Termo = 0xFF; }  // Индицируем красным термометром

    // Если флаг тахогенератора выставлен, то индицируем. Сброс флага идет в 
    // индикации значения тахо - положение ручки = 15.
    if (FlagTacho)  
    {
      Reg.Spin = 0xFF;
      if (Reg_Prog!=2) FlagTacho=0;  // Если ручка не в режиме отображения тахо, то сбрасываем флаг
                                     // для того, чтобы знак потух в других положениях ручки
    }
    else            Reg.Spin = 0; // Если флаг сброшен, то гасим знак
    
    //------------ End of обработка State регистров --------------------

    //------------ Индикация панели -----------------------------------
    // Ручка выбора программ задает режим индикации
    // MaskProg=0xFF - нормальная индикация в зависимости от положения ручки программ
    // MaskProg=0 - блокировка индикации во всех положениях ручки программ
    switch (Reg_Prog & MaskProg)
    {
    case 1:
              Reg.Seg1 = DIG_n;
              Reg.Seg2 = HEX[NState&0x01];
              Reg.Seg3 = DIG_L;
              Reg.Seg4 = HEX[LockState&0x01];
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              break;
    // Для отладки в этом режиме отображается тахогенератор и меню мотора
    case 2:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              // Если тахоген. работает, то сбрасываем флаг и отображаем значение
              if (FlagTacho)  // Флаг тахо выставлен - есть новые данные
              { 
                Reg.Seg1 = HEX[(Tacho>>12)&0x0F];
                Reg.Seg2 = HEX[(Tacho>>8)&0x0F];
                Reg.Seg3 = HEX[(Tacho>>4)&0x0F];
                Reg.Seg4 = HEX[Tacho&0x0F];
                FlagTacho=0;
              }
              else
              {
                Reg.Seg1 = DIG_S;
                Reg.Seg2 = DIG_P;
                Reg.Seg3 = HEX[(RotorState>>4)&0x0F];
                Reg.Seg4 = HEX[RotorState&0x0F];
              }
              break;
    case 3:
              Reg.Seg1 = DIG_H;
              Reg.Seg2 = DIG_2;
              Reg.Seg3 = DIG_o;
              Reg.Seg4 = HEX[WaterState&0x01];
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              break;
    case 4:
              Reg.Seg1 = DIG_P;
              Reg.Seg2 = DIG_SPACE;
              Reg.Seg3 = DIG_SPACE;
              Reg.Seg4 = HEX[PompState&0x01];
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              break;
    case 5:
              Reg.Seg1 = DIG_H;
              Reg.Seg2 = DIG_E;
              Reg.Seg3 = DIG_A;
              Reg.Seg4 = HEX[HeatingState&0x01];
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              break;
    case 6:
              Reg.Gradus = 0xFF;
              Reg.Timer = 0x00;
              Reg.Points = 0x00;
              Reg.Seg1 = DIG_SPACE;
              if (!TempMode)
              {
                Reg.Seg2 = DIG_SPACE;
                if (temperature==0xFF) { Reg.Seg3 = DIG_MINUS; Reg.Seg4 = DIG_MINUS; }
                else  { Reg.Seg3 = DEC[temperature]>>8; Reg.Seg4 = DEC[temperature]&0xFF; }
              }
              else
              {
                Reg.Seg2 = HEX[(adcTemperature>>8)&0x0F];
                Reg.Seg3 = HEX[(adcTemperature>>4)&0x0F];
                Reg.Seg4 = HEX[adcTemperature&0x0F];              
              }
              break;
    case 7:
              Reg.Seg1 = HEX[OldKey]; // Отладочный
              Reg.Seg2 = HEX[Key];
              Reg.Seg3 = HEX[RepeatFlag & 0x01];;
              Reg.Seg4 = Reg.Seg4 + 1;
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              break;
    case 8:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0xFF;
              Reg.Points = 0x80;
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
    case 9:
              Reg.Gradus = 0x00;
              Reg.Timer  = 0x00;
              Reg.Points = 0x00;
              if (ProgramNumber == 9)  // Если программа работает то отображаем PC & таймер цикла
              {
                if (HeatingState != 0) // Если включен ТЭН, то отображаем температуру нагрева
                {
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
                Reg.Seg1 = DIG_P;
                Reg.Seg2 = DIG_r;
                Reg.Seg3 = DIG_SPACE;
                Reg.Seg4 = DIG_9;
              }
              break;
    case 10:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              Reg.Seg1 = DIG_SPACE;
              Reg.Seg2 = DIG_SPACE;
              Reg.Seg3 = DIG_1;
              Reg.Seg4 = DIG_0;
              break;
    case 11:
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              Reg.Gradus = 0x00;
              Reg.Seg1 = DIG_SPACE;
              Reg.Seg2 = DIG_SPACE;
              Reg.Seg3 = DIG_1;
              Reg.Seg4 = DIG_1;
              break;
    case 12:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              Reg.Seg1 = DIG_SPACE;
              Reg.Seg2 = DIG_SPACE;
              Reg.Seg3 = DIG_1;
              Reg.Seg4 = DIG_2;
              break;
    case 13:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              Reg.Seg1 = DIG_SPACE;
              Reg.Seg2 = DIG_SPACE;
              Reg.Seg3 = DIG_1;
              Reg.Seg4 = DIG_3;
              break;
    case 14:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              if (ProgramNumber == 14)  // Если программа работает то отображаем PC & таймер цикла
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
                Reg.Seg4 = DIG_4;
              }
              break;
    case 15:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
              if (ProgramNumber == 15)  // Если программа работает то отображаем PC & таймер цикла
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
                Reg.Seg4 = DIG_5;
              }
              break;
    case 16:
              Reg.Gradus = 0x00;
              Reg.Timer = 0x00;
              Reg.Points =0x00;
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
            i2cMasterReceive(0x41, 3, BufRead); // Принимаем 3 байта
            // Фильтрация помехи по I2C. По невыясненным причинам не всегда
            // стабильно читает по I2C. После замены команд с окончанием NI (NonInterrupt) на команды без
            // окончания NI стабильность приема значительно улучшилась.
            if ( (BufRead[0]>0) && (BufRead[0]<17) )  // Если регистр программ в допуст. диапазоне, то записываем
            {
              Reg_Prog = BufRead[0];
              Reg_But1 = BufRead[1];
              Reg_But2 = BufRead[2]; 
            }
            else 
            {
              i2c_errors++;
//              Timer_Buzzer=1; // Трещим об ошибке
            }
            break;

    case 17: 
              i2cMasterSendNI(0x40, sizeof (Reg.Buf), Reg.Buf);  // Передаем все регистры индикации
              break;
    }
    //----------- End of I2C section ------------------

    
    WashingProcess();

    
  }
}



