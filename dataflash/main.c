/* 
Адаптация для Стартеркита MEGA128 и оптимизация кода by (R)soft 27.5.20011
Version 1.1
Цифровое устройство записи голоса:
  => 8-битовая запись звука 
  => Частота дискретизации 8 кГц 
  => Частота звука до 4 кГц 
  => Максимальное время записи 8 минут

Очистка всех страниц в случае необходимости. Запись данных в буфер 1. Если буфер заполнен,
то его содержимое записывается в страницу памяти. Чтение DataFlash через буфер 1 и буфер 2 в
регистр данных.

MCU - ATmega128 (адаптировано для стартеркита)
Кнопки: 1 - PLAY
        2 - ERASE
        3 - RECORD
DataFlash - AT45DB321 (32 Mbit, 8192 страниц по 528 байт)
FOSC - 8 MHz (расчетное), реальное 14.318 МГц
PORTB - SPI DataFlash Interface (3 Lines)+ Control DataFlash Interface (2 Lines)
PORTB:
  PB0 - nc
  PB1 - SCK
  PB2 - MOSI
  PB3 - MISO (input pin)
  PB4 - Ready_Busy (input pin)
  PB5 - Chip Select
  PB6 - PWM Output (выход звука)
  PB7 - nc
PORTF - ADC Input (1 Line)
  PF0 - ADC input
PORTE - Keys for Select Modes (3 Keys)
  PE4 - PLAY Button
  PE5 - ERASE Button
  PE6 - RECORD Button
PORTD - Indication
  PD6 - LED Status
Samlerate of ADC ~ 15686 Hz
*/

#include "iom128.h"
#include <ina90.h>
#include "stdlib.h"
#include "dataflash.h"

// прототипы
void setup (void);
void erasing (void);
void recording (void);
void write_to_flash (unsigned char ad_data);
void playback (void);
void next_page_to_next_buffer (unsigned char active_buffer, unsigned int page_counter);
void active_buffer_to_speaker (unsigned char active_buffer);

// глобальные переменные
volatile unsigned char wait = 0;

#define Button_Play   0x10 // PE 4
#define Button_Erase  0x20 // PE 5
#define Button_Record 0x40 // PE 6
#define Data_LED      0x40 // PD 6

void setup(void)
{
  DDRB = 0xE7;	// 1110 0111 Инициализация порта SPI
                // nc,  PWM, CS,  RDY, MISO, MOSI, SCK, nc
                // PB7, PB6, PB5, PB4, PB3, PB2 , PB1, PB0
                //  O    O    O    I    I    O     O    O
                //  1    1    1    0    0    1     1    1
  PORTB = 0xFF;	// все выходы в высоком состоянии, на входах нагрузочные резисторы
  DDRD =  0xFF; // Порт светодиодов на вывод
  PORTD = 0x00; // Погасить светодиоды
  DDRE =  0x00; // Port E определяется как вход (For Buttons)
  PORTE = 0xFF; // Подтяжка входов кнопочек
  DDRF =  0x00; // Port F определяется как вход (For ADC)
  PORTF = 0x00; // Без резисторов подтяжки

  __enable_interrupt();
}

void erasing(void)
{
  unsigned int block_counter = 0;
  unsigned char temp = 0x80;
  ACSR |= 0x02; 	// установка флага, показывающего, что следующим  
                        //   этапом должна быть запись данных

  // прерывания запрещены, порт SPI включён, 
  //  «ведущий» режим, первый MSB, 3 режим SPI, Fcl/4
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA); // Включение SPI

  while (block_counter < 512)
  {
    PORTB &= ~DF_CHIP_SELECT; // включение DataFlash

    SPDR = BLOCK_ERASE;
    while (!(SPSR & temp));   // ожидание завершения передачи

    SPDR = (char)(block_counter>>3);
    while (!(SPSR & temp));   // ожидание завершения передачи

    SPDR = (char)(block_counter<<5);
    while (!(SPSR & temp));   // ожидание завершения передачи

    SPDR = 0x00; 	      // не важно
    while (!(SPSR & temp));   // ожидание завершения передачи

    PORTB |= DF_CHIP_SELECT;  // выключение DataFlash

    block_counter++;
    while(!(PINB & DF_RDY_BUSY)); 	// ожидание очистки блока
  }

  SPCR = 0x00; 			//отключение SPI
}


