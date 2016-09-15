//#define I2C_PORT  PORTC
//#define I2C_PIN   PINC
//#define I2C_DIR   DDRC

#define SCL_PORT    PORTD_Bit6
#define SCL_PIN     PIND_Bit6
#define SCL_DDR     DDRD_Bit6

#define SDA_PORT    PORTD_Bit7
#define SDA_PIN     PIND_Bit7
#define SDA_DDR     DDRD_Bit7

#define I2C_SCL_1   SCL_DDR=0
#define I2C_SCL_0   SCL_DDR=1

#define I2C_SDA_1   SDA_DDR=0
#define I2C_SDA_0   SDA_DDR=1


unsigned char error_i2c; // Регистр ошибки I2C: 0 - OK, 0xFF - Error