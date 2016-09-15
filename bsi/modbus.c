#include "modbus.h"

//======================================================================
// The function returns the CRC16 as a unsigned short type
//======================================================================
// Name :  CRC16
// Width : 16
// Poly :  A001
// Init :  FFFF
// Функция принимает два аргумента: 
//	unsigned int *pBuf; Указатель на буфер
//	unsigned char DataLen; Количество байт в буфере
//	Функция возвращает CRC как тип unsigned int. 

unsigned short CRC16(unsigned char *pBuf, unsigned char DataLen)
{
  unsigned char CRCHi = 0xFF;
  unsigned char CRCLo = 0xFF; 
  unsigned char Index;
  while (DataLen--)
    {
      Index = CRCLo ^ *pBuf++; // calculate the CRC
      CRCLo = CRCHi ^ Table_CRC_H[Index];
      CRCHi = Table_CRC_L[Index];
    };
  return ((CRCHi<<8) | CRCLo); 
} 

//===============================================================
// То же самое что предыдущая процедура, только сразу сохраняет
// результат CRC16 в буфере, за областью подсчета CRC16

void CRC16_And_Store(unsigned char *pBuf, unsigned char DataLength)
{
  unsigned char CRCHi = 0xFF;
  unsigned char CRCLo = 0xFF; 
  unsigned char Index;
  while (DataLength--)
    {
      Index = CRCLo ^ *pBuf++;
      CRCLo = CRCHi ^ Table_CRC_H[Index];
      CRCHi = Table_CRC_L[Index];
    };
  *pBuf++=CRCLo;
  *pBuf++=CRCHi;
}

//======================================================================
// Функция 3 протокола Modbus (0x03):
// чтение значений из нескольких регистров ввода (Read Input Registers).
// На входе unsigned char length_rx - количество принятых байт в буфере.
// Запрос:
//  0       1    2        3    4    5    6       7
// SLV_ADR FUNC HI_ADR LO_ADR HI_N LO_N  CRCLo CRCHi
// Ответ:
//  0       1    2                  
// SLV_ADR FUNC NBYTES DAT1...DATN CRCLo CRCHi (256-5)/2=125,5
// Классический пакет:
// 02 03 01 80 00 32 C4 38
// 02 03 01 40 00 1E C5 D9
// ----------------------------
// 02 03 00 0A 00 01 A4 3B -> 02 03 02 00 00 FC 44
// 02 03 00 02 00 01 25 F9 -> 02 03 02 00 00 FC 44

void Modbus_Function_3 (void)
{

  if (HowMany_RX!=8)  // Если длина пакета <> 8, то ошибка.
    {
      return; // Сразу выходим из процедуры
    }

  if (UART_Buf[4]!=0) // Если старший байт количества слов запроса <>0,
                      // то ошибка (имееется в виду очень большой запрос >512 байт).
    {
      return;
    }

//  unsigned char LenPacketLOW; // Переменная для младшего байта длины данных для ответа
//  LenPacketLOW=UART_Buf[5];
  if ( (UART_Buf[5]==0) || (UART_Buf[5]>125) )  // Если младший байт количества слов запроса > 125,
                     // или равен нулю то ошибка (запрос больше чем 125 слов считается некорректным).
                      // Формула рассчета (256-5)/2=125 слов + 1 байт
    {
      return;
    }

  unsigned int ChSum; // Переменная для подсчета контрольной суммы
  ChSum=CRC16((unsigned char *)UART_Buf,8); // Считаем CRC16 всего пакета
  if (ChSum!=0) // Если контрольная сумма всего пакета не равна нулю, то ошибка
    {
      return;
    }

  unsigned int AddrFromCopy;
  AddrFromCopy=( ((UART_Buf[2]<<8)|UART_Buf[3]) + OFFSET ); // Откуда копируем (виртуальный адрес + OFFSET)
  unsigned char Bytes;
  Bytes=(UART_Buf[5] * 2); // длину в словах превращаем в байты
  UART_Buf[2]=Bytes;      // Подготовка ответа - Длина данных в байтах

  unsigned char *p;
  p=(unsigned char *)AddrFromCopy; // Откуда копируем
  unsigned char *d=UART_Buf+3;  // Адрес, куда копируем (буфер UART)

  while (Bytes--)
    {
      *d++=*p++;
    };

//  Bytes = (UART_Buf[2] + 3); // Длина данных в байтах плюс три заголовочных
  CRC16_And_Store((unsigned char *)UART_Buf, (UART_Buf[2] + 3)); // Посчитать CRC16 с сохранением суммы в конце буфера. Длина данных в байтах плюс три заголовочных
  Transmit(UART_Buf[2] + 5); // Отправить ответ + 3 заголовочных + 2 байта CRC16
}

