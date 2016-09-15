// HD66766 Header
// By (R)soft 11.11.2011 - 26.11.2011

// За какие лапки будем дергать LCD
// Порт C - управление, порт A & B - данные
// Порт D - отладка

#define PIN_RS    PORTC_Bit0 // Register Select Bit: 0 - Index/Status, 1 - Control
#define DDR_RS    DDRC_Bit0
#define PIN_XCS   PORTC_Bit1 // Chip Select (Active is LOW LEVEL)
#define DDR_XCS   DDRC_Bit1
#define PIN_E   PORTC_Bit2 // Enable Data (Active is HIGH LEVEL)
#define DDR_E   DDRC_Bit2
#define PIN_RW   PORTC_Bit3 // Read-Write Data (0:Write, 1:Read)
#define DDR_RW   DDRC_Bit3
#define PIN_XRES  PORTC_Bit4  // /Reset Chip (Active is LOW LEVEL)
#define DDR_XRES  DDRC_Bit4


#define PD_LO   PORTA // Порт данных LOW byte
#define PIN_LO  PINA
#define	DDRD_LO DDRA  // Для вывода в порт данных, байт нужно предварительно инвертировать

#define PD_HI   PORTB // Порт данных HIGH byte
#define PIN_HI  PINB
#define DDRD_HI DDRB

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define RS_1   	DDR_RS=0
#define RS_0   	DDR_RS=1
#define XCS_1  	DDR_XCS=0
#define XCS_0  	DDR_XCS=1
#define E_1  	DDR_E=0
#define E_0  	DDR_E=1
#define RW_1    DDR_RW=0
#define RW_0    DDR_RW=1
#define XRES_1  DDR_XRES=0
#define XRES_0  DDR_XRES=1

// Размеры экрана
#define HD66766_PIXELX  (160) // У контроллера 176
#define HD66766_PIXELY  (128) // У контроллера 132
#define HD66766_ROW     (HD66766_PIXELX/8) // Количество строк 20
#define HD66766_COL     (HD66766_PIXELY/8) // Количество символов в строке 16

// Команды ЖКИ

#define HD66766_R00 (0x00)  // Start Oscillation
#define HD66766_R01 (0x01)  // Driver Output Control
#define HD66766_R02 (0x02)  // LCD-Driving Waveform Control
#define HD66766_R03 (0x03)  // Power Control 1
#define HD66766_R04 (0x04)  // Contrast Control
#define HD66766_R05 (0x05)  // Entry Mode ???
#define HD66766_R06 (0x06)  // Compare Resister
#define HD66766_R07 (0x07)  // Display Control
#define HD66766_R0B (0x0B)  // Frame Cycle Control
#define HD66766_R0C (0x0C)  // Power Control 2
#define HD66766_R11 (0x11)  // Vertical Scroll Control
#define HD66766_R14 (0x14)  // 1st Screen Driving Position
#define HD66766_R15 (0x15)  // 2nd Screen Driving Position
#define HD66766_R16 (0x16)  // Horizontal RAM Address Position
#define HD66766_R17 (0x17)  // Vertical RAM Address Position
#define HD66766_R20 (0x20)  // RAM Write Data Mask
#define HD66766_R21 (0x21)  // RAM Address Set
#define HD66766_R22 (0x22)  // Write Data to GRAM / Read Data from GRAM
#define HD66766_R30 (0x30)  // Grayscale Palette Control (1)
#define HD66766_R31 (0x31)  // Grayscale Palette Control (2)
#define HD66766_R32 (0x32)  // Grayscale Palette Control (3)
#define HD66766_R33 (0x33)  // Grayscale Palette Control (4)
#define HD66766_R34 (0x34)  // Grayscale Palette Control (5)
#define HD66766_R35 (0x35)  // Grayscale Palette Control (6)
#define HD66766_R36 (0x36)  // Grayscale Palette Control (7)
#define HD66766_R37 (0x37)  // Grayscale Palette Control (8)
#define HD66766_R38 (0x38)  // Grayscale Palette Control (9)
#define HD66766_R39 (0x39)  // Grayscale Palette Control (10)
#define HD66766_R3A (0x3A)  // Grayscale Palette Control (11)
#define HD66766_R3B (0x3B)  // Grayscale Palette Control (12)
#define HD66766_R3C (0x3C)  // Grayscale Palette Control (13)
#define HD66766_R3D (0x3D)  // Grayscale Palette Control (14)
#define HD66766_R3E (0x3E)  // Grayscale Palette Control (15)
#define HD66766_R3F (0x3F)  // Grayscale Palette Control (16)


// 16 символов в строке, всего 20 строк текста (шрифт 8х8)

char CON66766 [HD66766_COL*HD66766_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Расположение адресов и пикселов на экране
// +--------------+
// |x0,y159       |x127,y159 (line160)
// |              |
// |              |
// |              |
// |              |
// |              |
// |              |
// |x0,y0 -->     |x127,y0 (Line1)
// +--------------+
//      [chip]


// Определения начала каждой строки в консоли
#define LINE_01 CON66766
#define LINE_02 CON66766+HD66766_COL
#define LINE_03 CON66766+(HD66766_COL*2)
#define LINE_04 CON66766+(HD66766_COL*3)
#define LINE_05 CON66766+(HD66766_COL*4)
#define LINE_06 CON66766+(HD66766_COL*5)
#define LINE_07 CON66766+(HD66766_COL*6)
#define LINE_08 CON66766+(HD66766_COL*7)
#define LINE_09 CON66766+(HD66766_COL*8)
#define LINE_10 CON66766+(HD66766_COL*9)
#define LINE_11 CON66766+(HD66766_COL*10)
#define LINE_12 CON66766+(HD66766_COL*11)
#define LINE_13 CON66766+(HD66766_COL*12)
#define LINE_14 CON66766+(HD66766_COL*13)
#define LINE_15 CON66766+(HD66766_COL*14)
#define LINE_16 CON66766+(HD66766_COL*15)
#define LINE_17 CON66766+(HD66766_COL*16)
#define LINE_18 CON66766+(HD66766_COL*17)
#define LINE_19 CON66766+(HD66766_COL*18)
#define LINE_20 CON66766+(HD66766_COL*19)

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

extern __flash BMP bmp_file; // CODE 00000BB8 - 0000ECED (0xe136 bytes), align: 2

