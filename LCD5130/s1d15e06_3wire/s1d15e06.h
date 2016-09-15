// 3-Wire SPI

// За какие лапки будем дергать LCD

#define PIN_SCL   PORTA_Bit5 // Serial Clock
#define DDR_SCL   DDRA_Bit5
#define PIN_SI    PORTA_Bit6 // Serial Data Input
#define DDR_SI    DDRA_Bit6
#define PIN_XRES  PORTA_Bit7 // /Reset LCD Driver
#define DDR_XRES  DDRA_Bit7
#define PIN_XCS   PORTA_Bit4
#define DDR_XCS   DDRA_Bit4

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define SCL_1   DDR_SCL=0
#define SCL_0   DDR_SCL=1
#define SI_1    DDR_SI=0
#define SI_0    DDR_SI=1
#define XRES_1  DDR_XRES=0
#define XRES_0  DDR_XRES=1
#define XCS_1   DDR_XCS=0
#define XCS_0   DDR_XCS=1

// Размеры экрана
#define S1D15E06_PIXELX  (96)
#define S1D15E06_PIXELY  (65)
#define S1D15E06_ROW     (8) // Количество строк
#define S1D15E06_COL     (16) // Количество символов в строке

char CON15E06[(S1D15E06_COL*S1D15E06_ROW)+1];

// Command Table
#define S1D15E06_DISPLAY_OFF          (0xAE)
#define S1D15E06_DISPLAY_ON           (0xAF)
#define S1D15E06_DISPLAY_OFF_MODE0    (0xBE)
#define S1D15E06_DISPLAY_OFF_MODE1    (0xBF)
#define S1D15E06_DISPLAY_NORMAL       (0xA6)
#define S1D15E06_DISPLAY_REVERSE      (0xA7)
#define S1D15E06_ALL_PIXEL_OFF        (0xA4)
#define S1D15E06_ALL_PILEL_ON         (0xA5)
#define S1D15E06_COMM_SLCT_NORMAL     (0xC4)
#define S1D15E06_COMM_SLCT_REVERSE    (0xC5)
#define S1D15E06_DISPLAY_START_LINE   (0x8A)  // After this command will send 1 or 2 bytes
#define S1D15E06_PAGE_ADDRESS_SET     (0xB1)  // After this command will send 1 byte
#define S1D15E06_COLUMN_ADDRESS_SET   (0x13)  // After this command will send 1 byte
#define S1D15E06_DISPLAY_DATA_WRITE   (0x1D)  // After this command will send n data bytes
#define S1D15E06_DISPLAY_DATA_READ    (0x1C)
#define S1D15E06_DIRECTION_SLCT_COL   (0x84)
#define S1D15E06_DIRECTION_SLCT_PAGE  (0x85)
#define S1D15E06_COLUMN_ADDR_NORMAL   (0xA0)
#define S1D15E06_COLUMN_ADDR_REVERSE  (0xA1)
#define S1D15E06_NLINLE_INVERSION_SET (0x36) // After this command will send 1 byte
#define S1D15E06_NLINLE_OFF           (0xE4)
#define S1D15E06_NLINLE_ON            (0xE5)
#define S1D15E06_DISPLAY_MODE         (0x66) // After this command will send 1 byte
#define S1D15E06_GRAYSCALE_PTRN_SET   (0x39) // After this command will send 1 byte
#define S1D15E06_AREA_SCROLL_SET      (0x10) // After this command will send 4 bytes
#define S1D15E06_DUTY_SET             (0x6D) // After this command will send 2 bytes
#define S1D15E06_PARTIAL_DISPLAY_OFF  (0x96)
#define S1D15E06_PARTIAL_DISPLAY_ON   (0x97)
#define S1D15E06_PARTIAL_DISPLAY_SET  (0x32) // After this command will send 2 bytes
#define S1D15E06_READ_MODIFY_WRITE    (0xE0)
#define S1D15E06_END                  (0xEE)
#define S1D15E06_OSCILLATOR_OFF       (0xAA)
#define S1D15E06_OSCILLATOR_ON        (0xAB)
#define S1D15E06_OSCILLATOR_FREQ_SLCT (0x5F) // After this command will send 1 byte
#define S1D15E06_POWER_CONTROL_SET    (0x25) // After this command will send 1 byte
#define S1D15E06_STEPUP_FREQ_SLCT     (0x41) // After this command will send 1 byte
#define S1D15E06_VOLTAGE_SELECT       (0x2B) // After this command will send 1 byte
#define S1D15E06_ELECTRONIC_VOLUME    (0x81) // After this command will send 1 byte contrast value
#define S1D15E06_DISCHARGE_OFF        (0xEA)
#define S1D15E06_DISCHARGE_ON         (0xEB)
#define S1D15E06_POWER_SAVING_OFF     (0xA8)
#define S1D15E06_POWER_SAVING_ON      (0xA9)
#define S1D15E06_TEMP_GRADIENT_SET    (0x4E)  // After this command will send 1 byte
#define S1D15E06_STATUS_READ          (0x8E)  // After this command will 1 byte data is read
#define S1D15E06_SOFTWARE_RESET       (0xE2)
#define S1D15E06_MLS_DRIVE_SELECT     (0x9C)  // After this command will send 1 byte
#define S1D15E06_NOP                  (0xE3)


























