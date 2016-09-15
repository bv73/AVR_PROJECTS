#define ISENSE    5   // Номер канала измерения тока
#define USENSE    7
#define VREF      3.22  // Опорное напряжение (напряжение питания)
#define VIN_ZERO  2.5 // Напряжение на АЦП при нулевом токе (Вольт)
#define ADC_REF   0x0316  // Значение АЦП при нулевом токе (задать такое значение,
                          // чтобы младший разряд не прыгал)

//-----------------------------------------------------------------
// Инициализация Портов и АЦП канала измерения тока и напряжения

void acs712_init (void)
{
  // Disable Digital Pins
  // Disable ADC5 Pin
  DIDR0=(1<<ADC5D);

  // AVCC=AREF
  // MUX=ADC5
  ADMUX=(0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0);

  // Maximum prescaller
  // ADC Enable
  // Start Firtst One Conversion
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);


}

//---------------------------------------------
// Чтение 10-битного значения из АЦП по заданному каналу
// 5 - токовый канал
// 7 - канал вольтметра

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
