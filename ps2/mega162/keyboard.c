// ����� ���� ������� 11-������� ���� �������� ~ 0,7 ��;
// ������ ������ ����� ~ 66 ��� (15 ���) (0,066*11=0,726 ��);
// ������� � ������ ����������� ��������� ������� �������� ����� ����� ����� - 33 ���;
// � ���� ������ ��������� ��� ��������� ���-�� ���������.

#include "keyboard.h"
#include "scancodes.h"

//===============================================================

void init_keyboard(void)
{
  input_pointer =  kb_buffer; // ������������� ������
  output_pointer = kb_buffer;
  buffcnt = 0;
  is_up=0;
  shift=0;
  mode=0;

  bitcount = 11;  // ���������=0, 8 ��� ������, �����������, ��������=1
  TimeOut_KBD=0;

  MCUCR = (0<<ISC01)|(1<<ISC00);  // ��������� ���������� INT0 �� ��������� ���. ���������    
  GICR|= (1<<INT0);            // ���������� ���������� INT0
}

//===========================================================
// ������ ��������� ���� ������� � ����� ����������

void put_kbbuff(unsigned char c)
{
    if (buffcnt<KBD_BUFF_SIZE)    // If buffer not full
    {
        *input_pointer = c;                // ��������� ��� ������� � ������
        input_pointer++;                   // Increment pointer

        buffcnt++;    // ������� �������� ����� ������ �����������
    
        if (input_pointer >= kb_buffer + KBD_BUFF_SIZE)        // Pointer wrapping
          {
            input_pointer = kb_buffer;
          }
    }
}

//===========================================================
// ��������� �� ����� �������� �������� ���� � ����������


