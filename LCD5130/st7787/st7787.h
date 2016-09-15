// ST7787 Header
// By (R)soft 13.5.2013
// Connectiong LCD: PORTC - Control,
// PORTB - HI Byte, PORTA - LOW Byte

#define PIN_CSX   PORTC_Bit0 // Chip Select (Active is LOW LEVEL)
#define DDR_CSX   DDRC_Bit0
#define PIN_DCX   PORTC_Bit1 // DATA Command Bit: 0 - Command, 1 - Data
#define DDR_DCX   DDRC_Bit1
#define PIN_WRX   PORTC_Bit2 // Write Data (Active is LOW LEVEL)
#define DDR_WRX   DDRC_Bit2
#define PIN_RDX   PORTC_Bit3 // Read Data (Active is LOW LEVEL)
#define DDR_RDX   DDRC_Bit3
#define PIN_RESX  PORTC_Bit4  // /Reset Chip (Active is LOW LEVEL)
#define DDR_RESX  DDRC_Bit4

#define PD_LO   PORTA // Порт данных LOW byte
#define PIN_LO  PINA
#define	DDRD_LO DDRA  // Для вывода в порт данных, байт нужно предварительно инвертировать

#define PD_HI   PORTB // Порт данных HIGH byte
#define PIN_HI  PINB
#define DDRD_HI DDRB

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define CSX_1  	DDR_CSX=0
#define CSX_0  	DDR_CSX=1
#define DCX_1   DDR_DCX=0
#define DCX_0   DDR_DCX=1
#define WRX_1   DDR_WRX=0
#define WRX_0   DDR_WRX=1
#define RDX_1  	DDR_RDX=0
#define RDX_0  	DDR_RDX=1
#define RESX_1  DDR_RESX=0
#define RESX_0  DDR_RESX=1

// Размеры экрана 320x240x18
#define ST7787_PIXELX  (320)
#define ST7787_PIXELY  (240)
#define ST7787_ROW     (ST7787_PIXELX/8) // Количество строк 40
#define ST7787_COL     (ST7787_PIXELY/8) // Количество символов в строке 30

// Команды ЖКИ
// System Function Command List 1
#define ST7787_SWRESET    (0x01)  // Software Reset
#define ST7787_RDDID      (0x04)  // ID Read
#define ST7787_RDDST      (0x09)  // Read Display Status
#define ST7787_RDDPM      (0x0A)  // Read Display Power Mode
#define ST7787_RDDMADCTL  (0x0B)  // Read Display MADCTL
#define ST7787_RDDCOLMOD  (0x0C)  // Read Display Pixel Format
#define ST7787_RDDIM      (0x0D)  // Read Display Image Mode
#define ST7787_RDDSM      (0x0E)  // Read Display Signal Mode
#define ST7787_RDDSDR     (0x0F)  // Read Display Self-diagnostic Result
// System Function Command List 2
#define ST7787_SLPIN      (0x10)  // Sleep In & Booster Off
#define ST7787_SLPOUT     (0x11)  // Sleep Out & Booster On
#define ST7787_PTLON      (0x12)  // Partial Mode On
#define ST7787_NORON      (0x13)  // Partial Mode Off (Normal Mode)
#define ST7787_INVOFF     (0x20)  // Display Inversion Off (Normal)
#define ST7787_INVON      (0x21)  // Display Inversion On
#define ST7787_GAMSET     (0x26)  // Gamma Curve Select
#define ST7787_DISPOFF    (0x28)  // Display Off
#define ST7787_DISPON     (0x29)  // Display On
#define ST7787_CASET      (0x2A)  // Column Address Set (X)
#define ST7787_RASET      (0x2B)  // Row Address Set (Y)
#define ST7787_RAMWR      (0x2C)  // Memory Write
#define ST7787_RAMRD      (0x2E)  // Memory Read
// System Function Command List 3
#define ST7787_PTLAR      (0x30)  // Partial Start/End Address Set
#define ST7787_SCRLAR     (0x33)  // Scroll Area Set
#define ST7787_TEOFF      (0x34)  // Tearing Effect Line Off
#define ST7787_TEON       (0x35)  // Tearinf Effect On
#define ST7787_MADCTL     (0x36)  // Memory Data Acess Control
#define ST7787_VSCSAD     (0x37)  // Scroll Start Address of RAM
#define ST7787_IDMOFF     (0x38)  // IDLE Mode Off
#define ST7787_IDMON      (0x39)  // IDLE Mode On
#define ST7787_COLMOD     (0x3A)  // Interface Pixel Format
#define ST7787_OTP_PROC   (0x3F)  // OTP Process
#define ST7787_RDID1      (0xDA)  // Read ID1
#define ST7787_RDID2      (0xDB)  // Read ID2
#define ST7787_RDID3      (0xDC)  // Read ID3
// Panel Function Command List 1
#define ST7787_RGBCTR     (0xB0)  // Set Display I/F Mode
#define ST7787_FRMCTR1    (0xB1)  // In Normal Mode Full Colors
#define ST7787_FRMCTR2    (0xB2)  // In IDLE Mode 8-Colors
#define ST7787_FRMCTR3    (0xB3)  // In Partial Mode + Full Colors
#define ST7787_INVCTR     (0xB4)  // Display Inversion Control
#define ST7787_RGB_PRCTR  (0xB5)  // RGB I/F Blanking Porch Setting
#define ST7787_DISSET5    (0xB6)  // Display Function Setting
#define ST7787_VSYNCOUT   (0xBC)  // External VSYNC Disable
#define ST7787_VSYNCOIN   (0xBD)  // External VSYNC Enable
// Panel Function Command List 2
#define ST7787_PWCTR1     (0xC0)  // Power Control Setting
#define ST7787_PWCTR2     (0xC1)  // Power Control Setting
#define ST7787_PWCTR3     (0xC2)  // In Normal Mode Full Color
#define ST7787_PWCTR4     (0xC3)  // In IDLE Mode (8-Colors)
#define ST7787_PWCTR5     (0xC4)  // In Partial Mode + Full Colors
#define ST7787_VMCTR1     (0xC5)  // VCOM Control 1
#define ST7787_VMCTR2     (0xC6)  // VCOM Control 1
// Panel Function Command List 3
#define ST7787_WRID1      (0xD0)  // Reserved
#define ST7787_WRID2      (0xD1)  // LCM Version Code
#define ST7787_WRID3      (0xD2)  // Customer Project Code
#define ST7787_OTP_LOAD   (0xDE)  // OTP Process
#define ST7787_OTP_PROG   (0xDF)  // OTP Programming Command
// Panel Function Command List 4
#define ST7787_GAMCTRP1   (0xE0)  // Set Gamma Correction Pos Polarity
#define ST7787_GAMCTRN1   (0xE1)  // Set Gamma Correction Neg Polarity
#define ST7787_VCOM_MMODE (0xFB)  // VCOM Multi Mode
//-----------------------------------------------------------------------

