// --- LCD подключен без использования Z-состояния ноги ввода
// --- Т.е. алгоритм дрыгания ногами изменен. Раньше в порт
// --- LCD выводились нули и ногой дрыгали изменением направления
// --- порта (на вывод - 0, на ввод - 1).
// --- Теперь направление порта всегда на вывод и управление
// --- LCD осуществляется установкой битов порта.
// --- Тестировано для LCD Nokia 1200 c напряжением питания 3,3 В


// За какие лапки будем дергать LCD

#define PIN_CSN     PORTB_Bit4 // Chip Select Bit
#define DDR_CSN     DDRB_Bit4
#define PIN_SCL     PORTB_Bit6 // Serial Clock
#define DDR_SCL     DDRB_Bit6
#define PIN_SI      PORTB_Bit7 // Serial Data Input
#define DDR_SI      DDRB_Bit7
#define PIN_RESN    PORTB_Bit5 // /Reset LCD Driver
#define DDR_RESN    DDRB_Bit5

#define CSN_1  PIN_CSN=1
#define CSN_0  PIN_CSN=0
#define SCL_1  PIN_SCL=1
#define SCL_0  PIN_SCL=0
#define SI_1   PIN_SI=1
#define SI_0   PIN_SI=0
#define RESN_1 PIN_RESN=1
#define RESN_0 PIN_RESN=0

// Размеры экрана (контроллер обеспечивает 132x65)
#define SED15B1_PIXELX  (96) // У контроллера этот параметр 132
#define SED15B1_PIXELY  (65)
#define SED15B1_ROW     (8) // Количество строк
#define SED15B1_COL     (16) // Количество символов в строке

// Сдвиг адреса X из-за неподключенных сегментов в начале
// т.к. контроллер имеет разрядность по Х=132 а сам ЖКИ всего 96 пикселей, то
// экран подключен по центру сегментов контроллера, слева три символа по 6 байт и справа
// три символа по 6 байт не подключены, поэтому начальный сдвиг нужно учесть
// Разбито на две тетрады т.к. я туплю и мне лень, потом доделаю
// 18 in HEX 0x12   36 in HEX 0x24
#define SED15B1_ADD_XHI   0x00
#define SED15B1_ADD_XLO   0x00

// Команды ЖКИ
#define SED15B1_DISPLAY_ON          (0xAF)
#define SED15B1_DISPLAY_OFF         (0xAE) // Display OFF для снижения энергопотребления
#define SED15B1_DISPLAY_NORMAL      (0xA6)
#define SED15B1_DISPLAY_REVERSE     (0xA7)
#define SED15B1_SEGMENT_DIR_NORMAL  (0xA0)
#define SED15B1_SEGMENT_DIR_REVERSE (0xA1)
#define SED15B1_COMMON_DIR_NORMAL   (0xC0)
#define SED15B1_COMMON_DIR_REVERSE  (0xC8)
#define SED15B1_SET_BIAS_19         (0xA2) // Set BIAS 1/9
#define SED15B1_SET_BIAS_17         (0xA3) // Set BIAS 1/7
#define SED15B1_RESISTOR_RATIO      (0x20) // 0...6
#define SED15B1_ELECTRONIC_VOLUME   (0x80) // | 0...1F (1F - low, 0 - high)
#define SED15B1_POWER_CTRL_SET      (0x28) // | 0x04 booster | 0x02 voltage regulator | 0x01 booster ON/OFF
#define SED15B1_ALL_POINTS_ON       (0xA5) // Включить все пиксели
#define SED15B1_ALL_POINTS_OFF      (0xA4) // Нормальное отображение

#define SED15B1_START_LINE_ADDR    (0x40) // С какого адреса выводить 0...3F

// 0 <= Y <= 8
#define SED15B1_SETYADDR           (0xB0) // Установка адреса строки - последние 4 бита 0...8

// 0 <= X <= 131
#define SED15B1_SETXADDR_LOWER     (0x00) // Set A3...A0 (Low Tetrade)
#define SED15B1_SETXADDR_UPPER     (0x10) // Set A7...A4 (High Tetrade)

// Основные глобальные (volatile) переменные LCD
//volatile unsigned char sed_x = 0; // Позиция X, которая указывается при выводе текста
//volatile unsigned char sed_y = 0;
//volatile unsigned char sed_contrast = 0; // Значение контрастности
//volatile unsigned char sed_inverse = 0; // Инверсия вывода (0 - выключена)

// 16 символов в строке, всего 8 строк текста

char CON15B1 [(SED15B1_COL*SED15B1_ROW)+1]; // CONsole buffer для вывода функцией sprintf (128 символов)

// Определения начала каждой строки в консоли
#define LIN1 CON15B1
#define LIN2 CON15B1+SED15B1_COL
#define LIN3 CON15B1+(SED15B1_COL*2)
#define LIN4 CON15B1+(SED15B1_COL*3)
#define LIN5 CON15B1+(SED15B1_COL*4)
#define LIN6 CON15B1+(SED15B1_COL*5)
#define LIN7 CON15B1+(SED15B1_COL*6)
#define LIN8 CON15B1+(SED15B1_COL*7)

// function prototypes

void sed15b1_write (unsigned char ch); // Запись байта в ЖКИ
void sed15b1_cmd(unsigned char cmd); // Отправка команды в ЖКИ
void sed15b1_data(unsigned char dat); // Отправка данных в ЖКИ
//void pcf8812_set_addr(unsigned char x, unsigned char y); // Установка адреса в ЖКИ
//void pcf8812_gotoxy(unsigned char x, unsigned char y); // Установка позиции вывода по координатам x,y
//void pcf8812_putchar(unsigned char ch); // Вывод символа на ЖКИ
//void pcf8812_cls(void); // Очистка экрана - заполнение ОЗУ нулями, установка позиции вывода в ноль
//void pcf8812_puts (unsigned char *s); // Вывод строки (строка в ОЗУ)
//void pcf8812_set_contrast(unsigned char c); // Задать значение контраста
//void pcf8812_set_inverse(unsigned char inv); // Задать значение инверсии
//void pcf8812_set_power(unsigned char power); // Включить/выключить дисплей
//void pcf8812_init (void); // Инициализания контроллера AVR
//void pcf8812_update_console(void); // Выводит из буфера консоли CON на ЖКИ
void sed15b1_clear_console (unsigned char byte); // Заполнение буфера консоли CON требуемым байтом