//=====================================================================
// Функция 6 протокола Modbus (0x06):
// запись значения в один регистр хранения (Preset Single Register).
// На входе unsigned char length_rx - количество принятых байт в буфере
// Запрос:
//  0       1    2      3      4      5     6 7
// SLV_ADR FUNC HI_ADR LO_ADR HI_DAT LO_DAT CRC
// Ответ - копия запроса:
// SLV_ADR FUNC HI_ADR LO_ADR HI_DAT LO_DAT CRC

/*
void Modbus_Function_6 (void)
{

  if (HowMany_RX!=8)  // Если длина пакета <> 8, то ошибка.
      {
        return;
      }
  
  unsigned int ChSum; // Переменная для подсчета контрольной суммы
  ChSum=CRC16((unsigned char *)UART_Buf, 8); // Считаем CRC16 всего пакета
  if (ChSum!=0) // Если контрольная сумма всего пакета не равна нулю, то ошибка
      {
        return;
      }
  
  unsigned int AddrToCopy;
  AddrToCopy=( ((UART_Buf[2]<<8)|UART_Buf[3]) + OFFSET); // Куда копируем (Виртуальный адрес + OFFSET)
  unsigned char *p;
  p=(unsigned char *)AddrToCopy;
  *p++=UART_Buf[4]; // Запись в регистр памяти первого байта
  *p=UART_Buf[5];   // Запись второго байта
  Transmit(8);  // Эхо ответ подтверждения
}
*/


//============================================================
// Функция 8 Modbus (0x08)
// На входе unsigned char length_rx - количество принятых байт в буфере
// 02 08 80 00 00 00 C9 F8

void Modbus_Function_8 (void)
{

  if (HowMany_RX!=8)  // Если длина пакета <> 8, то ошибка.
      {
        return;
      }

  unsigned int ChSum; // Переменная для подсчета контрольной суммы
  ChSum=CRC16((unsigned char *)UART_Buf, 8); // Считаем CRC16 всего пакета
  if (ChSum!=0) // Если контрольная сумма всего пакета не равна нулю, то ошибка
    {
      return;
    }

 // Проверка адреса  
  if ( (UART_Buf[2]==0) && (UART_Buf[3]==0) ) // Если ADDR=0
    {
      Transmit(8);  // То просто отвечаем
      return;   // и вываливаемся
    }

  if ( (UART_Buf[2]==0x80) && (UART_Buf[3]==0) )  // Если ADDR=0x8000 Это команда сохранения конфига в EEPROM
    {
      CRC16_And_Store((unsigned char *)(unsigned int)&BSI_Settings, sizeof(BSI_Settings)-2 ); // Считаем КС

      // Write_Config_To_EEPROM
      Write_Config_To_EEPROM (EEPROM_MAIN_SETTINGS_ADDR); // записываем конфиг в первую копию  
      Write_Config_To_EEPROM (EEPROM_RESERVED_SETTINGS_ADDR); // записываем конфиг в резервную копию

      Restart();
      Transmit(8);
      return;
    }

  if (UART_Buf[3]==2) // Если адрес = 0002
    {
      UART_Buf[4]=BSI_ResultEx.BSI_Results.Status.word; // Выдать слово флагов
      CRC16_And_Store((unsigned char *)UART_Buf,6); // Посчитать CRC16 с сохранением суммы в конце буфера
      Transmit(8); // Даем ответ
    }
}


//============================================================
// Функция 16 Modbus (0x10)
// запись значений в несколько регистров хранения (Preset Multiple Registers)
// На входе unsigned char length_rx - количество принятых байт в буфере
// Запрос:
//  0       1    2      3       4       5      6     7    X X  XX
// SLV_ADR FUNC HI_ADR LO_ADR N_RG_HI N_RG_LO NBYTE DAT1 DATN CRC (256-9)/2=123,5
// Ответ:
//  0       1    2      3      4        5     6 7
// SLV_ADR FUNC HI_ADR LO_ADR N_RG_HI N_RG_LO CRC
// 02 10 01 80 00 32 64 xx xx
// NBYTE должно быть удвоеному количеству слов (N_RG_LO)
// -----------------------------------------------
// Команда 1E
//  02 10 40 00 00 01 02 00 1E 73 6A -> 02 10 40 00 00 01 14 3A
//  Slv

