// #include "ds1621.h"

void Start_DS1621 (void)
{
  I2C_Start();
  if (Error_I2C==0)
    {
      I2C_Byte_Write(I2C_ADDR_DS1621);
      if (Error_I2C==0)
        {
          I2C_Byte_Write(DS_Command_Start);
        }
      I2C_Stop();
    }
}

// ===================== Init DS1621

void Init_DS1621 (void)
{
  I2C_Start();
  if (Error_I2C==0)
    {
      I2C_Byte_Write(I2C_ADDR_DS1621);
      if (Error_I2C==0)
        {
          I2C_Byte_Write(DS_Command_Init);
        }
      if (Error_I2C==0)
        {
          I2C_Byte_Write(0x00);
        }  
      I2C_Stop();
    }
}

// ======== Запуск DS1621 ==============
// В связи с измененным алгоритмом опроса внутреннего термометра (выключить/включить питание),
// данная процедура не имеет смысла и отключена

/*
void Pusk_DS1621 (void)
{
  Init_DS1621();
  if (Error_I2C==0)
    {
      Start_DS1621();
    }
}
*/

// ================= Чтение из DS1621 ================================
// Читает данные температуры из DS1621 и помещает в ячейки TERH & TERL

void Read_DS1621  (void)
{
  unsigned char tmp;
  I2C_Start();
  if (Error_I2C==0)
    {
      I2C_Byte_Write(I2C_ADDR_DS1621);
      if (Error_I2C==0)
        {
          I2C_Byte_Write(DS_Command_Read);
        }
      if (Error_I2C==0)
        {
          I2C_Start();
          if (Error_I2C==0)
            {
              I2C_Byte_Write(I2C_ADDR_DS1621+1); // Для чтения адрес на 1 больше
            }
          if (Error_I2C==0)
            {
              tmp=I2C_Byte_Read();
              TerIn_HI=tmp;
              I2C_ACK();

              tmp=I2C_Byte_Read();
              TerIn_LO=tmp;
              I2C_NACK();

              Error_I2C=0x00; // Сброс переменной ошибки
            }
        }
      I2C_Stop();
    }
}

// =====================================================================
// Отладочный кусок, выводит прочитанную температуру DS1621 в UART
/*
void Send_Result_DS1621 (void)
{
 signed int temperature;

 if (Error_I2C!=0)
  {
     char message[]="Error_Reading_DS1621_";
     sprintf (UART_Buf,message);
     Transmit(sizeof(message));
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
    sprintf (UART_Buf+1,"%d %d",temperature,TerIn_2dT);

    UART_Buf[0]=' ';
    Transmit(8); 
   }
}
*/

//====================================================================
// Читает из внутреннего термометра и проверяет на ошибки,
// выставляет флаги готовности и знака температуры.
// Вычисляет |2*dT| и знак ухода температуры

void Read_Internal_T (void)
{
  Read_DS1621();
  if ( (Error_I2C) || (TerIn_LO&0x7F) ) // Если была ошибка I2C или второй байт
    // температуры кажет не 0x80 или 0x00, то ошибка
    {
      BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=0; // Сброс бита готовности измерения DS1621
      return; // И сразу выход
    }
  
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
      BSI_ResultEx.BSI_Results.Status.Flag.signTintern=1; // Если знак минус то устанавливаем флаг
    }
  else
    {
      BSI_ResultEx.BSI_Results.Status.Flag.signTintern=0;
    }

      // Модуль от два дельта Т плюс удвоенные полградуса
  TerIn_2dT=abs((deltaT*2) + double_half); // Сохранение результата с учётом удвоенного "полградуса" и модуля
  
  BSI_ResultEx.BSI_Results.Status.Flag.ReadyTintern=1; // Флаг готовности температуры
  BSI_ResultEx.BSI_Results.Tin=(TerIn_LO<<8|TerIn_HI); // Записывается в перевернутом состоянии
}


