

// За какие лапки будем дергать LCD

#define PIN_A0    PORTA_Bit0 // A0 (Data=1/Command=0) Bit
#define DDR_A0    DDRA_Bit0
#define PIN_SCLK  PORTA_Bit1 // Serial Clock
#define DDR_SCLK  DDRA_Bit1
#define PIN_SDATA PORTA_Bit2 // Serial Data Input
#define DDR_SDATA DDRA_Bit2
#define PIN_RESET PORTA_Bit3 // /Reset LCD Driver
#define DDR_RESET DDRA_Bit3
#define PIN_CS    PORTA_Bit4
#define DDR_CS    DDRA_Bit4

// бит на ввод -> подтяжка внешними резисторами к VCC
// бит на вывод -> устанавливается 0 (задается при инициализации)
#define A0_1    DDR_A0=0
#define A0_0    DDR_A0=1
#define SCLK_1  DDR_SCLK=0
#define SCLK_0  DDR_SCLK=1
#define SDATA_1 DDR_SDATA=0
#define SDATA_0 DDR_SDATA=1
#define RESET_1 DDR_RESET=0
#define RESET_0 DDR_RESET=1
#define CS_1    DDR_CS=0
#define CS_0    DDR_CS=1

// Размеры экрана (контроллер обеспечивает 132x65)
#define S1D15G14_PIXELX  (96) // У контроллера этот параметр 104
#define S1D15G14_PIXELY  (65) // У контроллера 82
#define S1D15G14_ROW     (8) // Количество строк
#define S1D15G14_COL     (16) // Количество символов в строке

char CON[50];


// Command Table
#define S1D15G14_NOP                  (0x00)
#define S1D15G14_SOFTWARE_RESET       (0x01)
#define S1D15G14_BOOSTER_VOLTAGE_OFF  (0x02)
#define S1D15G14_BOOSTER_VOLTAGE_ON   (0x03)
#define S1D15G14_TEST_MODE1           (0x04)
#define S1D15G14_READ_DISPLAY_STATUS  (0x09)  // After this command will 32-bit data is read
#define S1D15G14_SLEEP_IN             (0x10)
#define S1D15G14_SLEEP_OUT            (0x11)
#define S1D15G14_PARTIAL_DISPLAY_MODE (0x12)
#define S1D15G14_DISPLAY_NORMAL_MODE  (0x13)
#define S1D15G14_INVERSION_OFF        (0x20)
#define S1D15G14_INVERSION_ON         (0x21)
#define S1D15G14_ALL_PIXEL_OFF        (0x22)
#define S1D15G14_ALL_PILEL_ON         (0x23)
#define S1D15G14_WRITE_CONTRAST       (0x25) // After this command will send contrast value
#define S1D15G14_DISPLAY_OFF          (0x28)
#define S1D15G14_DISPLAY_ON           (0x29)
#define S1D15G14_COLUMN_ADDRESS_SET   (0x2A) // After this command will send two bytes
#define S1D15G14_PAGE_ADDRESS_SET     (0x2B)  // After this command will send two bytes
#define S1D15G14_MEMORY_WRITE         (0x2C) // After this command will send n data bytes
#define S1D15G14_COLOUR_SET           (0x2D) // After this command will send 20 bytes of data
#define S1D15G14_RAM_DATA_READ        (0x2E)
#define S1D15G14_PARTIAL_AREA         (0x30)  // After this command will send two bytes
#define S1D15G14_VERTICAL_SCROLL_DEF  (0x33) // After this command will send 3 bytes
#define S1D15G14_TEST_MODE2           (0x34)
#define S1D15G14_TEST_MODE3           (0x35)
#define S1D15G14_MEMORY_ACCESS_CTRL   (0x36)  // After this command will send one byte
#define S1D15G14_VRT_SCROLL_STRT_ADDR (0x37) // Afrer this command will send one byte
#define S1D15G14_IDLE_MODE_OFF        (0x38)
#define S1D15G14_IDLE_MODE_ON         (0x39)
#define S1D15G14_PIXEL_FORMAT         (0x3A)  // After this command will send one byte
#define S1D15G14_TEST_MODE4           (0xDE)
#define S1D15G14_NOP2                 (0xAA)
#define S1D15G14_INITIAL_ESCAPE       (0xC6)
#define S1D15G14_TEST_MODE5           (0xDA)
#define S1D15G14_TEST_MODE6           (0xDB)
#define S1D15G14_TEST_MODE7           (0xDC)
#define S1D15G14_TEST_MODE8           (0xB2)
#define S1D15G14_GRAYSCALE_POS_SET0   (0xB3) // After this command will send 15 bytes
#define S1D15G14_GRAYSCALE_POS_SET1   (0xB4) // After this command will send 15 bytes
#define S1D15G14_GAMMA_CURVE_SET      (0xB5) // After this command will send one byte
#define S1D15G14_DISPLAY_CONTROL      (0xB6) // After this command will send 7 bytes
#define S1D15G14_TEMP_GRADIENT_SET    (0xB7)  // After this command will send 14 bytes
#define S1D15G14_TEST_MODE9           (0xB8)
#define S1D15G14_REFRESH_SET          (0xB9)  // After this command will send one byte
#define S1D15G14_VOLTAGE_CONTROL      (0xBA)  // After this command will send 2 bytes
#define S1D15G14_COMM_DRIVER_OUTPUT   (0xBD)  // After this command will send one byte
#define S1D15G14_POWER_CONTROL        (0xBE)  // After this command will send one byte

























