//-------------------------------------------
// XPT2046, TSC2046 (ADS7846) Driver
// for AVR mega2561
// Version 1.0 by (R)soft 8-03-2015

#include "integer.h"
#include <stdbool.h>

// Peripheral definitions for TSC2046

#define TSC_DELAY   64 // Delay value for 3-wire SPI

// значения при нажатии в разных углах дисплея
#define X_MIN 0x050 // 12-битное значение
#define X_MAX 0x7B8
#define Y_MIN 0x078
#define Y_MAX 0x778
#define Z_MIN 0x058
#define Z_MAX 0x4E0
#define SCALE_X 3.9500
#define SCALE_Y 6.588235
#define Z_THRESHOLD Z_MIN-10

// Pin definitions for SPI pins PORTC
#define TSC_CLK   PORTC_Bit0
#define DDR_TCLK  DDRC_Bit0
#define TSC_CSX   PORTC_Bit1
#define DDR_TCSX  DDRC_Bit1
#define TSC_DIN   PORTC_Bit2 // Data Input to touch
#define DDR_TDIN  DDRC_Bit2
#define TSC_BUSY  PORTC_Bit3
#define DDR_TBUSY DDRC_Bit3
#define TSC_DOUT  PORTC_Bit4 // Data Out from touch
#define DDR_TDOUT DDRC_Bit4
#define TSC_IRQX  PORTC_Bit5 // IRQX
#define DDR_TIRQX DDRC_Bit5

//-------------------------------------------------------------
// TSC2046 Commands:
// S A2 A1 A0 MOD S/D PD1 PD0 (PD1&PD0=0 - IRQ ENABLE) (MODE=0 12-bit)
// 1 1  0  1  0   0   0   0
#define TOUCH_X 0xD0 // 0xD8 for 8-bit resolution

// S A2 A1 A0 MOD S/D PD1 PD0
// 1 0  0  1  0   0   0   0
#define TOUCH_Y 0x90  // 0x98 for 8-bit resolution

// S A2 A1 A0 MOD S/D PD1 PD0
// 1 0  1  1  0   0   0   0
#define TOUCH_Z1 0xB0 // 0xB8 for 8-bit resolution

// S A2 A1 A0 MOD S/D PD1 PD0
// 1 1  0  0  0   0   0   0
#define TOUCH_Z2 0xC0 // 0xC8 for 8-bit resolution

// 1 0 1 0 0 1 0 0
#define TOUCH_VBAT 0xA4

// 1 1 1 0 0 1 0 0
#define TOUCH_AUX 0xE4

// 1 0 0 0 0 1 0 0
#define TOUCH_TEMP0 0x84

// 1 1 1 1 0 1 0 0
#define TOUCH_TEMP1 0xF4




//=============================================================
// Чтение байта from Touch
// Сначала MSB (первым идет старший бит)

unsigned char touch_read (void)
{
  unsigned char i;
  unsigned char result=0;
  unsigned char mask=0x80;

  for (i=0; i<8; i++)
  {
    TSC_CLK=1; // Чтение по нарастанию SCLK, данные выставились
    delay_us( TSC_DELAY );

    if (PINC&(1<<PC4)) // Берем значение бита (учесть номер порта!!!)
      {
        result |= mask; // Если была единица то делаем OR результата и маски
      }
    mask>>=1;   // Двигаем маску

    TSC_CLK=0;
    delay_us( TSC_DELAY ); 
  }
  
  return (result);
}


//===========================================
// Отправка байта to Touch

void touch_write(unsigned char byte)
{
    if (byte&0x80) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );
    
    if (byte&0x40) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    

    if (byte&0x20) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    
    
    if (byte&0x10) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    
    
    if (byte&0x08) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    
    
    if (byte&0x04) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    
    
    if (byte&0x02) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    
    
    if (byte&0x01) TSC_DIN=1;
    else           TSC_DIN=0;
    TSC_CLK=1;
    delay_us( TSC_DELAY );
    TSC_CLK=0;
    delay_us( TSC_DELAY );    

}


//--------------------------------------------------
// Инициализация Touch

static
void touch_init (void)
{
  DDR_TCLK=1; // direction output
  DDR_TCSX=1;
  DDR_TDIN=1; // direction output (data to touch)
  DDR_TBUSY=0; // input pin
  DDR_TDOUT=0; // input pin (data from touch)
  DDR_TIRQX=0; // input pin (interrupt from touch)

  TSC_BUSY=1; // pullup input pins
  TSC_DOUT=1;
  TSC_IRQX=1;
  
  TSC_CSX=1; // Исходно CSX=1
  TSC_CLK=0;
  TSC_DIN=1;
}

//--------------------------------------------------------------
// Отправка команды в xxx2046 и получение 16-битного резульатата 

