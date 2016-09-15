// PWM Dimmer routines by (R)soft in April 2015
// Used 8-bit Timer0 1:8 with TCNTn=0xDC
// And INT0 for Zero Cross Detect
//=============================================


//========================================================================
// TIMER0 OVERFLOW INTERRUPT
// Управление симистором

#pragma vector=TIMER0_OVF_vect
__interrupt void TC0_ovf(void)
{
  TCNT0 = TIMER_PWM_CONST;  // Перезагрузка таймера после переполнения

  // Check if a PWM Count has reached a value
  // If so, turn on
//  RED_LED = ~RED_LED;

    if(RotorState == PWMCount) 
    {
      ROTOR=1; // Включаем симистор
    }
    PWMCount--; // Считаем, пока не совпадет
  
}

//=================================================================
// External Interrupt 0 service routine
// Zero Cross Detect

#pragma vector=INT0_vect
__interrupt void ext_int0_isr(void)

{
  TCCR0B = 0x00;  // Stop Timer
  ROTOR = 0;  // Turn Triac Off
        
  TCNT0 = TIMER_PWM_CONST;  // Load Timer presets

  PWMCount = 0xFF;// Reset the PWM Count
  TCCR0B=(0<<CS22)|(1<<CS21)|(0<<CS20);	// Start Timer
}



