#include <iom2561.h>
#include <iomacro.h>
#include <intrinsics.h>

#include "ad7714.h"
#include "ad7714.c"

#define LED1  PORTD_Bit6
#define LED2  PORTD_Bit7
#define ON    1
#define OFF   0

void Error (void)
{
      while (1)
      {
        LED1=ON;
        LED2=OFF;
        delay_ms(100);
        LED1=OFF;
        LED2=ON;
        delay_ms(100);
      }
}

void main( void )
{
  DDRD_Bit6=1; // Порт светодиода на вывод
  DDRD_Bit7=1;
  
  LED1=ON;
  delay_ms(100);
  LED1=OFF;
  delay_ms(500);
  
  ErrorFlag_7714=0; // Флаг ошибки обнуляем
  Init_7714(); // Настройка порта и SPI

  AD7714_PORT &= ~(1<<CS_7714); // CS=0
  delay_ms(5);
  
  Reset_7714(); // Программный сброс АЦП
  ADC7714_Channel=7;  // AIN6/AIN6

  // === Register 2 === (RS2..RS0 = 0,1,0)
  // Bit7 B/U  - Bipolar/Unipolar Mode (=1 - Unipolar Mode)
  // Bit6 WL - Word Length (=1 - 24 Bit)
  // Bit5 BST - Current Boost (=0 - Gain 1..4 on 2,4576 MHz)
  // Bit4 ZERO - 0
  // Bit3..Bit0 - FS11..FS8 Filter Selection (0xF)
  WriteReg_7714(0x02, 0xCF); // Регистр 2 - верхний регистр фильтра, байт =0x4F (7бит=0-биполярн/1-униполярн)
  if (ErrorFlag_7714)
    {
      Error();
    }

  WriteReg_7714(0x03, 0xA0); // Регистр 3 - нижний регистр фильтра, (0x0FA0=4000)


  // === Register 1 === (RS2..RS0 = 0,0,1)
  // Bit7..Bit5 - MD2..MD0 Mode 001 - Self-Calibration
  // Bit4..Bit2 - Gain ( 000 = Gain 1)
  // Bit1 BO - Burnout Current (=0 - OFF)
  // Bit0 FSYNC - Filter Synchronization (=0 - OFF)
  WriteReg_7714(0x01, 0x20); // Регистр 1 - регистр режима, (0x20 - включение автокалибровки)
  
  for (;;)
  {
    LED1 = ~LED1;
//    delay_ms(100);
    Read_ADC_7714();
    if (ErrorFlag_7714)
      {
        Error();
      }
  }
  
  
}