void decode(unsigned char sc)
{
    unsigned char i;

    // ���� is_up=0
    if (!is_up)       // ��������� �������� ������������� ��� ��������������� ���������� ������
     {
        switch (sc)
        {
          case 0xF0 :        // ������������� ���������� ������
            {
              is_up = 1;
              break;
            }
          case 0x12 :        // ����� SHIFT
            {
              shift = 1;
              break;
            }
          case 0x59 :        // ������ SHIFT
            {
              shift = 1;
              break;
            }
          case 0x05 :        // F1
            {
              if(mode == 0)
                {
                  mode = 1;    // ���� � ����� ����-����
                }
              if(mode == 2)
                {
                  mode = 3;    // ����� �� ������ ����-����
                }
              break;
            }

          default:
            {
              if(mode == 0 || mode == 3)        // ���� ����� ASCII
               {
                  if(!shift)  // ----------- ���� SHIFT �� ������
                    {                            // ����� ���� ����������� �������
                      for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++)
                        {
                          if (unshifted[i][0] == sc) 
                            {
                              put_kbbuff(unshifted[i][1]);
                            }
                        }
                     }
                  else 
                    {          // -------------  ���� SHIFT ������
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
               {                            // ����� ����-����
//                  print_hexbyte(sc);            // ����� ����-����
                  put_kbbuff(' ');
                  put_kbbuff(' ');
               }
              break;
            }
        }
      } 
    else 
     {
        is_up = 0;                            // ��� ���� 0xF0 � ������ ����������
        switch (sc)
        {
          case 0x12 :                        // ����� SHIFT
            {
              shift = 0;
              break;
            }
          case 0x59 :                        // ������ SHIFT
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
// ���������� ������� �� ��� �����, ������ - ��������� ���������� ������ CLK
// ������ - ��������� ������������ ������ CLK.

#pragma vector=INT0_vect
__interrupt void INT0_interrupt(void)
{

    if (TimeOut_KBD==0 && bitcount==11) // �������� �� ������ ���������
      {
        kbd_data_byte=0; // �������� ������ ��� ���������� �����
        kbd_data_mask=0x01;
        TimeOut_KBD=TIMEOUT_KEYBOARD; // �������������� �������� �� ���� �� ������� � ��� ��� �������!
      }
  
    if (KBD_PIN & (1<<KBD_CLK))      // ����� ����� � ����������� �� ���� ����� ������� CLK
      {                            //----------------------- ���. ������� CLK = 1 - �����
        LED_PORT|=(1<<LED_KBD);  // ����� LED

        bitcount--; // �� ������ ��������� ������� �����
        TimeOut_KBD=TIMEOUT_KEYBOARD; // ����� ������ ���������� ���� �� ���� ��������� ��������
        
        if (bitcount == 0)  // ��� ���� ��������? ���������� �� ������������� � � �����
          {
            put_kbbuff(kbd_data_byte); // ���������� �������� ���� � ��������� �������������
// ������������� �������� ��� ��� ������ ���������� ����� �������������� � ���������� �� ��������
          }
        
       } 
    else 
     {                                // --------------------- ���. ������� CLK = 0 - ���� ������ ����
        LED_PORT&=~(1<<LED_KBD); // �������� LED - ������
         if(bitcount > 2 && bitcount < 11)    // ���� � 3 �� 10 � ���� ������. Parity bit,
            {                                    // �����- � ����- ���� ������������
              if(KBD_PIN & (1<<KBD_DATA))  // ����� �������� ���� DATA
                {
                  kbd_data_byte |= kbd_data_mask;  // ���� ���� ������� �� ������ OR ���������� � �����
                }
              kbd_data_mask<<=1; // ������� ����� �����
            }
      }
}

//===========================================================================

unsigned char getchar_kbd(void)
{
    unsigned char byte;
    while(buffcnt == 0);                   // �������� ������

      byte = *output_pointer;                                // Get byte
      output_pointer++;                                    // Increment pointer

      if (output_pointer >= kb_buffer + KBD_BUFF_SIZE)            // �������� ���������
        {
          output_pointer = kb_buffer;
        }
    
      buffcnt--;                                    // Decrement buffer count

    return byte;
}

//==============================================================================
// ��� ������ ��� ����� ����������, ����� ����� ���������� ���� ps2
// ����� ����� ���� ������������� � ��� �����������.
// �.�. ��� �������� ������� ����� ���������� ����� ���� (��������� �������� �� ����),
// �� ���� ��� ������� - ���� ��������� ���������� � �������� ������ � ���� ���������,
// ���� ������������ ���������� �� INT ��������� � ������ ����� - ����� �������� �������.
// �� ��������... ���� ��������... ��������� ������ � ������, ��� ������ � �����������
// ������� �������� ���, ��� �����, ��� ����� ����� ����� ���������� �� ������� �������,
// �� ������, ���� ����� ��� ������� ��� ���� ���������.
// ������ �� ����� �������� ����� ����� ���������� INT �������� ���������.
// ���������� ��������� ������ ���������� ���� � ���, ���� �� ������� �������� �������.
// ������� FF - OK, ���� 0 - ������� ��� ������.
// �� ����� - ���� ��� ��������.

unsigned char keyboard_command(unsigned char comm)
{
  KBD_DDR|=(1<<KBD_CLK); // ����� CLK �� �����, ����� ��������� �����

  KBD_PORT&=~(1<<KBD_CLK); // CLK=0
  delay_us(33);

  //--------------------------- �������� ���������� ���� ----------------------
  KBD_DDR|=(1<<KBD_DATA); // ����� ������ �������� �� �����
  KBD_PORT&=~(1<<KBD_DATA); // Set DATA Low - ��������� ���=0
  delay_us(33);
  KBD_PORT|=(1<<KBD_CLK); // CLK=1
  KBD_DDR&=~(1<<KBD_CLK); // ����� CLK �� ���� - ������ ����� ����� ������� � �� ���������� ������
  // ����� ���� ��� CLK ������� � ����� CLK �������� - ������ ����� ������ �������, 
  // � �� �������� ����������� ��������� ����. ��� ���� ��������� ��� 0 �� ��� ���������.

  TimeOut_KBD=TIMEOUT_KEYBOARD; // ������� ��� �������� ������� �������� ������� �� �����
  while (KBD_PIN & (1<<KBD_CLK)) // ���� ����������� �� ��� ��� ���� CLK=1
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }
  // ��������� ���� CLK ���� 0 - ����� ����� ��� � ���� ������

  // ���� ����������� ����� CLK � �������� ��������� 1
  TimeOut_KBD=TIMEOUT_KEYBOARD;
  // ����, ���� CLK �������� �� 0 � 1
  while (!(KBD_PIN & (1<<KBD_CLK))) // ���� ����������� �� ��� ��� ���� CLK=0
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }

  //------------- ����� ������� ���� �������� �����. ������ ���� ������� ���. ---------------
  unsigned char parity_register=0; // ������� ��� �������� ��������
  unsigned char i; // ������� ���
  for (i=0;i<8;i++)
    {
      // ������� ���������� ��� ������
      if (comm&0x01)
        {
          KBD_PORT|=(1<<KBD_DATA); // ���� ���� 1
          parity_register++; // ���������� ������� ��������� ���
        }
      else
        {
          KBD_PORT&=~(1<<KBD_DATA);
        }

      // ������ ����, ���� ����� �� ��������� ������������ ��� ������ - CLK �������� �� 1 � 0
      TimeOut_KBD=TIMEOUT_KEYBOARD; // ������� 5 �� ��� �������� ������� �������� ������� �� �����
      while (KBD_PIN & (1<<KBD_CLK)) // ���� ����������� �� ��� ��� ���� CLK=1
        {
          if (TimeOut_KBD==0)
            {
              return (0); // ���� ������� �������� �� ������� � �������
            }
        }
      // ��������� ���� CLK ���� 0
      // ���� ����������� ����� CLK � �������� ��������� 1

      TimeOut_KBD=TIMEOUT_KEYBOARD;
      // ����, ���� CLK �������� �� 0 � 1
      while (!(KBD_PIN & (1<<KBD_CLK))) // ���� ����������� �� ��� ��� ���� CLK=0
        {
          if (TimeOut_KBD==0)
            {
              return (0); // ���� ������� �������� �� ������� � �������
            }
        }
      
      comm>>=1; // �������� ���� ������ ������
       
    }

  // Once the Parity bit has been set and the falling edge of the KBD clock detected, 
  // we must release the KBD data line, and wait for another falling edge of the KBD clock 
  // to see if the Keyboard has acknowledged the byte.
  // ����� ���� ��� ��� �������� ��� ���������� � ��������� ����� CLK ��������,
  // �� ������ ���������� ����� ������ � ��������� ���������� ���������� ������ CLK,
  // ����� ������� ��� ����� ����������� ����� �����.

  //----------------------------- �������� ���� �������� ---------------------
  // ���������� �� ����� ������ ��� ��������
  if (parity_register&0x01)
    {
      KBD_PORT&=~(1<<KBD_DATA); // ���� ���� ������� �� ���� ������=0
    }
  else
    {
      KBD_PORT|=(1<<KBD_DATA); // ���� ������=1
    }

  // ������ ����, ���� ����� �� ��������� ������������ ��� ������ - CLK �������� �� 1 � 0
  TimeOut_KBD=TIMEOUT_KEYBOARD; // ������� ��� �������� ������� �������� ������� �� �����
  while (KBD_PIN & (1<<KBD_CLK)) // ���� ����������� �� ��� ��� ���� CLK=1
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }
  // ��������� ���� CLK ���� 0 - ��� �������� ��������� �����
  
  // ���� ����������� ����� CLK � �������� ��������� 1
  TimeOut_KBD=TIMEOUT_KEYBOARD;
  // ����, ���� CLK �������� �� 0 � 1
  while (!(KBD_PIN & (1<<KBD_CLK))) // ���� ����������� �� ��� ��� ���� CLK=0
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }

  //----------------------------- �������� ��������� ���� -----------------------
  // ��������� ���� CLK ���� 1 - ����������� ����� ������, �������� �������� ���.
  
  KBD_PORT|=(1<<KBD_DATA); // ���� ������=1
  KBD_DDR&=~(1<<KBD_DATA); // ����� ������ �� ���� - ����������� ����
  
  TimeOut_KBD=TIMEOUT_KEYBOARD;
  while (KBD_PIN & (1<<KBD_CLK)) // ���� ����������� �� ��� ��� ���� CLK=1
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }
  // ��������� ���� CLK ���� 0
  
  // ���� ����������� ����� CLK � �������� ��������� 1
  TimeOut_KBD=TIMEOUT_KEYBOARD;
  // ����, ���� CLK �������� �� 0 � 1
  while (!(KBD_PIN & (1<<KBD_CLK))) // ���� ����������� �� ��� ��� ���� CLK=0
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }
  // ����� ����� ���� ��� CLK=0 �� ������ ����� ������ - ������������� �����.
  // ���� ����� �������� ����, �� �� ��.

  if (KBD_PIN & (1<<KBD_DATA))
    {
      return (0); // ���� ����� �� �������� ���� �� ������� ��� ������ - ������������
    }

  // ----------------- � ����� ����� - ���� ����������� ����� CLK � �������� ��������� 1 -----------
  TimeOut_KBD=TIMEOUT_KEYBOARD;
  // ����, ���� CLK �������� �� 0 � 1
  while (!(KBD_PIN & (1<<KBD_CLK))) // ���� ����������� �� ��� ��� ���� CLK=0
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }

  TimeOut_KBD=TIMEOUT_KEYBOARD;
  // ����, ���� ����� DATA �����������
  while (!(KBD_PIN & (1<<KBD_DATA))) // ���� ����������� �� ��� ��� ���� DATA=0
    {
      if (TimeOut_KBD==0)
        {
          return (0); // ���� ������� �������� �� ������� � �������
        }
    }
    
  return (0xFF);
}

//===========================================================================
// ��-�� ����, ��� ��������� ���������� � ��-�� ���������� ������ �� ��������
// �������� ������ ����������� �����������

unsigned char kbd_command(unsigned char cmd)
{
  unsigned char responce;

  // �� ���� ���� ���� �������� �� ������ ��������� �.�. ���������� ������� ������������
  // ���������� TimeOut_KBD � bitcount
  while (!(TimeOut_KBD==0 && bitcount==11)); // ������� ���� ��� ���� �������� - ���� ���� �����������

  GICR&= ~(1<<INT0);  // ������ ��� ������� ����� CLK ��������� ���������� INT0, ����� ������ ���� :)
  
  responce=keyboard_command(cmd);

  // ��� ����������� �� ����, �� ��������� ��� ��� - ���������� ��������� ���������� ��� ������ ���� � �����
  TimeOut_KBD=TIMEOUT_KEYBOARD;
  GICR|= (1<<INT0); // ���������� ���������� INT0

  return (responce); // ������� ������
}