// Для упрощения работы с кодом, префикс контроллера MC2PA8201
// заменен на более короткое N93 (модель телефона).
// Код также подходит для дисплеев от Nokia N71, N73.
// MC2PA8201 частично совместим с R69406, не проверено (нет даташита).
// MC2PA8201 по регистрам совместим с LDS285
// MC2PA8201(Nokia6300 & NokiaE51)

// MC2PA8201 Header
// By (R)soft 12.11.2013-15.12.2013
// Connecting LCD: PORTx - Data, PORTA - Control/debug

#define PIN_DCX   PORTA_Bit0 // DATA Command Bit: 0 - Command, 1 - Data
#define DDR_DCX   DDRA_Bit0
#define PIN_RDX   PORTA_Bit1 // Read Data (Active is LOW LEVEL)
#define DDR_RDX   DDRA_Bit1
#define PIN_WRX   PORTA_Bit2 // Write Data (Active is LOW LEVEL)
#define DDR_WRX   DDRA_Bit2
#define PIN_RESX  PORTA_Bit3  // /Reset Chip (Active is LOW LEVEL)
#define DDR_RESX  DDRA_Bit3

#define PDATA     PORTD // Порт данных
#define PIN_DAT   PIND
#define	DDR_DAT   DDRD  

#define DCX_1   PIN_DCX=1
#define DCX_0   PIN_DCX=0
#define WRX_1   PIN_WRX=1
#define WRX_0   PIN_WRX=0
#define RDX_1  	PIN_RDX=1
#define RDX_0  	PIN_RDX=0
#define RESX_1  PIN_RESX=1
#define RESX_0  PIN_RESX=0

// Размеры экрана 320x240
// В зависимости от портретной или альбомной ориентации,
// нужно менять значения, также смотреть параметр LCD_ORIENTATION
#define N93_PIXELY  (320)
#define N93_PIXELX  (240)

#define N93_WIDTH   N93_PIXELX  // Ширина
#define N93_HEIGHT  N93_PIXELY  // Высота

#define N93_ROW     (N93_PIXELY/8) // Количество строк
#define N93_COL     (N93_PIXELX/8) // Количество символов в строке


// Команды ЖКИ
#define N93_NOP                               0x00
#define N93_SOFTWARE_RESET                    0x01
#define N93_READ_DISPLAY_ID                   0x04
#define N93_READ_DISPLAY_STATUS               0x09
#define N93_READ_DISPLAY_POWER_MODE           0x0A
#define N93_READ_DISPLAY_MADCTL               0x0B
#define N93_READ_DISPLAY_PIXEL_FORMAT         0x0C
#define N93_READ_DISPLAY_IMAGE_MODE           0x0D
#define N93_READ_DISPLAY_SIGNAL_MODE          0x0E
#define N93_READ_DISPLAY_SELF_DIAGNOSTICS     0x0F
#define N93_SLEEP_IN                          0x10
#define N93_SLEEP_OUT                         0x11
#define N93_PARTIAL_MODE_ON                   0x12
#define N93_NORMAL_DISPLAY_MODE_ON            0x13
#define N93_DISPLAY_INVERSION_OFF             0x20
#define N93_DISPLAY_INVERSION_ON              0x21
#define N93_GAMMA_SET                         0x26
#define N93_DISPLAY_OFF                       0x28
#define N93_DISPLAY_ON                        0x29
#define N93_COLUMN_ADDRESS_SET                0x2A
#define N93_PAGE_ADDRESS_SET                  0x2B
#define N93_MEMORY_WRITE                      0x2C
#define N93_COLOUR_SET                        0x2D
#define N93_MEMORY_READ                       0x2E
#define N93_PARTIAL_AREA                      0x30
#define N93_VERTICAL_SCROLLING_DEFINITION     0x33
#define N93_TEARING_EFFECT_LINE_OFF           0x34
#define N93_TEARING_EFFECT_LINE_ON            0x35
#define N93_MEMORY_ACCESS_CONTROL             0x36
#define N93_VERTICAL_SCROLLING_START_ADDRESS  0x37
#define N93_IDLE_MODE_OFF                     0x38
#define N93_IDLE_MODE_ON                      0x39
#define N93_INTERFACE_PIXEL_FORMAT            0x3A
#define N93_READ_ID1                          0xDA
#define N93_READ_ID2                          0xDB
#define N93_READ_ID3                          0xDC


