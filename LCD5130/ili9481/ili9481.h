// ILI9481 Header for TFT8K2346 & TFT8K5029 LCMs
// (R)soft 20/2/2014
// Version 1.0

#define PIN_D17   PORTA_Bit0  // D17
#define DDR_D17   DDRA_Bit0
#define PIN_D16   PORTA_Bit1 // D16
#define DDR_D16   DDRA_Bit1
#define PIN_RDX   PORTA_Bit2  // RDX
#define DDR_RDX   DDRA_Bit2
#define PIN_RESX  PORTA_Bit3  // RESX
#define DDR_RESX  DDRA_Bit3
#define PIN_WRX   PORTA_Bit4  // WRX
#define DDR_WRX   DDRA_Bit4
#define PIN_RS    PORTA_Bit5  // RS
#define DDR_RS    DDRA_Bit5
#define PIN_CSX   PORTA_Bit6  // CSX
#define DDR_CSX   DDRA_Bit6
#define PIN_BL    PORTA_Bit7 // Back Light ON/OFF pin
#define DDR_BL    DDRA_Bit7

#define DATA_LO   PORTB // Порт данных LOW Byte
#define PIN_DATL  PINB
#define	DDR_DATL  DDRB  

#define DATA_HI   PORTD // Порт данных HIGH Byte
#define PIN_DATH  PIND
#define	DDR_DATH  DDRD  

#define D17_1   PIN_D17=1
#define D17_0   PIN_D17=0
#define D16_1   PIN_D16=1
#define D16_0   PIN_D16=0
#define RDX_1   PIN_RDX=1
#define RDX_0   PIN_RDX=0
#define RESX_1  PIN_RESX=1
#define RESX_0  PIN_RESX=0
#define WRX_1   PIN_WRX=1
#define WRX_0   PIN_WRX=0
#define RS_1    PIN_RS=1
#define RS_0    PIN_RS=0
#define CSX_1   PIN_CSX=1
#define CSX_0   PIN_CSX=0
#define BL_ON   PIN_BL=1
#define BL_OFF  PIN_BL=0

// В зависимости от портретной или альбомной ориентации,
// нужно менять значения, также смотреть параметр LCD_ORIENTATION
#define ILI9481_PIXELY  (296)
#define ILI9481_PIXELX  (480)

#define ILI9481_WIDTH   ILI9481_PIXELY  // Ширина
#define ILI9481_HEIGHT  ILI9481_PIXELX  // Высота



// ============== Command Set ======================
#define ILI9481_NOP                               0x00
#define ILI9481_SOFTWARE_RESET                    0x01
#define ILI9481_READ_DISPLAY_ID                   0x04
#define ILI9481_READ_DISPLAY_STATUS               0x09
#define ILI9481_READ_DISPLAY_POWER_MODE           0x0A
#define ILI9481_READ_DISPLAY_MADCTL               0x0B
#define ILI9481_READ_DISPLAY_PIXEL_FORMAT         0x0C
#define ILI9481_READ_DISPLAY_IMAGE_MODE           0x0D
#define ILI9481_READ_DISPLAY_SIGNAL_MODE          0x0E
#define ILI9481_READ_DISPLAY_SELF_DIAGNOSTICS     0x0F
#define ILI9481_SLEEP_IN                          0x10
#define ILI9481_SLEEP_OUT                         0x11
#define ILI9481_PARTIAL_MODE_ON                   0x12
#define ILI9481_NORMAL_DISPLAY_MODE_ON            0x13
#define ILI9481_DISPLAY_INVERSION_OFF             0x20
#define ILI9481_DISPLAY_INVERSION_ON              0x21
#define ILI9481_GAMMA_SET                         0x26
#define ILI9481_DISPLAY_OFF                       0x28
#define ILI9481_DISPLAY_ON                        0x29
#define ILI9481_COLUMN_ADDRESS_SET                0x2A
#define ILI9481_PAGE_ADDRESS_SET                  0x2B
#define ILI9481_MEMORY_WRITE                      0x2C
#define ILI9481_COLOUR_SET                        0x2D
#define ILI9481_MEMORY_READ                       0x2E
#define ILI9481_PARTIAL_AREA                      0x30
#define ILI9481_VERTICAL_SCROLLING_DEFINITION     0x33
#define ILI9481_TEARING_EFFECT_LINE_OFF           0x34
#define ILI9481_TEARING_EFFECT_LINE_ON            0x35
#define ILI9481_MEMORY_ACCESS_CONTROL             0x36
#define ILI9481_VERTICAL_SCROLLING_START_ADDRESS  0x37
#define ILI9481_IDLE_MODE_OFF                     0x38
#define ILI9481_IDLE_MODE_ON                      0x39
#define ILI9481_INTERFACE_PIXEL_FORMAT            0x3A
#define ILI9481_READ_ID1                          0xDA
#define ILI9481_READ_ID2                          0xDB
#define ILI9481_READ_ID3                          0xDC


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
#define PURPLE_COLOR    0x780F //
#define CYAN_COLOR      0x07FF//
#define YELLOW_COLOR    0xFFE0//
#define WHITE_COLOR     0xFFFF
#define BLUE_COLOR      0x001F  //
#define NAVY_COLOR      0x000F
#define GREEN_COLOR     0x07E0//
#define RED_COLOR       0xF800//
#define BLACK_COLOR     0x0000//
#define OLIVE_COLOR     0x7BE0 //
#define ORANGE_COLOR    0xFD20//
#define PINK_COLOR      0xF81F//
