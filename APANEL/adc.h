//------------------------------------------------------
// Инициализация Портов и АЦП

void InitADC (void)
{
  // AVCC=AREF
  // MUX=ADC3
  ADMUX=(0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(1<<MUX0);

  // Maximum prescaller
  // ADC Enable
  // Start Firtst One Conversion
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}


//-----------------------------------
// Чтение 10-битного значения из АЦП

unsigned int ReadADC (void)
{
  unsigned char tmpL, tmpH;

  ADMUX=(0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(1<<MUX0);
  ADCSRA|=(1<<ADSC); // Запуск одного преобразования
  while (ADCSRA&(1<<ADSC)); // Ждем конца преобразования

  tmpL=ADCL;
  tmpH=ADCH;
  
  return (tmpH<<8|tmpL);
}

//-----------------------------------------------------
// Значения АЦП поворотного переключателя программ
// Номер программы - значение
// 1  - 0x220
// 2  - 0x266
// 3  - 0x2A9
// 4  - 0x2EE
// 5  - 0x331
// 6  - 0x377
// 7  - 0x3BB
// 8  - 0x3FF
// 9  - 0x000
// 10 - 0x043
// 11 - 0x087
// 12 - 0x0CB
// 13 - 0x110
// 14 - 0x154
// 15 - 0x198
// 16 - 0x1DD