unsigned short touch_getbyte (unsigned char command)  //  communication with xxx2046
{
  unsigned char dataHIGH, dataLOW;
  
  TSC_CSX=0; // Выбор кристалла
  delay_us( TSC_DELAY );
  
  touch_write(command);  // send command
  
  dataHIGH = touch_read();  // Читаем старший байт
  dataLOW  = touch_read();  // читаем младший байт

  TSC_CSX=1;
  delay_us( TSC_DELAY );
  
  return (((dataHIGH<<8 | dataLOW)>>4)&0x0FFF); // младшие 4 бита всегда нулевые
}

//----------------------------------------------------------
/*
Так как у меня было всего одно устройство на шине, то CS был закорочен на 0. 
А читать данные я начинал по сигналу PENIRQ. 
Вначале читал как ты в цикле X,Y,X,Y… на матрице в 3 дюйма вроде неплохо, 
но в один прекрасный день принесли матрицу в 15 дюймов, 
вот тут и начались проблемы, данные начали скакать, 
пальцем нажал — одни показания, стайлусом — другие. 
И тут я решил залезть на сайт TI и поискать у них решение. 
И ура, я его нашел…
Сделал все как у них написано:
1 — При запуске посылаем команду 0xD8
2 — Потом по сигналу PENIRQ начинаю считывать координату X три раза 
командами 0xD9 0xD9 0xD8, затем
3 — Потом по сигналу PENIRQ начинаю считывать координату Y три раза 
командами 0x99 0x99 0x98
4 — Одно значение по X и одно значение по Y будет неправильным, 
а по два оставшихся будут практически идентичными.

После того, как переделал все по этому алгоритму, значения стали стабильными, 
перестали плавать от силы нажатия и метода нажатия ( стайлус или палец ).

Так испробуй по сигналу CS подать команду 0xD8, а затем пункты 2-4
*/


/*
//--------------------------------------------
// Ожидание прерывания от Touch
void  touch_irq(void)
{
  unsigned char tmp;
  while(1)
  {
  HWREG(TSC_GPIO_PORT_BASE | (TSC_IRQX << 2)) = TSC_IRQX; // TSC_IRQX=1

  GPIOPinTypeGPIOInput(TSC_GPIO_PORT_BASE, TSC_IRQX); // TSC_IRQX на ввод
  tmp = GPIOPinRead(TSC_GPIO_PORT_BASE, TSC_IRQX); // Read TSC_IRQX

  if (tmp==0) break;
  
  GPIOPinTypeGPIOOutput(TSC_GPIO_PORT_BASE, TSC_IRQX); // TSC_IRQX на вывод
  GPIOPadConfigSet(TSC_GPIO_PORT_BASE, TSC_IRQX, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  };

  UARTprintf("Touch IRQ! ");
}
*/

//-------------------------------------------------------
// Значения координат прямоугольника в области нажатия
unsigned short x1_rect, x2_rect, y1_rect, y2_rect;
unsigned short x_touch, y_touch;


//-------------------------------------------------
// Опрос тача один раз и если было нажатие, то 
// вычисляем координаты прямоугольника и значения нажатия
// Return: 0 - not touched, value - сила нажатия.

unsigned short touch_poll (void)
{
  unsigned short x1, x2, x3, y1, y2, y3;
  unsigned short z1;

  //Делаем несколько пустых выборок для улучшения результата помехоустойчивости
//  touch_getbyte(TOUCH_X); // dummy read 
//  touch_getbyte(TOUCH_Y); // dummy read
  
  x1 = touch_getbyte(TOUCH_X);
  x2 = touch_getbyte(TOUCH_X);
  x3 = touch_getbyte(TOUCH_X);

  y1 = touch_getbyte(TOUCH_Y);
  y2 = touch_getbyte(TOUCH_Y);
  y3 = touch_getbyte(TOUCH_Y);
  
  
  z1 = touch_getbyte(TOUCH_Z1);
//  z2 = touch_getbyte(TOUCH_Z2);

  if (z1 <= Z_THRESHOLD) return (0); // если не нажато то выходим
  
  if (x1 < X_MIN) x1 = X_MIN;
  float xf = (x1-X_MIN)/SCALE_X;
  x_touch =(unsigned short)xf; // берем x1 как наиболее точную координату
  if (x_touch > 479) x_touch = 479;
  x_touch = 479 - x_touch; // Flip
  
  if (y2 < Y_MIN) y2 = Y_MIN;
  float yf = (y2-Y_MIN)/SCALE_Y;
  y_touch =(unsigned short)yf; // Берем y2 как наиболее точную координату
  if (y_touch > 271) y_touch = 271;        
        
//------------------------------ for debug -------------------------------------      
  sprintf(LN2,"%03x %03x %03x", x1, x2, x3);
  sprintf(LN3,"%03x %03x %03x", y1, y2, y3);
  sprintf(LN4,"%03x", z1);
  sprintf(LN5,"X=%d", x_touch);
  sprintf(LN6,"Y=%d", y_touch);
  pcf8814_update_console();  
//------------------------------------------------------------------------------
      
  return z1; // Возвращаем силу нажатия      
}

