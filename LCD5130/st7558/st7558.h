// ST7558 Header by (R)soft 9.5.2013
// Работа с шиной i2c
// Дисплей от Моторола C115
// Какие ноги LCD подключать - определено в хидере исходника i2c
// кроме /RESET

#define ST7558_I2C_ADDR     (0x78)  // Адрес контроллера на шине i2c

// Размеры экрана (контроллер обеспечивает 65x102)
#define ST7558_PIXELX   (96) // У контроллера этот параметр 102
#define ST7558_PIXELY   (65) // У контроллера 65
#define ST7558_ROW      (8) // Количество строк
#define ST7558_COL      (16) // Количество символов в строке

// LCD Instructions
#define ST7558_RESET        (0x03)
#define ST7558_FUNCTIONSET0 (0x20)  // PD=0, V=0, H=0
#define ST7558_FUNCTIONSET1 (0x21)  // PD=0, V=0, H=1
#define ST7558_EXT_CONTROL  (0x28)
// H=0
#define ST7558_VLCD_RANGE   (0x10)
#define ST7558_DISP_CONTROL (0x08)
#define ST7558_SET_YADDR    (0x40)
#define ST7558_SET_XADDR    (0x80)
#define ST7558_LOAD_CTRL1   (0x0F)
#define ST7558_LOAD_CTRL2   (0x09)
// H=1
#define ST7558_BOOST_STAGE  (0x08)
#define ST7558_REG_INIT1    (0x0E)
#define ST7558_REG_INIT2    (0x12)
#define ST7558_BIAS_SYSTEM  (0x10)
#define ST7558_SET_VOP      (0x80)

// 16 символов в строке, всего 8 строк текста
char CON7558 [ST7558_COL*ST7558_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LN1 CON7558
#define LN2 CON7558+ST7558_COL
#define LN3 CON7558+(ST7558_COL*2)
#define LN4 CON7558+(ST7558_COL*3)
#define LN5 CON7558+(ST7558_COL*4)
#define LN6 CON7558+(ST7558_COL*5)
#define LN7 CON7558+(ST7558_COL*6)
#define LN8 CON7558+(ST7558_COL*7)