// 30 символов в строке, всего 40 строк текста (шрифт 8х8)
char CON7787 [ST7787_COL*ST7787_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LINE_01 CON7787
#define LINE_02 CON7787+ST7787_COL
#define LINE_03 CON7787+(ST7787_COL*2)
#define LINE_04 CON7787+(ST7787_COL*3)

typedef struct
{ 
  unsigned int  bfType; // bfType определяет тип файла. Здесь он должен быть BM.
  unsigned long bfSize;   // размер самого файла в байтах.
  unsigned int  bfReserved1; // bfReserved1 и bfReserved2 зарезервированы и должны быть нулями. 
  unsigned int  bfReserved2; 
  unsigned long bfOffBits; // где начинается сам битовый массив относительно начала файла
} BITMAPFILEHEADER;

typedef struct
{
  unsigned long biSize; // это размер самой структуры (Header Size)
  signed long   biWidth; // Ширина картинки в пикселах
  signed long   biHeight; // Высота картинки в пикселах
  unsigned int  biPlanes; // количество плоскостей. Пока оно всегда устанавливается в 1. 
  unsigned int  biBitCount; // Количество бит на один пиксель
  unsigned long biCompression;  // обозначает тип сжатия (0..6)
  unsigned long biSizeImage; // размер картинки в байтах
  signed long   biXPelsPerMeter; // горизонтальное разрешение (в пикселях на метр)
  signed long   biYPelsPerMeter; // вертикальное разрешение (в пикселях на метр)
  unsigned long biClrUsed;      // количество используемых цветов из таблицы
  unsigned long biClrImportant; // количество важных цветов
} BITMAPINFOHEADER;

typedef struct
{
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bih;
  unsigned char data_BGR[];
} BMP;

typedef struct
{
  unsigned char rgbBlue; 
  unsigned char rgbGreen; 
  unsigned char rgbRed; 
  unsigned char rgbReserved; 
} RGBQUAD;

typedef struct
{
  unsigned char rgbBlue; 
  unsigned char rgbGreen; 
  unsigned char rgbRed; 
} RGBTRIPLE;

#define BI_RGB            0
#define BI_RLE8           1
#define BI_RLE4           2
#define BI_BITFIELDS      3
#define BI_JPEG           4
#define BI_PNG            5
#define BI_ALPHABITFIELDS 6


//-------------------------------------------------------
// R4 R3 R2 R1 | R0 G5 G4 G3 | G2 G1 G0 B4 | B3 B2 B1 B0
// 5xR:6xG:5xB

#define PURPLE_COLOR    0xF81F
#define LIGHTBLUE_COLOR 0x07FF
#define YELLOW_COLOR    0xFFE0
#define WHITE_COLOR     0xFFFF
#define BLUE_COLOR      0x001F
#define GREEN_COLOR     0x07E0
#define RED_COLOR       0xF800
#define BLACK_COLOR     0x0000

//unsigned char Palette [1024];

// extern __flash BMP bmp_file; 

