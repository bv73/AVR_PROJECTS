#include "i2c.h"

void I2C_ACK  (void)
{
  I2C_SDA_Out();  // SDA for OUT (SDA=0) Отвечаем нулем
  delay_us(4);
  I2C_SCL_In();   // SCL=1
  delay_us(4);
  I2C_SCL_Out();  // SCL=0
  delay_us(4);
}

void I2C_NACK  (void)
{
  I2C_SDA_In();   // SDA=1 Отвечаем единицей
  delay_us(4);
  I2C_SCL_In();   // SCL=1
  delay_us(4);
  I2C_SCL_Out();  // SCL=0
  delay_us(4);
}

// I2C Start =====================================
// Возвращает флаг ошибки: 0 - ОК, 0xFF - Ошибка

void I2C_Start  (void)
{
  Error_I2C=0;
  I2C_SDA_In();     // SDA for In (SDA=1)
  delay_us(1);
  I2C_SCL_In();     // SCL for In (SCL=1)
  delay_us(1);
  if (I2C_PIN&(1<<SDA_PIN) && I2C_PIN&(1<<SCL_PIN)) // Если обе ноги в единице, то ОК
    {
      I2C_SDA_Out();
      delay_us(4);
      I2C_SCL_Out();
      delay_us(4);  
    }
  else
    {
      Error_I2C=0xFF;
    }
}

//=======================================
// I2C Stop

void I2C_Stop (void)
{
  I2C_SDA_Out();
  delay_us(1);
  I2C_SCL_In();
  delay_us(1);
  I2C_SDA_In();
  delay_us(4);  
}

//=========================================
// i2c Byte Write

void I2C_Byte_Write (unsigned char byte)
{
//  __watchdog_reset();
  unsigned char counter;
  for (counter=0; counter<8; counter++)
    {
      if (byte&0x80)
        {
          I2C_SDA_In(); // SDA=1
        }
      else
        {
          I2C_SDA_Out(); // Иначе SDA=0
        }
      delay_us(4);
  
      I2C_SCL_In();  // SCL=1 Делаем клок единицей
      delay_us(4);
  
      I2C_SCL_Out(); // SCL=0
      delay_us(4);
  
      byte<<=1;  // Двигаем байтик
    }

  I2C_SDA_In(); // SDA на ввод (SDA=1)
  delay_us(4);

  I2C_SCL_In(); // SCL=1
  delay_us(4);

  Error_I2C=0xFF;
  counter=0xFF; // Счетчик для цикла опроса готовности слейва
  do
  {
    if (!(I2C_PIN&(1<<SDA_PIN))) // Ждем ACK от девайса
                                 // Если ответ=0 то ОК
    {
      Error_I2C=0; break;
    }
  }
  while (counter--);

  I2C_SCL_Out();    // SCL=0
}

// ============================================
// i2c Byte Read

unsigned char I2C_Byte_Read (void)
{
// __watchdog_reset();
 unsigned char counter;
 unsigned char result = 0;
 unsigned char mask = 0x80;
 I2C_SDA_In(); // Линию данных на вход (SDA=1)
 
 for (counter=0; counter<8; counter++)
   {
      delay_us(4);
  
      I2C_SCL_In();   // SCL for In (SCL=1)
      delay_us(4);
  
      if (I2C_PIN&(1<<SDA_PIN)) // Берем значение бита SDA
        {
          result |= mask; // Если была единица то делаем OR результата и маски
        }
      mask>>=1;   // Двигаем маску
      I2C_SCL_Out();  // Линию SCL в исходное состояние нуля (SCL=0)
   }

 return result;
}

