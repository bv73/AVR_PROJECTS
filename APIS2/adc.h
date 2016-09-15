//------------------------------------------------------
// Инициализация Портов и АЦП

void InitADC (void)
{
  // AVCC=AREF
  // MUX=ADC7
  ADMUX=(0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);

  // Maximum prescaller
  // ADC Enable
  // Start Firtst One Conversion
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}


//---------------------------------------------
// Чтение 10-битного значения из АЦП по заданному каналу
// 5 - вход ADC5
// 7 - вход ADC7 и т.д.

unsigned int adc_read (unsigned char channel)
{
  unsigned char tmpL, tmpH;

  PRR&=~(1<<PRADC); // Сброс бита пониженного энегргопотребления АЦП
  ADMUX&=0xF8;
  ADMUX|=channel; // Установка номера канала
  ADCSRA|=(1<<ADSC); // Запуск одного преобразования
  while (ADCSRA&(1<<ADSC)); // Ждем конца преобразования

  tmpL=ADCL;
  tmpH=ADCH;
  
  return (tmpH<<8|tmpL);
}
