// KS0717 Header by (R)soft 8.5.2013
// PORTD - Control, PORTC - DATA
// Сигнал RESET можно не подключать к контроллеру, проверено,
// заводим его на +VCC


// За какие лапки будем смыкать LCD
#define PIN_RESETN  PORTD_Bit3  // /RESET - сброс ЖКИ, акт. уровень низкий
#define DDR_RESETN  DDRD_Bit3
#define PIN_RS      PORTD_Bit4  // RS - Register Select: LO - Write to Register
                                //                       HI - DATA Display
#define DDR_RS      DDRD_Bit4
#define PIN_ENABLE  PORTD_Bit5  // ENABLE - выбор шины, данные передаются
                                // по нарастанию _/
#define DDR_ENABLE  DDRD_Bit5
#define PDATA	PORTC // Порт данных
#define	DDRDATA	DDRC  // Для вывода в порт данных, байт нужно предварительно инвертировать


// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define RESETN_1  DDR_RESETN=0
#define RESETN_0  DDR_RESETN=1
#define RS_1      DDR_RS=0
#define RS_0      DDR_RS=1
#define ENABLE_1  DDR_ENABLE=0
#define ENABLE_0  DDR_ENABLE=1

// Размеры экрана (контроллер обеспечивает 100x55)
#define KS0717_PIXELX   (96) // У контроллера этот параметр 100
#define KS0717_PIXELY   (54) // У контроллера 55
#define KS0717_ROW      (7) // Количество строк
#define KS0717_COL      (16) // Количество символов в строке

// LCD Instructions
#define KS0717_DISPLAY_OFF    (0xAE)
#define KS0717_DISPLAY_ON     (0xAF)
#define KS0717_SETYADDR       (0x40)
#define KS0717_REF_VOLT_MODE  (0x81)
#define KS0717_SET_PAGE_ADDR  (0xB0)
#define KS0717_SETXADDR_LO    (0x00)
#define KS0717_SETXADDR_HI    (0x10)
#define KS0717_ADC_NORMAL     (0xA0) // Зеркалирование по X
#define KS0717_ADC_REVERSE    (0xA1)
#define KS0717_REVERSE_OFF    (0xA6)
#define KS0717_REVERSE_ON     (0xA7)
#define KS0717_ENTIRE_OFF     (0xA4)
#define KS0717_ENTIRE_ON      (0xA5)
#define KS0717_SELECT_BIAS_18 (0xA2)
#define KS0717_SELECT_BIAS_16 (0xA3)
#define KS0717_SET_MDF_READ   (0xE0)
#define KS0717_RESET_MDF_READ (0xEE)
#define KS0717_RESET_DISPLAY  (0xE2)
#define KS0717_SHL_NORMAL     (0xC0)
#define KS0717_SHL_REVERSE    (0xC8)
#define KS0717_POWER_CONTROL  (0x28)
#define KS0717_RESISTOR_SLCT  (0x20)
#define KS0717_STATIC_IND_OFF (0xAC)
#define KS0717_STATIC_IND_ON  (0xAD)

// 16 символов в строке, всего 6 строк текста
char CON0717 [KS0717_COL*KS0717_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LINEN1 CON0717
#define LINEN2 CON0717+KS0717_COL
#define LINEN3 CON0717+(KS0717_COL*2)
#define LINEN4 CON0717+(KS0717_COL*3)
#define LINEN5 CON0717+(KS0717_COL*4)
#define LINEN6 CON0717+(KS0717_COL*5)
#define LINEN7 CON0717+(KS0717_COL*6)

// function prototypes
/*
void ks0717_write (unsigned char ch); // Запись байта в ЖКИ
void ks0717_cmd(unsigned char cmd); // Отправка команды в ЖКИ
void ks0717_data(unsigned char dat); // Отправка данных в ЖКИ
void ks0717_set_addr(unsigned char x, unsigned char y); // Установка адреса в ЖКИ
void ks0717_gotoxy(unsigned char x, unsigned char y); // Установка позиции вывода по координатам x,y
void ks0717_putchar(unsigned char ch); // Вывод символа на ЖКИ
void ks0717_cls(void); // Очистка экрана - заполнение ОЗУ нулями, установка позиции вывода в ноль
void ks0717_puts (unsigned char *s); // Вывод строки (строка в ОЗУ)
void ks0717_set_contrast(unsigned char c); // Задать значение контраста
void ks0717_set_inverse(unsigned char inv); // Задать значение инверсии
void ks0717_set_power(unsigned char power); // Включить/выключить дисплей
void ks0717_init (void); // Инициализания контроллера AVR
void ks0717_update_console(void); // Выводит из буфера консоли CON на ЖКИ
void ks0717_clear_console (unsigned char byte); // Заполнение буфера консоли CON требуемым байтом
*/
