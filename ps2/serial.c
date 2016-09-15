
//#define ESC 0x1b
#define SERIAL_BUFF_SIZE  (32)

//static __flash char CLR[] = {ESC, '[','H', ESC, '[', '2', 'J',0};

char UART_buffer[SERIAL_BUFF_SIZE];
char *inptr, *outptr;
unsigned char buff_cnt;


void init_uart(void)
{
    inptr =  UART_buffer;
    outptr = UART_buffer;
    buff_cnt = 0;
}

/*
void clr(void)
{
    puts_P(CLR);        // Передача команды "очистки экрана" ("clear screen") терминалу VT100
}
*/

unsigned char putchar_serial(unsigned char c)
{
    if (buff_cnt<SERIAL_BUFF_SIZE) 
     {
        *inptr = c;                             // Put character into buffer
        inptr++;                                // Increment pointer
        
        buff_cnt++;
        
        if (inptr >= UART_buffer + SERIAL_BUFF_SIZE)   // Просмотр указателя
          {
            inptr = UART_buffer;
          }
 
        UCSR0B = (1<<UDRIE0)|(1<<TXEN0);  // Активизация прерывания при очистке регистра данных UART (0x28)
        
        return 1;
      } 
    else 
      {
        return 0;                               // Буфер заполнен
      }
        
}


void print_hexbyte(unsigned char i)
{
    unsigned char h, l;
    
    h = i & 0xF0;               // High nibble
    h = h>>4;
    h = h + '0';
    
    if (h > '9')
      {
        h = h + 7;
      }

    l = (i & 0x0F)+'0';         // Low nibble
    if (l > '9')
      {
        l = l + 7;
      }

    putchar_serial(h);
    putchar_serial(l);
}


// Управление прерыванием передатчика

#pragma vector=USART0_UDRE_vect
__interrupt void USART0_UDRE_interrupt(void)
{
    UDR0 = *outptr;                              // Посылка следующего байта
    outptr++;                                   // Увеличение указателя
    
    if (outptr >= UART_buffer + SERIAL_BUFF_SIZE)      // Просмотр указателя
      {
        outptr = UART_buffer; // Если указатель превысил размер буфера то обнуляем (кольцевой буфер)
      }

    buff_cnt--;

    if(buff_cnt == 0)                         // Если буфер пуст:
      {
        UCSR0B &= ~(1<<TXCIE0);        //   выключаем прерывания
      }
}
    