void recording(void)
{
  // прерывания запрещены, порт SPI включён, «ведущий» режим, первый MSB, 3 режим SPI, Fcl/4
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA);  // 0x5C=92 0101 1100
                // SPI Control Register SPCR (SPIE=0, SPE=1, DORD=0, MSTR=1, CPOL=1, CPHA=1, SPR1=0, SPR0=0)
                // SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
  ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<ADLAR); // Если ADLAR=1 то берем результат из ADCH, если ADLAR=0 то из ADCL
                // номер входного вывода АЦП = 0, Internal VREF=2.56 V (REFS0=1 & REFS1=1)
                // (REFS1=1, REFS0=1, ADLAR=?, MUX4=0, MUX3=0, MUX2=0, MUX1=0, MUX0=0)
  ADCSR = (1<<ADEN)|(1<<ADSC)|(1<<ADIF)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS0); // 0xDD=221 1101 1101
                // одиночное АЦ преобразование, fCK/32, старт преобразования
                // ADC Control and Status Register ADCSR
                // (ADEN=1, ADSC=1, ADFR=0, ADIF=1, ADIE=1, ADPS2=1, ADPS1=0, ADPS0=1)
                // ADEN=1 (Enable ADC), ADSC=1 (Single Conv), ADFR=0 (No Free Run), ADIF=1 (Interrupt Flag)
                // ADIE=1 (Interrupt Enable), ADPS2=1, ADPS1=0, ADPS0=1 (Division Factor = 32)
  while (!(PINE & Button_Record)); // цикл продолжается пока нажата кнопка записи (кнопка 3)

  ADCSR = 0x00; // выключение АЦП
  SPCR = 0x00;  // выключение SPI
}

#pragma vector=ADC_vect
__interrupt void sample_ready(void)
{
  unsigned char count = 0;

  while (count < 6) count++; // ожидание в течение нескольких циклов
  ADCSR|=(1<<ADSC);            // 0x40 старт нового АЦ преобразования
  write_to_flash(ADCH); // (ADLAR=1) чтение данных, сохранение во флэш
}


void write_to_flash(unsigned char flash_data)
{
  static unsigned int buffer_counter;
  static unsigned int page_counter;
  unsigned char temp = 0x80;

  if((ACSR & 0x02)) // если флаг установлен, то новые данные должны быть установлены
    {
      buffer_counter = 0;
      page_counter = 0;   // сброс счётчика если должны быть записаны новые данные 
      ACSR &= 0xFD; 	// очистка флага сигнала
     }

  while(!(PINB & DF_RDY_BUSY)); 	// проверка занятости флэша

  PORTB &= ~DF_CHIP_SELECT; 	// включение DataFlash

  SPDR = BUFFER_1_WRITE;
  while (!(SPSR & temp)); 	// ожидание завершения передачи
  SPDR = 0x00; 			// не важно
  while (!(SPSR & temp)); 	// ожидание завершения передачи

  SPDR = (char)(buffer_counter>>8); // не важно + первые два бита буфера адреса
  while (!(SPSR & temp)); 		 // ожидание завершения передачи

  SPDR = (char)buffer_counter; 	 // буфер адреса (макс. 2^8 = 256 страниц)
  while (!(SPSR & temp)); 		 // ожидание завершения передачи
  SPDR = flash_data; 		// запись данных в регистр данных SPI
  while (!(SPSR & temp)); 	// ожидание завершения передачи

  PORTB |= DF_CHIP_SELECT; 	// выключение DataFlash

  buffer_counter++;

  if (buffer_counter > bytes_per_page) 	// если буфер заполнен, то его 
                                //содержимое записывается в страницу памяти 
    {
      buffer_counter = 0;
      if (page_counter < dataflash_pages) // если память не заполнена
      {
        PORTB &= ~DF_CHIP_SELECT; 	// включить DataFlash

        SPDR = B1_TO_MM_PAGE_PROG_WITHOUT_ERASE; // записать данные из буфера 1 в страницу
        while (!(SPSR & temp)); 	// ожидание завершения передачи
        SPDR = (char)(page_counter>>6);
        while (!(SPSR & temp)); 	// ожидание завершения передачи
        SPDR = (char)(page_counter<<2);
        while (!(SPSR & temp)); 	// ожидание завершения передачи
        SPDR = 0x00; 			// не важно
        while (!(SPSR & temp)); 	// ожидание завершения передачи

        PORTB |= DF_CHIP_SELECT; 	// выключение DataFlash

        page_counter++;
      }
      else
      {
        PORTD &= ~Data_LED; 	 // погасить LED 
        while (!(PINE & Button_Record)); // ждать пока кнопка записи не отпущена (кнопка 3)
      }
    }
}


void playback(void)
{
  unsigned int page_counter = 0;
  unsigned char active_buffer = 1; // активный буфер = буфер 1

  TCCR1A = (1<<COM1B1)|(1<<WGM10); 	// 0x21 0010 0001 - 8 бит ШИМ, используется COM1B
  TCNT1 = 0x00; 	// обнуление счётчика 1
  TIFR = (1<<TOV1); 		// 0x04 0000 0100 - сброс флага переполнения  счётчика 1 
  TIMSK = (1<<TOIE1); 	// 0x04 0000 0100 - разрешение прерывания переполнения счётчика 1 
  TCCR1B = (1<<CS10); 	// 0x01 0000 0001 - коэф. Пересчёта счётчика 1 = 1
  OCR1B = 0x00;		// обнуление выходного регистра сравнения B

    // прерывания запрещены, порт SPI включён, «ведущий» режим, первый MSB, 3 режим SPI, Fcl/4
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA);  // 0x5C=92 0101 1100

  next_page_to_next_buffer (active_buffer, page_counter); // чтение страницы 0 в буфер 1
	
  while (!(PINB & DF_RDY_BUSY)); // ожидание завершения передачи данных из страницы 0 в буфер 1
  while ((page_counter < dataflash_pages-1)&(!(PINE & Button_Play))) // пока кнопка воспроизведения (кнопка 1) нажата
    {
      page_counter++; // теперь берём следующую страницу

      next_page_to_next_buffer (active_buffer, page_counter);
      active_buffer_to_speaker (active_buffer);

      if (active_buffer == 1) // если буфер 1 является активным буфером
        {
          active_buffer++; 	// то устанавливаем буфер 2 в качестве активного
        }
      else 			// иначе
        {
          active_buffer--; 	// устанавливаем буфер 1 в качестве активного
        }
      }
  TIMSK = 0x00; 	// запрещаем все прерывания
  TCCR1B = 0x00; 	// останавливаем счётчик 1
  SPCR = 0x00; 		// отключаем SPI
}


