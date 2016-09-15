//----------------------------------
// VoltAmpermeter by (R)soft
// 21 Nov 2013
// v1.0 24 Nov 2013
// v1.1 26 Nov 2013
// v2.0 15.12.2013 - добавлен код барометра
// v2.1 7/5/2015 - добавлена кнопка смены режима, выкинул вольтметр & амперметр

#include <iom88pa.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include "delay.h"

/*
#define delay_us(V) __delay_cycles(V*8L) // Внутренний кварц 8 МГц

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}
*/

#include "font5x7.h"

#include "sed15b1.h"
#include "sed15b1.c"

#include "i2c.h"
#include "i2c.c"
#include "mpl3115a2.c"

//подключение 1Wire библиотеки
#include "OWISWBitFunctions.c"
#include "OWIHighLevelFunctions.c"

// Порт опроса датчика определен в фале OWIPolled.h
//номер вывода, к которому подключен датчик
#define BUS   OWI_PIN_2 // PORTC

//команды датчика DS18B20
#define DS18B20_CONVERT_T         0x44
#define DS18B20_READ_SCRATCHPAD   0xBE

#define KEY1  PIND_Bit5
#define KEY2  PINC_Bit1

unsigned char scratchpad[9];


void main( void )
{
  // При каждом включении питания режим барометра читается из EEPROM
  unsigned int eeprom_addr=0;
  unsigned char __eeprom *zp=(unsigned char __eeprom *)eeprom_addr;
  unsigned char barometer_mode=*zp; // Читаем режим барометра из EEPROM
  
  unsigned char temperature;
  unsigned char counter=0;
  unsigned int tmp = 0;

  // Set Mode PRESSURE or ALTIMETER
//  unsigned char barometer_mode=ALTIMETER;

  PORTC = 0xFF;
  DDRC  = 0xFD; // KEY2 Bit1 for Input
  PORTD = 0xFF;
  DDRD  = 0xDF; // KEY1 Bit5 for Input
  
  i2c_init();        // join i2c bus
  if (barometer_mode==PRESSURE)
  {
    setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  }
  else
  {
    setModeAltimeter(); // Measure altitude above sea level in meters
  }
  setOversampleRate(7); // Set Oversample to the recommended 128
  enableEventFlags(); // Enable all three pressure and temp event flags

  OWI_Init(BUS);  
  sed15b1_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана
  sed15b1_clear_console(0x20); // Чистим пробелами

  while (1)
  {
    if (!KEY1)  // Если кнопка смены режима нажата
    {
      barometer_mode = ~barometer_mode; // Меняем значение режима
      *zp=~(barometer_mode);            // и записываем его в EEPROM
      if (barometer_mode==PRESSURE) setModeBarometer();
      else  setModeAltimeter();
      sprintf(LIN1,"KEY1");
      delay_ms(100); // Чтобы кнопка срабатывала с задержкой      
    }
    
    OWI_DetectPresence(BUS);     // Команда сброса шины
    OWI_SkipRom(BUS);                     // адресации всех устройств на шине
    OWI_SendByte(DS18B20_CONVERT_T ,BUS); // запуcк преобразования
    
    while (!OWI_ReadBit(BUS)); // Ожидание конца преобразования

    OWI_DetectPresence(BUS);      // Сброс шины
    OWI_SkipRom(BUS);             //    OWI_ReadRom(scratchpad,BUS);
    OWI_SendByte(DS18B20_READ_SCRATCHPAD, BUS); // Чтение памяти датчика в массив
    
    // чтение температуры побайтно    
    scratchpad[0] = OWI_ReceiveByte(BUS); // Temperature LSB
    scratchpad[1] = OWI_ReceiveByte(BUS); // Temperature MSB
    scratchpad[2] = OWI_ReceiveByte(BUS); // TH
    scratchpad[3] = OWI_ReceiveByte(BUS); // TL
    scratchpad[4] = OWI_ReceiveByte(BUS); // Config Register
    scratchpad[5] = OWI_ReceiveByte(BUS); // Reserved
    scratchpad[6] = OWI_ReceiveByte(BUS); // Reserved
    scratchpad[7] = OWI_ReceiveByte(BUS); // Reserved
    scratchpad[8] = OWI_ReceiveByte(BUS); // CRC

/*        
    sprintf(LIN1,"Debug:%02X",scratchpad[0]);
    sprintf(LIN1+8,"%02X",scratchpad[1]);
    sprintf(LIN1+10,"%02X",scratchpad[2]);
    sprintf(LIN1+12,"%02X",scratchpad[3]);
    sprintf(LIN1+14,"%02X",scratchpad[4]);    
    sprintf(LIN2+6,"%02X",scratchpad[5]);
    sprintf(LIN2+8,"%02X",scratchpad[6]);
    sprintf(LIN2+10,"%02X",scratchpad[7]);
    sprintf(LIN2+12,"%02X",scratchpad[8]);
*/    

    sprintf(LIN3,"T1=");
    // выводим знак и преобразуем число, если оно отрицательное
    if ((scratchpad[1]&128) == 0)
          {
           sprintf(LIN3+3,"+");
          }
    else
          {
            sprintf(LIN3+3,"-");
            tmp = ((unsigned int)scratchpad[1]<<8)|scratchpad[0];
            tmp = ~tmp + 1;
            scratchpad[0] = tmp;
            scratchpad[1] = tmp>>8;  
          }
      
    //выводим значение целое знач. температуры
    temperature = (scratchpad[0]>>4)|((scratchpad[1]&7)<<4);
    sprintf(LIN3+4,"%02d",temperature);
      
    //выводим дробную часть знач. температуры
    temperature = (scratchpad[0]&15);
    temperature = (temperature<<1) + (temperature<<3);
    temperature = (temperature>>4);
    sprintf(LIN3+6,".");
    sprintf(LIN3+7,"%d\xB7\x43",temperature);
//    sprintf(LIN2,"[%02X]",counter);
    counter++;
    if (counter==0)
    {
      sprintf(LIN1,"    ");      
    }

    if (barometer_mode==PRESSURE)
      {
        float pressure = readPressure(); // Читаем давление
        pressure=pressure/133.3; // 1 mmHg = 133.3 Pa
        float delta = pressure - 760.0; // Вычисление дельты давления
        sprintf(LIN4,"P=%05.2f mmHg",pressure);
        sprintf(LIN5,"\xB6P=%05.2f mmHg",delta);
      }
    else
      {
        float altitude = readAltitude();
        sprintf(LIN4,"Alt=%06.1f m",altitude);
      }

    float temperature = readTemp();
    sprintf(LIN6,"T2=%04.2f\xB7\x43",temperature);


    // Остальное выводится автоматически из буфера строк    
    sed15b1_update_console(); 

  }
}
