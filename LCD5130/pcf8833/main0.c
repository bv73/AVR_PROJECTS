#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
//#include <string.h> // for memset & memcpy
#include <pgmspace.h> // for memcpy_P
#include <stdlib.h>   // for srand

#define ABS(n) (((n) < 0) ? -(n) : (n))
#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font\font5x7.h"
#include "font\arial16.h"
#include "font\arial32.h"

#include "pcf8814\pcf8814.h"
#include "pcf8814\pcf8814.c"

#include "pcf8833\fonts.h"
#include "pcf8833\pcf8833.h"
#include "pcf8833\pcf8833.c"

//подключение 1Wire библиотеки
#include "..\OWI\OWISWBitFunctions.c"
#include "..\OWI\OWIHighLevelFunctions.c"

// Порт опроса датчика определен в фале OWIPolled.h
//номер вывода, к которому подключен датчик
#define BUS   OWI_PIN_7

//команды датчика DS18B20
#define DS18B20_CONVERT_T         0x44
#define DS18B20_READ_SCRATCHPAD   0xBE

unsigned char scratchpad[9];


void main( void )
{
  unsigned char temperature;
  unsigned char counter=0;
  unsigned int tmp = 0;

  OWI_Init(BUS);  

  pcf8814_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана
  pcf8833_init();    
  pcf8833_cls(BLACK); 

  srand(0x2345);
  
//  pcf8833_pixel(50,60,RED);
//  pcf8833_putstr("Nokia 6100",10,10,LARGE,BLUE,YELLOW);
//  pcf8833_putstr("LCD",58,26,LARGE,PINK,GREEN);
//  pcf8833_putstr("Colour",10,26,MEDIUM,RED,ORANGE);  
//  pcf8833_putstr("Temperature",5,46,LARGE,WHITE,BLACK);  
//  pcf8833_putstr("PCF8833 CONTROLLER",2,114,SMALL,WHITE,BLACK);  
//  pcf8833_draw(120,10,5,105,GREEN);
  pcf8833_setrect(10,10,60,40,1,BLUE);
  pcf8833_setrect(4,63,92,96,0,WHITE);
  unsigned char circle_x, circle_y, circle_r;
  unsigned int color;
  for (tmp=0;tmp<100;tmp++)
  {
    circle_x = ABS( rand() % 132);
    circle_y = ABS( rand() % 132);
    circle_r = ABS( rand() % 80);
    color = ABS( rand());

    pcf8833_circle(circle_x,circle_y,circle_r,color);
  }

  
//  pcf8833_putchar32('0',64,64,WHITE,BLUE);
  
  pcf8814_clear_console(0x20); // Чистим пробелами

/*
  sed15b1_putchar32(0x30,0,0); // char,x,y
  sed15b1_putchar16(0x31,0,4);
  sed15b1_putchar16(0x32,16,4);  
  sed15b1_putchar32(0x33,30,4);    
  sed15b1_putchar32(0x34,50,2);
  sed15b1_putchar32(0x35,32,0);
  sed15b1_putchar16(0x36,54,0);
  sed15b1_putchar16(0x37,22,1);
  sed15b1_putchar32(0x38,72,1);
  sed15b1_putchar16('9',10,6);
  sed15b1_putchar32('!',52,5);  
  sed15b1_putchar32('+',70,5);  
*/
  
  //  unsigned char id1,id2,id3,id4;
//  sprintf(LIN5,"Hello!");
//  sprintf(LIN5,"ID1=%02X",id1);
//  sprintf(LIN2,"ID2=%02X",id2);
//  sprintf(LIN3,"ID3=%02X",id3);
//  sprintf(LIN4,"ID4=%02X",id4);

//  sed15b1_update_console();
  
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char reg=0x30;

  while (1)
  {

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

        
    sprintf(LN1,"Debug:%02X",scratchpad[0]);
    sprintf(LN1+8,"%02X",scratchpad[1]);
    sprintf(LN1+10,"%02X",scratchpad[2]);
    sprintf(LN1+12,"%02X",scratchpad[3]);
    sprintf(LN1+14,"%02X",scratchpad[4]);    
    sprintf(LN2+6,"%02X",scratchpad[5]);
    sprintf(LN2+8,"%02X",scratchpad[6]);
    sprintf(LN2+10,"%02X",scratchpad[7]);
    sprintf(LN2+12,"%02X",scratchpad[8]);
    

    // выводим знак и преобразуем число, если оно отрицательное
    if ((scratchpad[1]&128) == 0)
          {
           sprintf(LN3,"+");
          }
    else
          {
            sprintf(LN3,"-");
            tmp = ((unsigned int)scratchpad[1]<<8)|scratchpad[0];
            tmp = ~tmp + 1;
            scratchpad[0] = tmp;
            scratchpad[1] = tmp>>8;  
          }
      
    //выводим значение целое знач. температуры
    temperature = (scratchpad[0]>>4)|((scratchpad[1]&7)<<4);
    sprintf(LN3+1,"%02d",temperature);
      
    //выводим дробную часть знач. температуры
    temperature = (scratchpad[0]&15);
    temperature = (temperature<<1) + (temperature<<3);
    temperature = (temperature>>4);
    sprintf(LN3+3,".");
    sprintf(LN3+4,"%d",temperature);
    sprintf(LN2,"[%02X]",counter);
    counter++;

    //Вместо апдейта консоли
    
    pcf8814_putchar32(CON8814[32+0],0,2);  // Sign
    pcf8814_putchar32(CON8814[32+1],20,2);  // Целое значение
    pcf8814_putchar32(CON8814[32+2],41,2);
    pcf8814_putchar32(CON8814[32+3],62,2); // Точка    
    pcf8814_putchar32(CON8814[32+4],71,2); // Остаток
    
    pcf8833_putchar32(CON8814[32+0],5,64,RED,BLUE);
    pcf8833_putchar32(CON8814[32+1],25,64,WHITE,BLUE);
    pcf8833_putchar32(CON8814[32+2],45,64,WHITE,BLUE);
    pcf8833_putchar32(CON8814[32+3],65,64,WHITE,BLUE);    
    pcf8833_putchar32(CON8814[32+4],72,64,WHITE,BLUE);

    //    pcf8814_update_console(); 

    
    if (!KEY1)
      {
        reg--;

        sprintf(LN8,"Contrast:%02X",reg);
        pcf8833_cmd(P_SETCON); 
        pcf8833_data(reg); // contrast

        pcf8814_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        reg++;

        sprintf(LN8,"Contrast:%02X",reg);
        pcf8833_cmd(P_SETCON); 
        pcf8833_data(reg); // contrast

        pcf8814_update_console();

        delay_ms(100);
      }

  }
}
