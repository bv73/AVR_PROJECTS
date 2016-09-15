// Длина всей посылки 11-битного кода занимает ~ 0,7 мс;
// Период одного клока ~ 66 мкс (15 кГц) (0,066*11=0,726 мс);
// Верхний и нижний полупериоды тактового сигнала примерно равны между собой - 33 мкс;
// У двух разных клавиатур эти временные хар-ки идентичны.

#include "keyboard.h"
#include "scancodes.h"

//===============================================================

void init_kb(void)
{
    inpt =  kb_buffer; // Инициализация буфера
    outpt = kb_buffer;
    buffcnt = 0;

    MCUCR = (0<<ISC01)|(1<<ISC00);  // Установка прерывания INT0 по изменению лог. состояния
//    edge_flag = 0;       // Флаг фронта: 0 = спадающий фронт 1 = нарастающий фронт
    bitcount = 11;  // Стартовый=0, 8 бит данных, проверочный, стоповый=1
//    kbd_data_byte=0;
}

//===========================================================
// Запись принятого кода клавиши в буфер клавиатуры

void put_kbbuff(unsigned char c)
{
    if (buffcnt<KBD_BUFF_SIZE)    // If buffer not full
    {
        *inpt = c;                // Сохраняем код клавиши в буфере
        inpt++;                   // Increment pointer

        buffcnt++;    // Счетчик принятых кодов клавиш увеличиваем
    
        if (inpt >= kb_buffer + KBD_BUFF_SIZE)        // Pointer wrapping
          {
            inpt = kb_buffer;
          }
    }
}

//===========================================================
// Процедура всё время получает принятый байт с клавиатуры


void decode(unsigned char sc)
{
    unsigned char i;

    // Если is_up=0
    if (!is_up)       // Последний принятый идентификатор был идентификатором отпускания кнопки
     {
        switch (sc)
        {
          case 0xF0 :        // Идентификатор отпускания кнопки
            {
              is_up = 1;
              break;
            }
          case 0x12 :        // Левый SHIFT
            {
              shift = 1;
              break;
            }
          case 0x59 :        // Правый SHIFT
            {
              shift = 1;
              break;
            }
          case 0x05 :        // F1
            {
              if(mode == 0)
                {
                  mode = 1;    // Вход в режим скан-кода
                }
              if(mode == 2)
                {
                  mode = 3;    // выход из режима скан-кода
                }
              break;
            }

          default:
            {
              if(mode == 0 || mode == 3)        // Если режим ASCII
               {
                  if(!shift)                    // Если SHIFT не нажата
                    {                            // тогда надо просмотреть таблицу
                      for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++)
                        {
                          if (unshifted[i][0] == sc) 
                            {
                              put_kbbuff(unshifted[i][1]);
                            }
                        }
                     }
                  else 
                    {                    // если SHIFT нажата
                      for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++)
                        {
                          if (shifted[i][0] == sc) 
                            {
                              put_kbbuff(shifted[i][1]);
                            }
                        }
                    }
               }
              else
               {                            // режим скан-кода
//                  print_hexbyte(sc);            // Вывод скан-кода
                  put_kbbuff(' ');
                  put_kbbuff(' ');
               }
              break;
            }
        }
      } 
    else 
     {
        is_up = 0;                            // Два кода 0xF0 в строке невозможно
        switch (sc)
        {
          case 0x12 :                        // Левый SHIFT
            {
              shift = 0;
              break;
            }
          case 0x59 :                        // Правый SHIFT
            {
              shift = 0;
              break;
            }
          case 0x05 :                        // F1
            {
              if(mode == 1)
                {
                  mode = 2;
                }
              if(mode == 3)
                {
                  mode = 0;
                }
              break;
            }
          case 0x06 :                        // F2
            {
//              clr();
              break;
            }
         } 
      }   
} 

//=============================================================================
// Прерывание разбито на две ветки, первая - обработка спадающего фронта CLK
// Вторая - обработка нарастающего фронта CLK.

#pragma vector=INT0_vect
__interrupt void INT0_interrupt(void)
{

    if (TimeOut_KBD==0 && bitcount==11) // Проверка на первое вхождение
      {
        kbd_data_byte=0; // Исходные данные для заполнения байта
        kbd_data_mask=0x01;
        TimeOut_KBD=TIMEOUT_KEYBOARD; // Переустановкой таймаута не ноль мы говорим о том что поехало!
      }
  
    if (KBD_PIN & (1<<KBD_CLK))      // Выбор ветки в зависимости от того какой уровень CLK
      {                            //----------------------- Лог. уровень CLK = 1 - выход
        LED_PORT|=(1<<LED_KBD);  // Гасим LED

        bitcount--; // По выходу уменьшаем счетчик битов
        TimeOut_KBD=TIMEOUT_KEYBOARD; // После приема очередного бита не даем наступить таймауту
        
        if (bitcount == 0)                    // Все биты получены?
          {
            put_kbbuff(kbd_data_byte); // Отправляем принятый байт в процедуру декодирования
//            bitcount = 11;  // Инициализируем счетчик бит для следующего приема байта
          }
        
       } 
    else 
     {                                // --------------------- Лог. Уровень CLK = 0 - вход опроса бита
        LED_PORT&=~(1<<LED_KBD); // Включаем LED - начало
         if(bitcount > 2 && bitcount < 11)    // Биты с 3 по 10 – биты данных. Parity bit,
            {                                    // старт- и стоп- биты игнорируются
              if(KBD_PIN & (1<<KBD_DATA))  // Берем значение бита DATA
                {
                  kbd_data_byte |= kbd_data_mask;  // Если была единица то делаем OR результата и маски
                }
              kbd_data_mask<<=1; // Двигаем маску влево
            }
      }
}

//--------------------------------------------------------

unsigned char getchar_kbd(void)
{
    unsigned char byte;
    while(buffcnt == 0); // Ожидание данных - крутит в цикле пока счетчик=0

      byte = *outpt;                                // Get byte
      outpt++;                                    // Increment pointer

      if (outpt >= kb_buffer + KBD_BUFF_SIZE)            // Просмотр указателя
        {
          outpt = kb_buffer;
        }
    
      buffcnt--;                                    // Decrement buffer count

    return byte;
}

