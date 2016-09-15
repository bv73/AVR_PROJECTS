
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
//#define CS_1  DDR_CS=0
//#define CS_0  DDR_CS=1
#define RESET_1 DDR_RESET=0
#define RESET_0 DDR_RESET=1
//#define LIGHT_1 DDR_LIGHT=0
//#define LIGHT_0 DDR_LIGHT=1

// Размеры экрана
#define PCF8812_PIXELX  (96) // У контроллера этот параметр 102
#define PCF8812_PIXELY  (65)
#define PCF8812_ROW     (8) // Количество строк
#define PCF8812_COL     (16) // Количество символов в строке

// Команды ЖКИ (Normal Functionset H=0)
// 0x28 0x29 YM=1 (Bit3)
// 0x20 0x21 YM=0
#define PCF8812_FUNCTIONSET         (0x20)  // V=0 (Horizontal Addressing), PD=0 (Chip is Active)
#define PCF8812_FUNCTIONSET_YM      (0x28)  // Mirror Y=1
#define PCF8812_DISPLAY_ON          (0x20)
#define PCF8812_DISPLAY_OFF         (0x24) // Display OFF для снижения энергопотребления
#define PCF8812_DISPLAY_BLANK       (0x08)
#define PCF8812_DISPLAY_FLUSH       (0x09)
#define PCF8812_DISPLAY_NORMAL      (0x0C)
#define PCF8812_DISPLAY_INVERT      (0x0D)
#define PCF8812_SET_VOP_RANGE_LOW   (0x10)
#define PCF8812_SET_VOP_RANGE_HIGH  (0x11)
#define PCF8812_SETYADDR            (0x40)  // 0 <= Y <= 8
#define PCF8812_SETXADDR            (0x80)  // 0 <= X <= 101

// Команды ЖКИ (Extended Functionset H=1)
#define PCF8812_FUNCTIONSETEXT      (0x21)
#define PCF8812_VOLTAGE_MUL2X       (0x08)
#define PCF8812_VOLTAGE_MUL3X       (0x09)
#define PCF8812_VOLTAGE_MUL4X       (0x0A)
#define PCF8812_VOLTAGE_MUL5X       (0x0B)
#define PCF8812_SET_BIAS            (0x10)
#define PCF8812_SET_VOP             (0x80)
#define PCF8812_TEMPCOEF            (0x06)  // TF=2

// Bias Defines
#define PCF8812_BIAS_100  (0x00) // 1:100
#define PCF8812_BIAS_80   (0x01) // 1:80
#define PCF8812_BIAS_65   (0x02)
#define PCF8812_BIAS_48   (0x03)
#define PCF8812_BIAS_40   (0x04)
#define PCF8812_BIAS_24   (0x05)
#define PCF8812_BIAS_18   (0x06)
#define PCF8812_BIAS_10   (0x07)

// Основные глобальные (volatile) переменные LCD
volatile unsigned char lcd_x = 0; // Позиция X, которая указывается при выводе текста
volatile unsigned char lcd_y = 0;
volatile unsigned char lcd_contrast = 0; // Значение контрастности
volatile unsigned char lcd_inverse = 0; // Инверсия вывода (0 - выключена)

// 16 символов в строке, всего 8 строк текста

char CON8812 [PCF8812_COL*PCF8812_ROW+1]; // CONsole buffer для вывода функцией sprintf (128 символов)

// Определения начала каждой строки в консоли
#define LINE1 CON8812
#define LINE2 CON8812+PCF8812_COL
#define LINE3 CON8812+(PCF8812_COL*2)
#define LINE4 CON8812+(PCF8812_COL*3)
#define LINE5 CON8812+(PCF8812_COL*4)
#define LINE6 CON8812+(PCF8812_COL*5)
#define LINE7 CON8812+(PCF8812_COL*6)
#define LINE8 CON8812+(PCF8812_COL*7)

// function prototypes

void pcf8812_write (unsigned char ch); // Запись байта в ЖКИ
void pcf8812_cmd(unsigned char cmd); // Отправка команды в ЖКИ
void pcf8812_data(unsigned char dat); // Отправка данных в ЖКИ
void pcf8812_set_addr(unsigned char x, unsigned char y); // Установка адреса в ЖКИ
void pcf8812_gotoxy(unsigned char x, unsigned char y); // Установка позиции вывода по координатам x,y
void pcf8812_putchar(unsigned char ch); // Вывод символа на ЖКИ
void pcf8812_cls(void); // Очистка экрана - заполнение ОЗУ нулями, установка позиции вывода в ноль
void pcf8812_puts (unsigned char *s); // Вывод строки (строка в ОЗУ)
void pcf8812_set_contrast(unsigned char c); // Задать значение контраста
void pcf8812_set_inverse(unsigned char inv); // Задать значение инверсии
void pcf8812_set_power(unsigned char power); // Включить/выключить дисплей
void pcf8812_init (void); // Инициализания контроллера AVR
void pcf8812_update_console(void); // Выводит из буфера консоли CON на ЖКИ
void pcf8812_clear_console (unsigned char byte); // Заполнение буфера консоли CON требуемым байтом

