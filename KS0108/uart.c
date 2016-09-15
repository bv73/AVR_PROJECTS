// Используем UART0 для приема и передачи информации
void InitUART (void)
#define FOSC      (8000000)
#define Baudrate  (1200)
#define UBRR_Value (FOSC/(16*Baudrate))-1
{  
  UBRR0H=UBRR_Value>>8; // Задание скорости UART0
  UBRR0L=UBRR_Value&0xFF;
  UCSR0A=(0<<U2X0); // Задание асинхронного режима и Double Speed
  UCSR0B=(1<<RXEN0)+(1<<TXEN0); //  Включение приема и передачи
}

// Отправка байта в последовательный порт
void _pch(char c)
{
  while(!UCSR0A_UDRE0); // Ждем, пока регистр передачи будет пуст
  UDR0=c; // Ложим в регистр передачи байтик
}

void _ps_P(char __flash *p)
{
  char c;
  while ((c=*p++)) _pch(c);
}
