#define I2C_PORT  PORTB
#define I2C_PIN   PINB
#define I2C_DIR   DDRB

#define SCL_PIN   PB0
#define SDA_PIN   PB1
#define PWR_PIN   PB2

//#define I2C_SCL_LO()  cbi(I2C_PORT,SCL_PIN)
//#define I2C_SCL_HI()  sbi(I2C_PORT,SCL_PIN)

//#define I2C_SDA_LO()  cbi(I2C_PORT,SDA_PIN)
//#define I2C_SDA_HI()  sbi(I2C_PORT,SDA_PIN)

#define I2C_SCL_In()  cbi(I2C_DIR,SCL_PIN)
#define I2C_SCL_Out() sbi(I2C_DIR,SCL_PIN)

#define I2C_SDA_In()  cbi(I2C_DIR,SDA_PIN)
#define I2C_SDA_Out() sbi(I2C_DIR,SDA_PIN)

#define I2C_SDA_Get() (I2C_PIN&(1<<SDA_PIN))

#define I2C_Power_OFF() cbi(I2C_PORT,PWR_PIN)
#define I2C_Power_ON()  sbi(I2C_PORT,PWR_PIN)

volatile unsigned char Error_I2C; // Регистр ошибки I2C: 0 - OK, 0xFF - Error