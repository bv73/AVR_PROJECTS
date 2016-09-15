#define REPER_POINT       (20)  // Реперная точка 20 градусов

// Definitions for DS1621
#define I2C_ADDR_DS1621   0x90
#define DS_Command_Init   0xAC // Access Config Register
#define DS_Command_Start  0xEE // Start Convert T
#define DS_Command_Read   0xAA // Read Temperature
// 0xA1 Access TH
// 0xA2 Access TL
// 0xA8 Read Counter
// 0xA9 Read Slope
// 0x22 Stop Convert T


signed char TerIn_HI; // Данные внутреннего термометра, температура со знаком
unsigned char TerIn_LO; // байт полградуса
unsigned char TerIn_2dT; // модуль от два дельта Т
unsigned char signTintern; // Знак
//unsigned int Tin; // Слово температуры