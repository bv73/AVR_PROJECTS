// I2C Special for VAmeter

#define SCL_PORT    PORTB_Bit1
#define SCL_PIN     PINB_Bit1
#define SCL_DDR     DDRB_Bit1

#define SDA_PORT    PORTB_Bit2
#define SDA_PIN     PINB_Bit2
#define SDA_DDR     DDRB_Bit2

#define I2C_SCL_1   SCL_DDR=0
#define I2C_SCL_0   SCL_DDR=1

#define I2C_SDA_1   SDA_DDR=0
#define I2C_SDA_0   SDA_DDR=1


volatile unsigned char error_i2c; // Регистр ошибки I2C: 0 - OK, 0xFF - Error