/*
//---------------------------------------
// Чтение картинки в буфер
// Параметры: координаты прямоугольника

void touch_rect_bufread (unsigned short x0, unsigned short y0, unsigned short x1, \
                          unsigned short y1)
{

  unsigned short i;
  unsigned short index = 0;
  
  ssd1963_Window(x0, y0, x1, y1);
  ssd1963_wr_cmd(SSD1963_MEMORY_READ);

//  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_IN);  // Шина данных на ввод
  
  // По даташиту ILI9481 первые два байта чтения из памяти дисплея недействительны   
  PIN_RDX=0; // // Dummy Read First Byte
  PIN_RDX=1;

  PIN_RDX=0; // // Dummy Read Second Byte
  PIN_RDX=1;

  for (i = 0; i < ((x1 - x0 +1) * (y1 - y0 +1)); i++) //+2 for dummy bytes
    { 
      PIN_RDX=0;
//      sdbufer[index++] = GPIOPinRead(PORT_DATA, 0xFF);  // Read byte
      PIN_RDX=1;

      PIN_RDX=0;
//      sdbufer[index++] = GPIOPinRead(PORT_DATA, 0xFF);  // Read byte
      PIN_RDX=1;
    }
  
//  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_OUT);  // Шина данных на вывод
  
  ssd1963_wr_cmd(SSD1963_NOP);  // Break Memory Read

//  UARTprintf("RD: %02x%02x %02x%02x\n", sdbufer[2], sdbufer[3], sdbufer[4], sdbufer[5]);
}




//---------------------------------------
// Вывод картинки из буфера
// Параметры: координаты прямоугольника

void touch_rect_bufwrite (unsigned int x0, unsigned int y0, unsigned int x1, \
                          unsigned int y1)
{

  unsigned int i;
  unsigned int index = 0;
  
  ssd1963_Window(x0, y0, x1, y1);
  ssd1963_wr_cmd(SSD1963_MEMORY_WRITE);
   
  for (i = 0; i < ((x1 - x0 +1) * (y1 - y0 +1)); i++) 
    { 
//      ssd1963_wr_dat(sdbufer[index++]); // Write HI byte
//      ssd1963_wr_dat(sdbufer[index++]); // Write LO byte
    } 

  ssd1963_wr_cmd(SSD1963_NOP); // Break Write Memory

}
*/


//-------------------------------------------------
// Опрос тача один раз и если было нажатие, то 
// выводим прямоугольник на некоторое время,
// затем возвращаем картинку на место.

void touch_setrect (void)
{
  unsigned short z;

  z = touch_poll();

  if (z > Z_THRESHOLD) // Если было нажатие, то рисуем
  {
    //IntMasterDisable();

    // Запоминаем картинку в буфере
//    touch_rect_bufread(y1_rect, x1_rect, y2_rect, x2_rect);
    
    // Выводим pixel
    // Оси X и Y тачпанели и дисплея не совпадают, поэтому подставляем зеркально
    ssd1963_putpixel(y_touch, x_touch, WHITE_COLOR);

    // Вывод значения координат в прямоугольнике
//    ssd1963_setcolor(WHITE_COLOR, RED_COLOR);
//    fontsize = SMALL_FONT;
    
/*
    sprintf(LN2,"Y:%x", y_touch);
    sprintf(LN3,"X:%x", x_touch);
    sprintf(LN4,"Z:%x", z);
    pcf8814_update_console();
*/
    
//    delay_ms(100); // Некоторая задержка, чтобы увидеть прямоугольник    
    
    // Возвращаем картинку на место
//    touch_rect_bufwrite(y1_rect, x1_rect, y2_rect, x2_rect);

    // Повторно выполняем ту же операцию чтобы картинка "выровнялась".
    // Почему нужно делать два раза - так и не понял.
    // Скорее всего при чтении данных, происходит XOR данных, поэтому
    // если сделать так два раза, то все восстанавливается.
//    touch_rect_bufread(y1_rect, x1_rect, y2_rect, x2_rect);
//    touch_rect_bufwrite(y1_rect, x1_rect, y2_rect, x2_rect);

    //IntMasterEnable();
    
  }
}


//-------------------------------------------------
// Опрос тача в цикле, пока не будет нажат

void wait_touch (void)
{

  while(!touch_poll()); // Крутимся в цикле пока не нажали

}


