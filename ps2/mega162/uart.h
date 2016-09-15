#define SIZE_BUF        (64)   // Размер буфера приема/передачи
#define MCU_QUARTZ      (8000000)
#define Host_Baudrate   (9600)
#define TIMEOUT_UART    (3)     // Timeout = 3 ms
#define Baudrate_Register ((unsigned short)(((unsigned long)MCU_QUARTZ)/(16UL*Host_Baudrate))-1)

//unsigned char *Pointer_BUF=&UART_Buf[SIZE_BUF]; // Указатель адреса буфера
unsigned char HowMany_TX;  // Сколько байт передать в прерывании
unsigned char HowMany_RX;  // Количество принятых байт
unsigned char Pointer_TX; // Указатель текущего положения буфера передачи
unsigned char TimeOut_RX;  // Таймаут по приему (в мс)
unsigned char ReceiveOK;  // Флаг принятой информации, если не ноль, то принят валидный пакет.

unsigned char LED_counter;

char UART_Buf[SIZE_BUF]; // Буфер приемника-передатчика UART



