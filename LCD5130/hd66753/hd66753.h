// HD66753 Header
// By (R)soft 24.10.2011
// За какие лапки будем дергать LCD
// Порт А - управление, порт C - данные


#define PIN_RS  PORTA_Bit0 // Register Select Bit: 0 - Index/Status 1 - Control
#define DDR_RS  DDRA_Bit0
#define PIN_E  	PORTA_Bit1 // Enable Data
#define DDR_E   DDRA_Bit1
#define PIN_NCS  PORTA_Bit2 // /Chip Select
#define DDR_NCS  DDRA_Bit2
#define PIN_NRST PORTA_Bit3 // /Reset LCD
#define DDR_NRST DDRA_Bit3

#define PDATA	PORTC // Порт данных
#define	DDRDATA	DDRC  // Для вывода в порт данных, байт нужно предварительно инвертировать

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define RS_1   	DDR_RS=0
#define RS_0   	DDR_RS=1
#define E_1  	DDR_E=0
#define E_0  	DDR_E=1
#define NCS_1  	DDR_NCS=0
#define NCS_0  	DDR_NCS=1
#define NRST_1  DDR_NRST=0
#define NRST_0  DDR_NRST=1

// Размеры экрана
#define HD66753_PIXELX  (150) // У контроллера 168
#define HD66753_PIXELY  (100) // У контроллера 132
#define HD66753_ROW     (12) // Количество строк
#define HD66753_COL     (30) // Количество символов в строке

// Команды ЖКИ

#define HD66753_R00 (0x00)  // Start Oscillation
#define HD66753_R01 (0x01)  // Driver Output Control
#define HD66753_R02 (0x02)  // LCD-Driving Waveform Control
#define HD66753_R03 (0x03)  // Power Control
#define HD66753_R04 (0x04)  // Contrast Control
#define HD66753_R05 (0x05)  // Entry Mode ???
#define HD66753_R06 (0x06)  // Rotation
#define HD66753_R07 (0x07)  // Display Control
#define HD66753_R08 (0x08)  // Cursor Control
#define HD66753_R0B (0x0B)  // Horizontal Cursor Position
#define HD66753_R0C (0x0C)  // Vertical Cursor Position
#define HD66753_R0D (0x0D)  // 1st Screen Driving Position
#define HD66753_R0E (0x0E)  // 2st Screen Driving Position
#define HD66753_R10 (0x10)  // RAM Write Data Mask
#define HD66753_R11 (0x11)  // RAM Address Set
#define HD66753_R12 (0x12) // Write Data to CGRAM

#define HD66753_SET_BIAS_19         (0xA2) // Set BIAS 1/9
#define HD66753_SET_BIAS_17         (0xA3) // Set BIAS 1/7

#define HD66753_ALL_POINTS_OFF      (0xA4) // Нормальное отображение
#define HD66753_ALL_POINTS_ON       (0xA5) // Включить все пиксели

#define HD66753_DISPLAY_NORMAL      (0xA6)
#define HD66753_DISPLAY_REVERSE     (0xA7)

#define HD66753_DATA_ORDER_OFF      (0xA8)
#define HD66753_DATA_ORDER_ON       (0xA9)

#define HD66753_RAM_ADDR_MODE_MSB   (0xAA)
#define HD66753_RAM_ADDR_MODE_LSB   (0xAB)

#define HD66753_PARTIAL_POSITION    (0xAC) // And second byte - 0...7

#define HD66753_NLINE_INVERSION     (0xAD)  // And second byte - Value

#define HD66753_DISPLAY_OFF         (0xAE) // Display OFF для снижения энергопотребления
#define HD66753_DISPLAY_ON          (0xAF)

#define HD66753_VERTICAL_MIRROR_OFF (0xC0)
#define HD66753_VERTICAL_MIRROR_ON  (0xC8)

#define HD66753_SET_PARTIAL_DISPLAY (0xD0) // 0...7

#define HD66753_ROW_MIRROR_OFF      (0xE0)
#define HD66753_ROW_MIRROR_ON       (0xE1)

#define HD66753_SOFTWARE_RESET      (0xE2)

#define HD66753_RESISTOR_RATIO      (0x20) // 0...6
#define HD66753_ELECTRONIC_VOLUME   (0x80) // | 0...1F (1F - low, 0 - high)

#define HD66753_POWER_CTRL_SET_OFF  (0x28)
#define HD66753_POWER_CTRL_SET_ON   (0x2F) // ??? 0x2C не пашет

#define HD66753_START_LINE_ADDR    (0x40) // С какого адреса выводить 0...3F

// 0 <= Y <= 8
#define HD66753_SETYADDR           (0xB0) // Установка адреса строки - последние 4 бита 0...8

// 0 <= X <= 131
#define HD66753_SETXADDR_LOWER     (0x00) // Set A3...A0 (Low Tetrade)
#define HD66753_SETXADDR_UPPER     (0x10) // Set A7...A4 (High Tetrade)

// Bias Defines
#define HD66753_BIAS_100  (0x00) // 1:100
#define HD66753_BIAS_80   (0x01) // 1:80
#define HD66753_BIAS_65   (0x02)
#define HD66753_BIAS_48   (0x03)
#define HD66753_BIAS_40   (0x04)
#define HD66753_BIAS_24   (0x05)
#define HD66753_BIAS_18   (0x06)
#define HD66753_BIAS_10   (0x07)


// 16 символов в строке, всего 8 строк текста

char CON66753 [HD66753_COL*HD66753_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LINEN1 CON66753
#define LINEN2 CON66753+HD66753_COL
#define LINEN3 CON66753+(HD66753_COL*2)
#define LINEN4 CON66753+(HD66753_COL*3)
#define LINEN5 CON66753+(HD66753_COL*4)
#define LINEN6 CON66753+(HD66753_COL*5)
#define LINEN7 CON66753+(HD66753_COL*6)
#define LINEN8 CON66753+(HD66753_COL*7)

// function prototypes
/*
void hd66753_write (unsigned char ch); // Запись байта в ЖКИ
void hd66753_cmd(unsigned char cmd); // Отправка команды в ЖКИ
void hd66753_data(unsigned char dat); // Отправка данных в ЖКИ
void hd66753_set_addr(unsigned char x, unsigned char y); // Установка адреса в ЖКИ
void hd66753_gotoxy(unsigned char x, unsigned char y); // Установка позиции вывода по координатам x,y
void hd66753_putchar(unsigned char ch); // Вывод символа на ЖКИ
void hd66753_cls(void); // Очистка экрана - заполнение ОЗУ нулями, установка позиции вывода в ноль
void hd66753_puts (unsigned char *s); // Вывод строки (строка в ОЗУ)
void hd66753_set_contrast(unsigned char c); // Задать значение контраста
void hd66753_set_inverse(unsigned char inv); // Задать значение инверсии
void hd66753_set_power(unsigned char power); // Включить/выключить дисплей
void hd66753_init (void); // Инициализания контроллера AVR
void hd66753_update_console(void); // Выводит из буфера консоли CON на ЖКИ
void hd66753_clear_console (unsigned char byte); // Заполнение буфера консоли CON требуемым байтом

*/