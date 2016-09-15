//=======================================================================
//  TIMER3 CAPTURE INTERRUPT
// Функция обработки прерывания третьего таймера в режиме захвата

#pragma vector=TIMER3_CAPT_vect
__interrupt void TC3_capture(void)
{

  unsigned int tmpLO = ICR3; // Сразу забираем то, что насчитали
  unsigned int tmpHI = Capture_Reg_HI2;

  if (ETIFR&(1<<TOV3)) // Если TOV3=1
    {
      Faza2=0;
    }
  else
    {
      if (Period2==0)
        {
          if (Faza2)
            {
              Period2=0xFF;
              ICR2A = (tmpHI*65536)|(tmpLO);
              Faza2=0;
            }
          else  // Если Faza1=0
            {
              ICR2B = (tmpHI*65536)|(tmpLO);
              Faza2=0xFF;
              Register_TPZ2=BSI_Settings.ZTimeMeasure[1];
            }
        }
    }

  BSI_ResultEx.BSI_Results.Z_cnt[1]++;

}

//=======================================================================
//  TIMER3 OVERFLOW INTERRUPT
// Переполнение третьего таймера в режиме захвата

#pragma vector=TIMER3_OVF_vect
__interrupt void TC3_ovf(void)
{
  Capture_Reg_HI2++; // ICR32 ICR33 Инкрементируем старшее слово
}


//========================================================================
// TIMER2 OVERFLOW INTERRUPT
// Системные часы
// Прерывания с частотой следования 8e6/256/32=1,024 ms

#pragma vector=TIMER2_OVF_vect
__interrupt void TC2_ovf(void)
{
  // Обработка таймаута для приемника UART
  // Если на входе в перрывание таймаут равен нулю, то
  // считаем, что обработкой приемника уже занимаются
  // Если таймаут не равен нулю, то уменньшаем значение счетчика таймаута

  if (TimeOut_RX!=0)
    {
      TimeOut_RX--;
      if (TimeOut_RX==0)
        {
          UCSR0B&=~(1<<RXEN0); // Отключение приемника
          ReceiveOK=0xFF;  // Выставляем флаг что инфа принята
        }
    }

//  PORTD^=(1<<WDI); // Дрыгаем ногой WDI (Debug Code)

  Register_OneSec--; // RG1SL RG1SH Уменьшаем счетчик для секундного таймера
  if (Register_OneSec==0) // Прошла ли секунда?
    {
      Register_OneSec=SEK_CONST; // Если секундный счетчик достиг нуля - перезагружаем
      SecundaOK=0xFF; // Устанавливаем флаг новой секунды
    }

  Register_WDT--;
  if (Register_WDT==0)
    {
      Register_WDT = WDT_CONST; // Перезагрузка счетчика WDT
      Flag_WDT = 0xFF; // Установили флаг WDT
    }
  
  if (Register_TPZ1!=0)  // Если счетчик TPZ1 ещё не ноль, то
    {
      Register_TPZ1--;       // Уменьшаем счетчик TPZ1
      if (Register_TPZ1==0)  // Если счетчик досчитал до нуля то ERR_IZM_PER: - Ошибка - Таймаут TPZ1
        {
          Faza1=0;  // Чистим бит FAZA1
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ1us=0;     // Чистим бит FT1
          BSI_ResultEx.BSI_Results.Z_Tus[0]=0; // Чистим T_PER1
        }
    }

  if (Register_TPZ2!=0)
    {
      Register_TPZ2--;
      if (Register_TPZ2==0)
        {
          Faza2=0;
          BSI_ResultEx.BSI_Results.Status.Flag.ReadyZ2us=0;
          BSI_ResultEx.BSI_Results.Z_Tus[1]=0;
        }
    }
}

// -----------------------------------------------------------------
//  TIMER1 CAPTURE INTERRUPT
// Функция обработки прерывания первого таймера в режиме захвата

#pragma vector=TIMER1_CAPT_vect
__interrupt void TC1_capture(void)
{
  // До этого момента (сохранение ICR1) проходит 21 такт (10 команд ST и одна IN)
  unsigned int tmpLO = ICR1; // Сразу забираем то, что насчитали
  unsigned int tmpHI = Capture_Reg_HI1;
  
  if (TIFR&(1<<TOV1)) // Если TOV1=1
    {
      Faza1=0;
    }
  else  // TOV1=0
    {
      if (Period1==0)
        {
          if (Faza1)
            {
              Period1=0xFF;
              ICR1A = (tmpHI*65536)|(tmpLO);
              Faza1=0;
            }
          else  // Если Faza1=0
            {
              ICR1B = (tmpHI*65536)|(tmpLO);
              Faza1=0xFF;
              Register_TPZ1=BSI_Settings.ZTimeMeasure[0]; // Перезагружаем декрементальный счетчик
            }
        }
    }

  BSI_ResultEx.BSI_Results.Z_cnt[0]++;
}

// -----------------------------------------------------------------
//  TIMER1 OVERFLOW INTERRUPT
// Переполнение первого таймера в режиме захвата
// Расширение счетчика до 32 бит

#pragma vector=TIMER1_OVF_vect
__interrupt void TC1_ovf(void)
{
  Capture_Reg_HI1++; // ICR12 ICR13 Инкрементируем старшее слово
}
