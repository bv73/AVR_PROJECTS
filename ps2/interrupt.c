#include "interrupt.h"

//========================================================
// Таймер используется для подсчета таймаута.
// Также есть секундный регистр с флагом о том, что очередная секунда прошла.
// Настраиваем таймер так что, вызов прерывания таймера следует каждую 1 мс.

void InitTimer2 (void)
{

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

  TCCR2B=(1<<CS21)|(1<<CS20);
  
  // ========================== TIMERS/COUNTERS INTERRUPT SETUP
  // Timer Counter Interrupt Mask Register
  // (0x8C by Default 1000 1100)
  // TIMSK = TOIE1, OCIE1A, OCIE1B, OCIE2,   TICIE1, TOIE2, TOIE0, OCIE0
  // TOIE1=1 Timer1 Interrupt Enable
  // TICIE=1 Input1 Capture Interrupt Enable
  // TOIE2=1 Timer2 Interrupt Enable
  TIMSK2 = (1<<TOIE2);
  
  // ETIMSK (0x24 By Default 0010 0100)
  //  ETIMSK = -, -, TICIE3, OCIE3A,     OCIE3B, TOIE3, -, -
  // TICIE3=1 Input3 Capture Interrupt Enable
  // TOIE3=1  Timer3 Interrupt Enable
  //ETIMSK = (1<<TICIE3)|(1<<TOIE3);
  
  SecundaOK=0;
  Register_OneSec = SEK_CONST; // Инициализация секундного счетчика
  
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

  if (TimeOut_KBD!=0)
    {
      TimeOut_KBD--;
      if (TimeOut_KBD==0) // Если таймаут клавы наступил, то считаем что байт передан
        {
          bitcount=11; // После этой переустановки - прерывания по приему будут следовать с начала
        }
    }
  
  LED_counter++;
  if (LED_counter==255)
    {
//      LED_counter=0;  
      LED_PORT^=(1<<LED_4); // Дрыгаем (Debug Code)
    }

  Register_OneSec--; //  Уменьшаем счетчик для секундного таймера
  if (Register_OneSec==0) // Прошла ли секунда?
    {
      Register_OneSec=SEK_CONST; // Если секундный счетчик достиг нуля - перезагружаем
      SecundaOK=0xFF; // Устанавливаем флаг новой секунды
    }

}