void next_page_to_next_buffer (unsigned char active_buffer, unsigned int page_counter)
{
  unsigned char temp = 0x80;
  while(!(PINB & DF_RDY_BUSY)); 	// ждём, пока флэш не освободится

  PORTB &= ~DF_CHIP_SELECT;     // включаем DataFlash

  if (active_buffer == 1) 	// если буфер 1 активный
    {
      SPDR = MM_PAGE_TO_B2_XFER; // то передаём следующую страницу в буфер 2
    }
  else	 				// иначе
    {
      SPDR = MM_PAGE_TO_B1_XFER; // передаём следующую страницу в буфер 1
    }
  while (!(SPSR & temp)); // ожидаем завершения передачи
  SPDR = (char)(page_counter >> 6);
  while (!(SPSR & temp)); // ожидаем завершения передачи
  SPDR = (char)(page_counter << 2);
  while (!(SPSR & temp)); // ожидаем завершения передачи
  SPDR = 0x00; // записываем не имеющий значения байт
  while (!(SPSR & temp)); // ожидаем завершения передачи
  PORTB |= DF_CHIP_SELECT; // выключаем DataFlash и начинаем передачу
}

#pragma vector=TIMER1_OVF_vect
__interrupt void out_now(void)
{
  wait = 0; // возникновение прерывания
}

void active_buffer_to_speaker (unsigned char active_buffer)
{
  // пока активный буфер не очистится воспроизводим его содержимое на динамике
  unsigned int buffer_counter = 0;
  unsigned char temp = 0x80;

  PORTB &= ~DF_CHIP_SELECT; // включение DataFlash

  if (active_buffer == 1)   // если буфер 1 активный буфер
    {
      SPDR = BUFFER_1_READ; // то читаем из буфера 1
    }
  else 	// иначе
    {
      SPDR = BUFFER_2_READ; // читаем из буфера 2
    }

  while (!(SPSR & temp)); // ожидаем завершения передачи 
  SPDR = 0x00;            // запись не имеющего значения байта
  while (!(SPSR & temp)); // ожидаем завершения передачи
  SPDR = 0x00;            // запись не имеющего значения байта
  while (!(SPSR & temp)); // ожидаем завершения передачи
  SPDR = 0x00;            // начать с адреса 0 буфера
  while (!(SPSR & temp)); // ожидаем завершения передачи
  SPDR = 0x00;            // запись не имеющего значения байта
  while (!(SPSR & temp)); // ожидаем завершения передачи

  while (buffer_counter < bytes_per_page)
    {
      SPDR = 0xFF; // записываем фиктивное значение в начало сдвигового регистра
      while (!(SPSR & temp)); // ожидаем завершения передачи
      while(wait); // ожидаем прерывание переполнения таймера 1
      OCR1B = SPDR; // воспроизводим данные из сдвигового регистра
      wait = 1; // сброс флага сигнала
  
      buffer_counter++;
    }

  PORTB |= DF_CHIP_SELECT; // выключение DataFlash
}

// ==========================================================================
// Кнопка 1 - PLAY
// Кнопка 2 - ERASE
// Кнопка 3 - RECORD
// ===========================================================================

// static __flash char Copyright[] = {"By (R)soft 2011"};

void main(void)
{
  setup();
//  SPSR = (1<<SPI2X); // Удваиваем скорость SPI
  
  for(;;)
   {
      if (!(PINE & Button_Record)) // если кнопка записи нажата (кнопка 3)
        {
          PORTD |= Data_LED; // зажигаем LED
          recording();
        }
      if (!(PINE & Button_Erase)) // если нажата кнопка очистки (кнопка 2)
        {
          PORTD |= Data_LED; // зажигаем LED
          erasing();
          while (!(PINE & Button_Erase)); // ждём пока кнопка очистки не отпущена (кнопка 2)
        }
      if (!(PINE & Button_Play)) //если нажата кнопка воспроизведения(кнопка 1)
        {
          PORTD |= Data_LED; // зажигаем LED
          playback();
          while (!(PINE & Button_Play)); // ждём пока кнопка воспроизведения не отпущена (кнопка 1)
        }
      PORTD &= ~Data_LED; // гасим LED во время «холостой» работы
//      PORTA=Copyright[0];
   }
}
