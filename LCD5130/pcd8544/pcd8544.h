#define delay_us(V) __delay_cycles(V*8L)


// За какие лапки будем дергать LCD

#define PIN_DC      PORTA_Bit0 // Data/Command Bit
#define DDR_DC      DDRA_Bit0
#define PIN_SCK     PORTA_Bit1 // Clock Data
#define DDR_SCK     DDRA_Bit1
#define PIN_DOUT    PORTA_Bit2 // Serial Data
#define DDR_DOUT    DDRA_Bit2
#define PIN_RESET   PORTA_Bit3 // Reset LCD этот бит нужно подключать к контроллеру
#define DDR_RESET   DDRA_Bit3

// Необязательные биты подключения

//#define PIN_CS      PORTA_Bit3 // Chip Select этот бит может быть всегда 0
//#define DDR_CS      DDRA_Bit3
//#define PIN_LIGHT   PORTA_Bit5 // Back Light LCD
//#define DDR_LIGHT   DDRA_Bit5

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define DC_1  DDR_DC=0
#define DC_0  DDR_DC=1
#define SCK_1 DDR_SCK=0
#define SCK_0 DDR_SCK=1
#define DOUT_1  DDR_DOUT=0
#define DOUT_0  DDR_DOUT=1
#define CS_1  DDR_CS=0
#define CS_0  DDR_CS=1
#define RESET_1 DDR_RESET=0
#define RESET_0 DDR_RESET=1
#define LIGHT_1 DDR_LIGHT=0
#define LIGHT_0 DDR_LIGHT=1

// Размеры экрана
#define PCD8544_PIXELX (84)
#define PCD8544_PIXELY (48)

// Команды ЖКИ (Normal Functionset)
#define PCD8544_FUNCTIONSET    (0x20)
#define PCD8544_SETYADDR       (0x40)
#define PCD8544_SETXADDR       (0x80)
#define PCD8544_DISPLAYBLANK   (0x08)
#define PCD8544_DISPLAYFLUSH   (0x09)
#define PCD8544_DISPLAYNORMAL  (0x0C)
#define PCD8544_DISPLAYINVERT  (0x0D)

// Команды ЖКИ (Extended Functionset)
#define PCD8544_FUNCTIONSETEXT (0x21)
#define PCD8544_SET_BIAS       (0x10)
#define PCD8544_SET_VOP        (0x80)
#define PCD8544_TEMPCOEF       (0x06)  // TF=2

#define PCD8544_BIAS_100       (0x00) // 1:100
#define PCD8544_BIAS_80        (0x01) // 1:80
#define PCD8544_BIAS_65        (0x02)
#define PCD8544_BIAS_48        (0x03)
#define PCD8544_BIAS_40        (0x04)
#define PCD8544_BIAS_24        (0x05)
#define PCD8544_BIAS_18        (0x06)
#define PCD8544_BIAS_10        (0x07)

// Основные глобальные (volatile) переменные LCD
volatile unsigned char lcd_x = 0; // Позиция X, которая указывается при выводе текста
volatile unsigned char lcd_y = 0;
volatile unsigned char lcd_contrast = 0; // Значение контрастности
volatile unsigned char lcd_inverse = 0; // Инверсия вывода (0 - выключена)

// 14 символов в строке, всего 6 строк текста

char CON [14*6+1]; // CONsole buffer для вывода функцией sprintf (84 символа)

#define LINE1 CON     // Определения начала каждой строки в консоли
#define LINE2 CON+14
#define LINE3 CON+28
#define LINE4 CON+42
#define LINE5 CON+56
#define LINE6 CON+70

// function prototypes

void pcd8544_w (unsigned char ch); // Запись байта в ЖКИ
void pcd8544_cmd(unsigned char cmd); // Отправка команды в ЖКИ
void pcd8544_data(unsigned char dat); // Отправка данных в ЖКИ
void pcd8544_set_addr(unsigned char x, unsigned char y); // Установка адреса в ЖКИ
void pcd8544_gotoxy(unsigned char x, unsigned char y); // Установка позиции вывода по координатам x,y
void pcd8544_putchar(unsigned char ch); // Вывод символа на ЖКИ
void pcd8544_cls(void); // Очистка экрана - заполнение ОЗУ нулями, установка позиции вывода в ноль
void pcd8544_puts (unsigned char *s); // Вывод строки (строка в ОЗУ)
void pcd8544_set_contrast(unsigned char c); // Задать значение контраста
void pcd8544_set_inverse(unsigned char inv); // Задать значение инверсии
void pcd8544_init (void); // Инициализания контроллера AVR
void update_console(void); // Выводит из буфера консоли CON на ЖКИ
void clear_console (unsigned char byte); // Заполнение буфера консоли CON требуемым байтом

// void LcdPixel (unsigned char x, unsigned char y); // Вывод точки
// void LcdPixelErase (unsigned char x, unsigned char y); // Стирание точки
// void erase_column (unsigned char x, unsigned char y); // Стирание колонки