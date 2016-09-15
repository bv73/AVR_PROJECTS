#define SIZE_BUF        (256)   // Размер буфера приема/передачи
#define ModBus_Baudrate (9600)
#define TIMEOUT         (3)     // Timeout = 3 ms
#define Baudrate_Register ((unsigned short)(((unsigned long)BSI_QUARTZ)/(16UL*ModBus_Baudrate))-1)

unsigned char Net_Addr; // Сетевой адрес платы в системе (1 или 2)

//unsigned char *Pointer_BUF=&UART_Buf[SIZE_BUF]; // Указатель адреса буфера
unsigned char HowMany_TX;  // Сколько байт передать в прерывании
volatile unsigned char HowMany_RX;  // Количество принятых байт (unsigned int потому, что длина буфера 300 байт)
volatile unsigned char Pointer_TX; // Указатель текущего положения буфера передачи
volatile unsigned char TimeOut_RX;  // Таймаут по приему (в мс)
volatile unsigned char ReceiveOK;  // Флаг принятой информации, если не ноль, то принят валидный пакет.
volatile unsigned char SecundaOK;   // Секундный флаг. Если не ноль, то прошла секунда

unsigned char UART_Buf[SIZE_BUF]; // Буфер приемника-передатчика UART



