// 39,0625 us for TIMER0 (PWM)
// 1:1 0x01 = 35,8 us
// 1:1 0x04 = 35 us
// 1:1 0x24 = 33,3 us
// 1:8 0x01 = 260,75 us
// 1:8 0x80 = 132,25 us
// 1:8 0xE0 = 35,25 us
// 1:8 0xDD = 38,5 us
// 1:8 0xDC = 39,33 us

#define TIMER_PWM_CONST  0xDC


//===========================================================================
// 8-bit Timer0 for PWM Triac Drives Main Motor
// 16-bit Timer1 for Capture Tacho
// 8-bit Timer2 for keyboard


void InitTimers (void)
{
  //=========================== TIMER0 39,33 us
  // For PWM 1:8
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:64
  //    100 - 1:256
  //    101 - 1:1024
  //    110 - Ext Clock Source falling edge
  //    111 - Ext Clock Source rising edge
  TCNT0 = TIMER_PWM_CONST;
  TCCR0A = (0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
  TCCR0B = (0<<CS02) | (1<<CS01) | (0<<CS00);
  
  //=========================== TIMER1
  // For Capture Tacho 1:64
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:64
  //    100 - 1:256
  //    101 - 1:1024
  //    110 - Ext Clock Source falling edge
  //    111 - Ext Clock Source rising edge
  TCNT1=0;
  TCCR1A = 0;
  // ICNC1=1 Noise canceler ON
  // ICES1 - Input Capture Edge Select, 0 - Falling, 1 - Rising
  TCCR1B = (1<<ICNC1) | (0<<ICES1) | (0<<CS12) | (1<<CS11) | (1<<CS10); // 1:64
  
  // ========================== TIMER2 4987 us
  // For 4166 us OCR2A=129
  // For 5022 us OCR2A=155
  // For 4987 us OCR2A=154
  // CS22 CS21 CS20 - Clock Select Prescaler
  //    000 - Timer/Counter Stopped
  //    001 - 1:1
  //    010 - 1:8
  //    011 - 1:32
  //    100 - 1:64
  //    101 - 1:128
  //    110 - 1:256
  //    111 - 1:1024

  TCCR2A=(1<<WGM21)|(0<<WGM20); // CTC mode
  TCCR2B = (1<<CS22) | (1<<CS21) | (0<<CS20); // 1:256
  OCR2A=154;
  
  // Timer0 Interrupt init (for PWM)
  TIMSK0 = (0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

  // Timer1 Interrupt Init
  // Input capture interrupt enable
  TIMSK1 = (1<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);
  
  // Timer2 Interrupt
  // TOIE2=1 Timer2 overflow Interrupt Enable
  TIMSK2 = (0<<OCIE2B) | (1<<OCIE2A) | (0<<TOIE2);  
}



//=========================================================
#pragma vector=TIMER2_OVF_vect
__interrupt void timer2_ovf(void)

{

}


//----------- KEYS --------------------------------------------------------
// Обработка клавиатуры и часов
// 5 ms timer

#pragma vector=TIMER2_COMPA_vect
__interrupt void timer2_compare(void)

{
//  RESET_LCD = ~RESET_LCD;
  msec++;
  if (msec==200) 
  { 
    msec=0;
    if (timer!=0) timer--; // Если таймер задержки не ноль, то уменьшаем
    
    sec++;
    if (sec==60)
    {
      sec=0;
      minute++;
      if (minute==60)
      {
        minute=0;
        hour++;
        if (hour==24) hour=0;
      }
    }
  }
          
  //------------- Обработчик пищалки -----------------------
#ifdef SOUND_ON
  if (Timer_Buzzer!=0)  { Timer_Buzzer--; BUZZER=1; }
  else BUZZER=0;
#endif 

  // Обработка нажатий только одиночных кнопок
  unsigned char ckey;
  switch (Reg_But1) // Преобразуем биты кнопок в коды
  {
    case 0x01: ckey = kSuper; break;
    case 0x02: ckey = kEasy; break;
    case 0x04: ckey = kIron; break;
    case 0x08: ckey = kSpin; break;
    case 0x10: ckey = kTermo; break;
    case 0x20: ckey = kTimer; break;
    case 0x40: ckey = kLock; break;
    case 0x80: ckey = kOn; break;
    default: ckey = 0x00;
  }
  if (Reg_But2==0xFF) ckey=kStart;
    
  if (ckey==OldKey && ckey!=0x00) //Если клавиша нажата и не изменилась
  {
    KeyCounter++; //Увеличиваем счетчик
    if (KeyCounter==0) 
    {
      Key=ckey;
      RepeatFlag=0;
    }
    if (KeyCounter==KEY_REPDEL) //Прошло время до начала автоповтора
    {
      KeyCounter=KEY_REPPER; //Время повторения
      Key=ckey;
      RepeatFlag=0xFF;
    }
   }
  else //Отпустили клавишу или код изменился
  {
    //дребезг
    OldKey=ckey; //Сохраняем новый код
    KeyCounter=KEY_DELAY; //Устанавливаем счетчик 50мс
  }

  if ( Key!=0 && Key==OldKey ) KeyFlag=0xFF;
    
}





// -----------------------------------------------------------------
//  TIMER1 CAPTURE INTERRUPT
// Функция обработки прерывания первого таймера в режиме захвата

#pragma vector=TIMER1_CAPT_vect
__interrupt void TC1_capture(void)
{
  unsigned int tmp = ICR1; // Сразу забираем то, что насчитали
  TCNT1=0;
  Tacho = tmp;
  FlagTacho = 0xFF;
}