void Modbus_Function_16 (void)
{

  //  unsigned char LenPacketLOW;
  if (UART_Buf[4]!=0) // Если старший байт запроса не равен нулю (запрос очень большой)
      {
        return;
      }

//  LenPacketLOW=UART_Buf[5];
  
  if ( (UART_Buf[5]==0) || (UART_Buf[5]>123) ) // Если младший байт количества слов запроса > 123,
                     // или равен нулю то ошибка (запрос больше чем 123 слова считается некорректным).
                    // Формула расчета (256-9)/2=123,5 (макс кол-во слов в пакете)
      {
        return;
      }

  unsigned char Bytes=(UART_Buf[5] * 2); //
  if (UART_Buf[6]!=Bytes) // Проверка длины запроса в словах и байтах
      {
        return;
      }

  if ( (Bytes+9)!=HowMany_RX )  // Если количество принятых байт в буфере
                                // не равно количеству байт в пакете плюс заголовочные и CRC16
      {
        return;
      }

  unsigned int ChSum; // Переменная для подсчета контрольной суммы
  ChSum=CRC16((unsigned char *)UART_Buf, Bytes+9); // Считаем CRC16 всего пакета
  if (ChSum!=0) // Если контрольная сумма всего пакета не равна нулю, то ошибка
      {
        return;
      }
  
  unsigned int AddrToCopy;
  AddrToCopy=( ((UART_Buf[2]<<8)|UART_Buf[3]) + OFFSET ); // Куда копируем (виртуальный адрес + OFFSET)

  // Проверка адреса
  // Здесь проверяется валидность AddrToCopy, если порядок, то продолжаем. Иначе вываливаемся.




  
  Bytes=UART_Buf[6]; // Сколько байт копировать

  unsigned char *p;
  p=(unsigned char *)AddrToCopy;

  unsigned char *d=UART_Buf+7; // Откуда выгребать данные
  while (Bytes--)
    {
      *p++=*d++;
    };

  CRC16_And_Store((unsigned char *)UART_Buf, 6); // Посчитать CRC16 с сохранением суммы в конце буфера
  Transmit(8);  // Эхо ответ подтверждения

}


//=====================================================================
// Процедура подсчитывает CRC16 принятого пакета, затем обрабатывает
// функции Modbus.

void Calc_Modbus (void)
{
//  UCSR0B&=~(1<<RXEN0); // Отключаем приемник
  ReceiveOK=0; // Сброс флага принятой информации

  // 1 - проверяем КС принятого пакета.
  // В HowMany_RX содержится кол-во принятых байт пакета, от него и пляшем...
  // Если посчитать КС всего пакета, включая КС пакета, то результат должен быть
  // равен нулю. На время обработки этой процедуры приемник отключен, чтобы
  // не поймать помеху или чего-то лишнего по прерыванию приемника...

//  CheckSum=CRC16((unsigned char*)UART_Buf,HowMany_RX);
  if (CRC16((unsigned char*)UART_Buf,HowMany_RX)==0)   // Если CRC16 совпало, то
    {
      switch (UART_Buf[1])  // Берем номер функции
        {
          case READ_HOLDING_REGISTERS:

                Modbus_Function_3(); // Обработка функции 3
                break;

          case DIAGNOSTIC:

                Modbus_Function_8();
                break;

          case PRESET_MULTIPLE_REGISTER:

                Modbus_Function_16();
                break;

          default: // все другие случаи - ошибка

                HowMany_RX=0;
                HowMany_TX=0;
                UCSR0B&=~(1<<TXEN0);
                UCSR0B|=(1<<RXEN0);

        }
    }

// Если CRC16 не совпало или номер функции не совпал (вторая проверка!),
// то тихонько игнорируем, отключаем передатчик и включаем приемник заново...

//  sprintf(UART_Buf,"CRC16:%04X Len:%X",sum,HowMany_RX);
//  ECHO (16);
//  sprintf(UART_Buf,"CRC16:%04X CRC in Packet: %04X Len:%X",sum,((unsigned int *)(((unsigned char*)UART_Buf)+HowMany_RX-2))[0],HowMany_RX-2);
//  ECHO(37);
  else
    {
      HowMany_RX=0;
      HowMany_TX=0;
      UCSR0B&=~(1<<TXEN0);  // Отключение передатчика
      UCSR0B|=(1<<RXEN0);   // Разрешить прием далее
    }
}