// 30 символов в строке, всего 40 строк текста (шрифт 8х8)
char CON_N93 [N93_COL*N93_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LINE_01 CON_N93
#define LINE_02 CON_N93+N93_COL
#define LINE_03 CON_N93+(N93_COL*2)
#define LINE_04 CON_N93+(N93_COL*3)

//-------------------------------------------------------------------------
//#define     LCD_ORIENTATION 0x00      //VERTICAL_NORMAL
//#define     LCD_ORIENTATION 0x40    //VERTICAL_MIRROR_X
//#define     LCD_ORIENTATION 0x80    //VERTICAL_MIRROR_Y
#define     LCD_ORIENTATION 0xC0    //VERTICAL_MIRROR_XY - main
//#define     LCD_ORIENTATION 0x20    //HORIZONTAL_NORMAL *
//#define LCD_ORIENTATION   0x60    //HORIZONTAL_MIRROR_X
//#define     LCD_ORIENTATION 0xA0    //HORIZONTAL_MIRROR_Y
//#define     LCD_ORIENTATION 0xE0    //HORIZONTAL_MIRROR_XY

// 24-bit colors
#define RED       0xFF0000
#define DEEPPINK  0xFF1493
#define ORANGE    0xFFA500
#define YELLOW    0xFFFF00
#define GOLD      0xFFD700
#define VIOLET    0xEE82EE
#define MAGENTA   0xFF00FF
#define BLUEVIOLET  0x8A2BE2
#define PURPLE    0x800080
#define LIME      0x00FF00
#define GREEN     0x008000
#define OLIVE     0x808000
#define TEAL      0x008080
#define CYAN      0x00FFFF
#define SKYBLUE   0x87CEEB
#define BLUE      0x0000FF
#define DARKBLUE  0x00008B
#define NAVY      0x000080
#define CHOCOLATE 0xD2691E
#define BROWN     0xA52A2A
#define WHITE     0xFFFFFF
#define DARKGREY  0xA9A9A9
#define GRAY      0x808080
#define BLACK     0x000000

// 16-bit colors
#define PURPLE_COLOR    0x780F
#define CYAN_COLOR      0x07FF
#define YELLOW_COLOR    0xFFE0
#define WHITE_COLOR     0xFFFF
#define BLUE_COLOR      0x001F
#define NAVY_COLOR      0x000F
#define GREEN_COLOR     0x07E0
#define RED_COLOR       0xF800
#define BLACK_COLOR     0x0000
#define OLIVE_COLOR     0x7BE0
#define ORANGE_COLOR    0xFD20
#define PINK_COLOR      0xF81F

// 16 bit/pixel
//-------------------------------------------------------
// R4 R3 R2 R1 | R0 G5 G4 G3 | G2 G1 G0 B4 | B3 B2 B1 B0
// 5xR:6xG:5xB

/*
// some RGB color definitions                                                 
#define Black           0x0000      //   0,   0,   0 
#define Navy            0x000F      //   0,   0, 128 
#define DarkGreen       0x03E0      //   0, 128,   0 
#define DarkCyan        0x03EF      //   0, 128, 128 
#define Maroon          0x7800      // 128,   0,   0 
#define Purple          0x780F      // 128,   0, 128 
#define Olive           0x7BE0      // 128, 128,   0 
#define LightGrey       0xC618      // 192, 192, 192 
#define DarkGrey        0x7BEF      // 128, 128, 128 
#define Blue            0x001F      //   0,   0, 255 
#define Green           0x07E0      //   0, 255,   0 
#define Cyan            0x07FF      //   0, 255, 255 
#define Red             0xF800      // 255,   0,   0 
#define Magenta         0xF81F      // 255,   0, 255 
#define Yellow          0xFFE0      // 255, 255,   0 
#define White           0xFFFF      // 255, 255, 255 
#define Orange          0xFD20      // 255, 165,   0 
#define GreenYellow     0xAFE5      // 173, 255,  47 
#define Pink                        0xF81F
*/
