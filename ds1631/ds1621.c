// #include "ds1621.h"

void start_DS1621 (void)
{
  i2c_start();
  if (!error_i2c)
    {
      i2c_byte_write(I2C_ADDR_DS1621);
      if (!error_i2c)
        {
          i2c_byte_write(DS_Command_Start);
        }
      i2c_stop();
    }
}

// ===================== Init DS1621

void init_DS1621 (void)
{
  i2c_start();
  if (!error_i2c)
    {
      i2c_byte_write(I2C_ADDR_DS1621);
      if (!error_i2c)
        {
          i2c_byte_write(DS_Command_Init);
        }
      if (!error_i2c)
        {
          i2c_byte_write(0x00);
        }  
      i2c_stop();
    }
}

// ======== Запуск DS1621 ==============
// В связи с измененным алгоритмом опроса внутреннего термометра (выключить/включить питание),
// данная процедура не имеет смысла и отключена


void pusk_DS1621 (void)
{
  init_DS1621();
  if (!error_i2c)
    {
      start_DS1621();
    }
}


// ================= Чтение из DS1621 ================================
// Читает данные температуры из DS1621 и помещает в ячейки TERH & TERL

void read_DS1621  (void)
{
  unsigned char tmp;
  i2c_start();
  if (!error_i2c)
    {
      i2c_byte_write(I2C_ADDR_DS1621);
      if (!error_i2c)
        {
          i2c_byte_write(DS_Command_Read);
        }
      if (!error_i2c)
        {
          i2c_start();
          if (!error_i2c)
            {
              i2c_byte_write(I2C_ADDR_DS1621+1); // Для чтения адрес на 1 больше
            }
          if (!error_i2c)
            {
              tmp=i2c_byte_read();
              TerIn_HI=tmp;
              i2c_ack();

              tmp=i2c_byte_read();
              TerIn_LO=tmp;
              i2c_nack();

              error_i2c=0x00; // Сброс переменной ошибки
            }
        }
      i2c_stop();
    }
}

// =====================================================================
// Отладочный кусок, выводит прочитанную температуру DS1621

void send_result_DS1621 (void)
{
 float temperature;

 if (error_i2c)
  {
     sprintf (LIN1,"Error_I2C");
//     sed15b1_update_console();
  }
 else
  {
    if (TerIn_LO==0)
       {
         temperature=TerIn_HI*100;
       }
    else
       {
         temperature=(TerIn_HI*100)+50;
       }
    sprintf (LIN3,"T=%04.01f",temperature/100);
    sprintf (LIN4,"Два дельта Т=%d",TerIn_2dT);
//    sed15b1_update_console();
   }
}


//====================================================================
// Читает из внутреннего термометра и проверяет на ошибки,
// выставляет флаги готовности и знака температуры.
// Вычисляет |2*dT| и знак ухода температуры

void read_internal_T (void)
{
  read_DS1621();
//  if ( (Error_I2C) || (TerIn_LO&0x7F) ) // Если была ошибка I2C или второй байт
    // температуры кажет не 0x80 или 0x00, то ошибка
//    {
//      BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=0; // Сброс бита готовности измерения DS1621
//      return; // И сразу выход
//    }
  
  unsigned char double_half;
  signed char deltaT;
  deltaT = (TerIn_HI - REPER_POINT); // Дельта Т вычисляется на основании реперной точки
  
  if (TerIn_LO==0x80)
    {
      double_half=1; // Если было 0.5 градуса то к удвоенной дельте прибавить 1
    }
  else
    {
      double_half=0; // Если НЕ было 0.5 градуса то к удвоенной дельте прибавить 0
    }

  if (deltaT<0)
    {
      signTintern=1; // Если знак минус то устанавливаем флаг
    }
  else
    {
      signTintern=0;
    }

      // Модуль от два дельта Т плюс удвоенные полградуса
  TerIn_2dT=abs((deltaT*2) + double_half); // Сохранение результата с учётом удвоенного "полградуса" и модуля
  
//  Tin=(TerIn_LO<<8|TerIn_HI); // Записывается в перевернутом состоянии
}


