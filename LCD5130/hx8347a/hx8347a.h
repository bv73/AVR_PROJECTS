// HX8347A Header
// By (R)soft 22.5.2013
// Connectiong LCD: PORTC - Control,
// PORTB - HI Byte, PORTA - LOW Byte

#define PIN_NCS     PORTC_Bit0 // Chip Select (Active is LOW LEVEL)
#define DDR_NCS     DDRC_Bit0
#define PIN_DNC     PORTC_Bit1 // DATA Command Bit: 0 - Command, 1 - Data
#define DDR_DNC     DDRC_Bit1
#define PIN_NWR     PORTC_Bit2 // Write Data (Active is LOW LEVEL)
#define DDR_NWR     DDRC_Bit2
#define PIN_NRD     PORTC_Bit3 // Read Data (Active is LOW LEVEL)
#define DDR_NRD     DDRC_Bit3
#define PIN_NRESET  PORTC_Bit4  // /Reset Chip (Active is LOW LEVEL)
#define DDR_NRESET  DDRC_Bit4

#define PD_LO       PORTA // Порт данных LOW byte
#define PIN_LO      PINA
#define	DDRD_LO     DDRA  // Для вывода в порт данных, байт нужно предварительно инвертировать

#define PD_HI       PORTB // Порт данных HIGH byte
#define PIN_HI      PINB
#define DDRD_HI     DDRB

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define NCS_1  	  DDR_NCS=0
#define NCS_0  	  DDR_NCS=1
#define DNC_1     DDR_DNC=0
#define DNC_0     DDR_DNC=1
#define NWR_1     DDR_NWR=0
#define NWR_0     DDR_NWR=1
#define NRD_1  	  DDR_NRD=0
#define NRD_0  	  DDR_NRD=1
#define NRESET_1  DDR_NRESET=0
#define NRESET_0  DDR_NRESET=1

// Размеры экрана 320x240x18
#define HX8347A_PIXELX  (240)
#define HX8347A_PIXELY  (320)
#define HX8347A_COL     (HX8347A_PIXELX/8) // Количество символов в строке 30
#define HX8347A_ROW     (HX8347A_PIXELY/8) // Количество строк 40

// Задержка операции записи/чтения в микросекундах
#define HX8347A_DELAY   (1)

// Команды ЖКИ
//----------------------------------------------------------------------
#define HX8347A_DISPL_MODE_CTRL (0x01)
#define HX8347A_COLSTARTHIGH    (0x02)
#define HX8347A_COLSTARTLOW     (0x03)
#define HX8347A_COLENDHIGH      (0x04)
#define HX8347A_COLENDLOW       (0x05)
#define HX8347A_ROWSTARTHIGH    (0x06)
#define HX8347A_ROWSTARTLOW     (0x07)
#define HX8347A_ROWENDHIGH      (0x08)
#define HX8347A_ROWENDLOW       (0x09)
#define HX8347A_VCOM_CTRL1      (0x43)
#define HX8347A_VCOM_CTRL2      (0x44)
#define HX8347A_VCOM_CTRL3      (0x45)
#define HX8347A_GAMMA_CTRL1     (0x46)
#define HX8347A_GAMMA_CTRL2     (0x47)
#define HX8347A_GAMMA_CTRL3     (0x48)
#define HX8347A_GAMMA_CTRL4     (0x49)
#define HX8347A_GAMMA_CTRL5     (0x4A)
#define HX8347A_GAMMA_CTRL6     (0x4B)
#define HX8347A_GAMMA_CTRL7     (0x4C)
#define HX8347A_GAMMA_CTRL8     (0x4D)
#define HX8347A_GAMMA_CTRL9     (0x4E)
#define HX8347A_GAMMA_CTRL10    (0x4F)
#define HX8347A_GAMMA_CTRL11    (0x50)
#define HX8347A_GAMMA_CTRL12    (0x51)
#define HX8347A_DISPLAY_CTRL8   (0x90)
#define HX8347A_DISPLAY_CTRL11  (0x91)
#define HX8347A_OSC_CTRL1       (0x19)
#define HX8347A_OSC_CTRL3       (0x93)
//-----------------------------------------------------------------------

// 30 символов в строке, всего 40 строк текста (шрифт 8х8)
//char CON8347 [HX8347A_COL*HX8347A_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LINE_01 CON8347
#define LINE_02 CON8347+HX8347A_COL
#define LINE_03 CON8347+(HX8347A_COL*2)
#define LINE_04 CON8347+(HX8347A_COL*3)

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


