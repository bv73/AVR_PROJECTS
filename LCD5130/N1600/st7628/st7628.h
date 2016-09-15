// ST7628 Driver For Nokia 1600 CSTN LCD
// AVR Code by (R)soft 12.12.2013
// ST7628 совместим по командам с LDS184!

// За какие лапки будем дергать LCD

#define PIN_SCL   PORTA_Bit6 // Clock Data
#define DDR_SCL   DDRA_Bit6
#define PIN_SI    PORTA_Bit7 // Serial Data Input
#define DDR_SI    DDRA_Bit7
#define PIN_XRST  PORTA_Bit5
#define DDR_XRST  DDRA_Bit5

#define SCL_1  PIN_SCL=1
#define SCL_0  PIN_SCL=0
#define SI_1   PIN_SI=1
#define SI_0   PIN_SI=0
#define XRST_1 PIN_XRST=1
#define XRST_0 PIN_XRST=0

// Размеры экрана
#define ST7628_PIXELX  (96)
#define ST7628_PIXELY  (68)
#define ST7628_ROW     (8) // Количество строк
#define ST7628_COL     (16) // Количество символов в строке


// Команды ЖКИ

#define ST7628_NOP            (0x00)
#define ST7628_SW_RESET       (0x01)
#define ST7628_RDD_ID         (0x04)
#define ST7628_RDD_ST         (0x09)
#define ST7628_RDD_PM         (0x0A)
#define ST7628_RDD_MADCTR     (0x0B)
#define ST7628_RDD_COLMOD     (0x0C)
#define ST7628_RDD_IM         (0x0D)
#define ST7628_RDD_SM         (0x0E)
#define ST7628_RDD_SDR        (0x0F)
#define ST7628_SLEEP_IN       (0x10)
#define ST7628_SLEEP_OUT      (0x11)
#define ST7628_PTL_ON         (0x12)
#define ST7628_NOR_ON         (0x13)
#define ST7628_INV_OFF        (0x20)
#define ST7628_INV_ON         (0x21)
#define ST7628_AP_OFF         (0x22)
#define ST7628_AP_ON          (0x23)
#define ST7628_WR_CNTR        (0x25)
#define ST7628_DISPLAY_OFF    (0x28)
#define ST7628_DISPLAY_ON     (0x29)
#define ST7628_CASET          (0x2A) // Col address set
#define ST7628_RASET          (0x2B) // Row address set
#define ST7628_RAM_WR         (0x2C)
#define ST7628_RGB_SET        (0x2D)
#define ST7628_RAM_RD         (0x2E)
#define ST7628_PT_LAR         (0x30)
#define ST7628_SCR_LAR        (0x33)
#define ST7628_TE_OFF         (0x34)
#define ST7628_TE_ON          (0x35)
#define ST7628_MADCTR         (0x36)
#define ST7628_VSCSAD         (0x37)
#define ST7628_IDM_OFF        (0x38)
#define ST7628_IDM_ON         (0x39)
#define ST7628_COLMOD         (0x3A)
#define ST7628_RDID1          (0xDA)
#define ST7628_RDID2          (0xDB)
#define ST7628_RDID3          (0xDC)

#define ST7628_DUTY_SET       (0xB0)
#define ST7628_FIRST_COM      (0xB1)
#define ST7628_OSC_DIV        (0xB3)
#define ST7628_PTL_MOD        (0xB4)
#define ST7628_NL_INV_SET     (0xB5)
#define ST7628_COM_SCAN_DIR   (0xB7)
#define ST7628_RMW_IN         (0xB8)
#define ST7628_RMW_OUT        (0xB9)
#define ST7628_VOP_SET        (0xC0)
#define ST7628_VOP_OFSET_INC  (0xC1)
#define ST7628_VOP_OFSET_DEC  (0xC2)
#define ST7628_VOP_BIAS_SEL   (0xC3)
#define ST7628_BST_BMP_X_SEL  (0xC4)
#define ST7628_BST_EFF_SEL    (0xC5)
#define ST7628_VOP_OFFSET     (0xC7)
#define ST7628_VG_SORC_SEL    (0xCB)
#define ST7628_ANA_SET        (0xD0)
#define ST7628_AUTO_LOAD_SET  (0xD7)
#define ST7628_RD_TST_STATUS  (0xDE)
#define ST7628_EPTC_IN        (0xE0)
#define ST7628_EPTC_OUT       (0xE1)
#define ST7628_FRM_SEL        (0xF0)
#define ST7628_FRM8_SEL       (0xF1)
#define ST7628_TMP_RNG        (0xF2)
#define ST7628_TMP_HYS        (0xF3)
#define ST7628_TEMP_SEL       (0xF4)
#define ST7628_THYS           (0xF7)
#define ST7628_FRAME_SET      (0xF9)


// Основные глобальные (volatile) переменные LCD
//volatile unsigned char lcd8814_x = 0; // Позиция X, которая указывается при выводе текста
//volatile unsigned char lcd8814_y = 0;
//volatile unsigned char lcd8814_contrast = 0; // Значение контрастности
//volatile unsigned char lcd8814_inverse = 0; // Инверсия вывода (0 - выключена)

// 16 символов в строке, всего 8 строк текста

char CON7628 [ST7628_COL*ST7628_ROW+1]; // CONsole buffer для вывода функцией sprintf (128 символов)

// Определения начала каждой строки в консоли
#define LINE1 CON7628
#define LINE2 CON7628+ST7628_COL
#define LINE3 CON7628+(ST7628_COL*2)
#define LINE4 CON7628+(ST7628_COL*3)
#define LINE5 CON7628+(ST7628_COL*4)
#define LINE6 CON7628+(ST7628_COL*5)
#define LINE7 CON7628+(ST7628_COL*6)
#define LINE8 CON7628+(ST7628_COL*7)

