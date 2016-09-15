//unsigned char __eeprom *zp;// = (unsigned char __eeprom *) 0x00;

#define MIN_TEMPERATURE (0)   // Мин. температура, выставляемая юзером
#define MAX_TEMPERATURE (70)  // Макс. температура, выставляемая юзером
#define MIN_SPIN        (0)   // Мин. обороты отжмиа
#define MAX_SPIN        (8)   // Макс. обороты отжима (8 соответствует 800 об/мин)

unsigned char __eeprom EE_tProgram8;    // температура нагрева для 8 программы
unsigned char __eeprom EE_tProgram9;    // температура нагрева для 9 программы
unsigned char __eeprom EE_rpmProgram8;  // обороты отжима для программы 8
unsigned char __eeprom EE_rpmProgram9;  // обороты отжима для программы 9
unsigned char __eeprom EE_rpmProgram12; // обороты отжима для программы 12
unsigned char __eeprom EE_rpmProgram13; // обороты отжима для программы 13
unsigned char __eeprom EE_rpmProgram14; // обороты отжима для программы 14
unsigned char __eeprom EE_rpmProgram15; // обороты отжима для программы 15

//---------------------------------------------------
// Проверка и инициализация EEPROM.
// Если после стирания проца данные в EEPROM похерились, то нужно записать
// правильные значения по умолчанию.
void InitEEPROM (void)
{
  unsigned char __eeprom *zp = 0;
  unsigned char byte = *zp;       // читаем байт из EEPROM
  if (byte==0xFF)                 // Если 0xFF то память не инициализирована
  {
    __disable_interrupt();
    *zp++ = 10; // Записываем правильное значение t for program 8
    *zp++ = 10; // Записываем правильное значение t for program 9
    *zp++ = 3;  // Обороты отжима для программы 8
    *zp++ = 3;  // Обороты отжима для программы 9
    *zp++ = 4;  // Обороты отжима для программы 12
    *zp++ = 4;  // Обороты отжима для программы 13
    *zp++ = 5;  // Обороты отжима для программы 14
    *zp++ = 4;  // Обороты отжима для программы 15
    __enable_interrupt();
  }
}


//----------------------------------------------------------------------------
// Store to EEPROM
void StoreEEPROM (unsigned char __eeprom *address, unsigned char byte)
{
  __disable_interrupt();
  *address = byte;
  __enable_interrupt();  
}

//---------------------------------------------------------------------------
// Load from EEPROM
unsigned char LoadEEPROM (unsigned char __eeprom *address)
{
  unsigned char byte;

  byte = *address;

  return byte;
}


