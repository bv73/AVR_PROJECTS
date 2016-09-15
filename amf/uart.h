#define QUARTZ    (8000000)
#define SIZE_BUF  (128)   // Размер буфера приема/передачи
#define Baudrate  (9600)
#define TIMEOUT   (3)     // Timeout = 3 ms
#define Baudrate_Register ((unsigned short)(((unsigned long)QUARTZ)/(16UL*Baudrate))-1)

unsigned char HowMany_TX;  // Сколько байт передать в прерывании
volatile unsigned char HowMany_RX;  // Количество принятых байт
volatile unsigned char Pointer_TX; // Указатель текущего положения буфера передачи
volatile unsigned char TimeOut_RX;  // Таймаут по приему (в мс)
volatile unsigned char ReceiveOK;  // Флаг принятой информации, если не ноль, то принят валидный пакет.
volatile unsigned char SecundaOK;   // Секундный флаг. Если не ноль, то прошла секунда

char UART_Buf[SIZE_BUF]; // Буфер приемника-передатчика UART



