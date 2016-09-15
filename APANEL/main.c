// APIS Panel I/O on the I2C bus (Slave)
// By (R)soft in April-May 2015
// Revision 1.1

#include <intrinsics.h> // for __ functions
#include <iom16a.h>
#include <iomacro.h>

#include "apanel.h"
#include "adc.h"
#include "timers.h"

#include "avrlib\i2c.h"
#include "avrlib\i2c.c"

// Prototypes
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);

int main( void )
{
  //============ InitPorts =========================================
  //----- Port A
  DDRA=0xF0; // POT, SW2, SW9, SW8 for input
  PORTA=0xF7; // POT no pullup
  //----- Port B
  DDRB=0xE8; // SW4, SW5, SW1, SW3 for input
  PORTB=0xFF; // Pullup
  //----- Port C
  DDRC=0xFF;
  PORTC=0xFF;
  //----- Port D
  DDRD=0x3F; // SW6 & SW7 for input
  PORTD=0xFF;  

  InitTimers();
  InitADC();
  
  //========= Init I2C Part
  i2cInit(); 
  // установка адреса устройства
  i2cSetLocalDeviceAddr(0x40, TRUE); 

  i2cSetSlaveReceiveHandler( i2cSlaveReceiveService );
  i2cSetSlaveTransmitHandler( i2cSlaveTransmitService );
    
  FlagSecunda=0;
  FlagHalf=0;
  FlagQuadro=0;

  DynamicStage=0;
  Reg_Secunda = SEK_CONST; // Инициализация секундного счетчика  
  Reg_Half = HALF_CONST;
  Reg_Quadro = QUADRO_CONST;
  
  __enable_interrupt();

  // Если мастер не передает данные индикации, то панель будет светить Err9 (Ошибка 9)
  // и будет мигать двоеточие.
  COM0.byte=0x7F;
  Reg.Seg1=DIG_E;
  Reg.Seg2=DIG_r;
  Reg.Seg3=DIG_r;
  Reg.Seg4=DIG_9;
  Reg.Red1=0x00;
  Reg.Red2=0x00;
  Reg.Red3=0x00;
  Reg.Ris1=0x00;
  Reg.Ris2=0x00;
  Reg.Ris3=0x00;
  Reg.Ris4=0x00;
  Reg.Modes=0x00;
  Reg.Spin=0x00;
  Reg.Termo=0x00;
  Reg.Timer=0x00;
  Reg.Lock=0x00;
  Reg.Key=0x00;
  Reg.Points=0x80;  // Мигание двоеточия
  Reg.Buzzer=0x00;
  Reg.Gradus=0x00;
  
  while (1)
  {
    __no_operation();
  }  
}


//---------------------------------------------------------------------------------------
// Мастер шлет данные - помещаем их в буфер индикации
void i2cSlaveReceiveService(unsigned char receiveDataLength, unsigned char* receiveData) 
{ 
  unsigned char i;

  for(i=0; i<receiveDataLength; i++) 
  { 
     Reg.Buf[i] = *receiveData++; 
  } 
} 

//----------------------------------------------------------------------------------------------
// Пришел запрос на чтение от мастера - отсылаем три байта переключателя режимов и кнопок
unsigned char i2cSlaveTransmitService(unsigned char transmitDataLength, unsigned char* transmitData) 
{ 

  *transmitData++ = Reg_Prog;
  *transmitData++ = Reg_But1.byte;
  *transmitData++ = Reg_But2;

  // return the number of bytes written
  return 3;
} 


