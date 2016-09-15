unsigned char colors[]= { 0x1F, 0xE3, 0xFC, 0x03, 0xE0, 0xFF, 0x00 };

enum color_numbers { red,green,blue,yellow,sky,black,white };

/*
#define red    0b 0001 1111     // 0x1F
#define green  0b 1110 0011     // 0xE3
#define blue   0b 1111 1100     // 0xFC
#define yellow 0b 0000 0011     // 0x03
#define sky    0b 1110 0000     // 0xE0
#define black  0xff
#define white 0x00
*/

//#define CMD    0
//#define DATA   1

/*
void send(unsigned char nokia_data, unsigned char cd);
void nokia_cs(void);

void nokia_printchar(unsigned char caracter);
void PrintByte(unsigned char bytefornokia);
void nokia_outs(unsigned char * s, unsigned char x, unsigned char y);
void nokia_senddata(unsigned char nokia_data);
void clear_screen(unsigned char fondo);
*/

unsigned char c;
unsigned char bgcolor=red;
unsigned char fgcolor=black;

void send_cmd(unsigned char ch) 
{
  unsigned char i;

  A0_0; // Признак команды A0=0

  CS_0;
  SCLK_0;   

  for (i=8;i;i--)
    {
      if (ch&0x80)
        {
          SDATA_1;
        }
      else
        {
          SDATA_0;
        }
      SCLK_1;
      ch<<=1;
     }

  CS_1;
}

void send_data(unsigned char ch)
{
  unsigned char i;
  A0_1; // Признак данных A0=1

  CS_0;
  SCLK_0;

  for (i=8;i;i--)
    {
      if (ch&0x80)
        {
          SDATA_1;
        }
      else
        {
          SDATA_0;
        }
      SCLK_1;
      ch<<=1;
     }

  CS_1;
}



void clear_screen(unsigned char fillbyte)
{
   unsigned int i;
   
   send_cmd(0x2C);           // Memory Write
   for(i=0; i<6566; i++)
     {
        send_data(fillbyte);             // clear screen
     }
//   CS_1;

}



//=========================================================================================
void nokiacolorinit(void)
{
  unsigned int i;
       
  PIN_A0=0;
  PIN_SCLK=0;
  PIN_SDATA=0;
  PIN_CS=0;
  PIN_RESET=0;
  CS_1;
  SCLK_1;

  delay_ms(250);

  // ============= Display Init from Page 79 of Datasheet =====================
  // LCD Hardware Reset
  RESET_0;
  delay_ms(1);
  RESET_1;
  delay_ms(10);
    
  send_cmd(S1D15G14_SOFTWARE_RESET);      // LCD Software Reset
  delay_ms(10);

  send_cmd(S1D15G14_INITIAL_ESCAPE);      // Initial Escape

  //-------------- Display Setup 1
  
  send_cmd(S1D15G14_REFRESH_SET);      // Refresh set
  send_data(0x00);

  send_cmd(S1D15G14_DISPLAY_CONTROL);      // Display Control
  send_data(128);  // P17...P10 Oscillation
  send_data(128);  // P27...P20 Set N inversions
  send_data(129);  // P37...P30 Set Frame Frequency, BIAS Voltage(000), Direction Size RAM, Duty (1/67)
  send_data(84);   // P46...P40 Set Duty in 1/82
  send_data(69);   // P56...P50 Set Duty in 1/67
  send_data(82);   // P66...P60 Set Duty in 1/82
  send_data(67);   // P77...P70 Set Duty in 1/67

  send_cmd(S1D15G14_GRAYSCALE_POS_SET0);      // Gray Scale Position GCPSET0
  send_data(1);
  send_data(2);
  send_data(4);
  send_data(8);
  send_data(16);
  send_data(30);
  send_data(40);
  send_data(50);
  send_data(60);
  send_data(70);
  send_data(80);
  send_data(90);
  send_data(100);
  send_data(110);
  send_data(127);

  send_cmd(S1D15G14_GRAYSCALE_POS_SET1);      // Gray Scale Position GCPSET1
  send_data(1);
  send_data(2);
  send_data(4);
  send_data(8);
  send_data(16);
  send_data(30);
  send_data(40);
  send_data(50);
  send_data(60);
  send_data(70);
  send_data(80);
  send_data(90);
  send_data(100);
  send_data(110);
  send_data(127);

  send_cmd(S1D15G14_GAMMA_CURVE_SET);           // Gamma Curve Set
  send_data(0x01); // P11,P10=01 (GCP0)
   
  send_cmd(S1D15G14_COMM_DRIVER_OUTPUT);           // Common Driver Output
  send_data(0x00); // P13...P10 - 0000
   
  // ----------- Power Supply Setup ------------------

  send_cmd(S1D15G14_POWER_CONTROL);           // Power Control
  // 0 P16 P15 P14 | P13 P12 P11 P10
  // 0  0   0   1  |  0   1   0   0
  // P16 - Boosting Clock (0 - Normal)
  // P15 - Always 0
  // P14 - Switch External/Internal Resistance 0:Internal, 1:External
  // P13 - Change Driving Capacity V1OUT 0:High-power mode 1:Low-power mode
  // P12..P10 - Set Freq Boosting 000:f/512 001:f/256 010:f/128 011:f/64 100:f/32
  send_data(0x14); // P16...P11

  send_cmd(S1D15G14_SLEEP_OUT);           // Sleep out
   
  send_cmd(S1D15G14_VOLTAGE_CONTROL);
  send_data(0x00); // P16...P10 Set Electronic Volume
  send_data(0x03);   // P21..P20 = Always 11
   
  // Set contrast 0...127
  // 63 - середина
  send_cmd(S1D15G14_WRITE_CONTRAST);           // Write contrast
  send_data(63);           // was 63

  send_cmd(S1D15G14_TEMP_GRADIENT_SET);           // Temperature gradient set
  for(i=0; i<14; i++)
    {
      send_data(0);
    }
  
  send_cmd(S1D15G14_BOOSTER_VOLTAGE_ON);           // Booster Voltage ON
  delay_ms(40);
  
  // ------------- Display Setup 2 ------------------
   
  send_cmd(S1D15G14_INVERSION_OFF);           // Invert colors off   
  
  // Set Partial Area
  
  // Verticall Scroll Definitions
   
  // Set Vertical Srolling Start Address
   
  // ---------- Display Setup 3 ---------------------
   
  send_cmd(S1D15G14_PIXEL_FORMAT);      // Interface pixel format
  // 0b 0000 0010    
  send_data(0x02); // 8 bit mode B22,B21,B20=010
  
  send_cmd(S1D15G14_COLOUR_SET);      // COLOUR SET  (RGBSET)
  send_data(0); // RED
  send_data(3);
  send_data(5);
  send_data(7);
  send_data(9);
  send_data(11);
  send_data(13);
  send_data(15);

  send_data(0);  // GREEN
  send_data(3);
  send_data(5);
  send_data(7);
  send_data(9);
  send_data(11);
  send_data(13);
  send_data(15);

  send_data(0);  // BLUE
  send_data(3);
  send_data(9);
  send_data(15);

  send_cmd(S1D15G14_MEMORY_ACCESS_CTRL);           // Memory access control
  // 0b 0010 0000
  // B7 B6 B5 B4 | B3 0 0 B0
  // 0  0  1  0    0  0 0 0    - B5=1 Inverting Mode
  send_data(0x20);   // B4-TOP-BUTTOM, B6-LEFTRIGHT, B5-INVERT MODE (From top to bottom), B3-RGB,
  //send(0b00000000, DATA);

  // Page Address Set
  
  // Column Address Set

  send_cmd(S1D15G14_DISPLAY_ON);           // Display On
  
  clear_screen(bgcolor);

}


void PrintByte(unsigned char byte)    // 8 pixels
{
    char caa;
   
   for (caa=8;caa>0;caa--)
   {

      if ((byte&0x01)==0)
        {
          send_data(bgcolor);
        }
      else 
        {
          send_data(fgcolor);
        }

      byte=byte>>1;
   }

}


////////////////////////////////////////////////////////////////////////////////
// Wrote in the character LCD past
////////////////////////////////////////////////////////////////////////////////

void nokia_printchar(unsigned char caracter)
{
  unsigned char char_row,charpos,chardata;

  for (char_row=0;char_row<5;char_row++)
   {           // They are 5 Bytes per character
                                                      // So this loop
      if (caracter<0x50)
         {
           charpos=(((caracter&0xFF)-0x20)*5)+char_row;
           chardata=ASCII_TABLE1[charpos];
         }
      else
         {
           charpos=(((caracter&0xFF)-0x50)*5)+char_row;
           chardata=ASCII_TABLE2[charpos];
         }

      PrintByte(chardata);                    // Send character

   }

   PrintByte(0x00);                           // We send space White 1

}


void nokia_outs(char * s, unsigned char x, unsigned char y)
{

   send_cmd(S1D15G14_PAGE_ADDRESS_SET);      // page address set
   send_data(y);
   send_data(y+7); // Количество строк - 1

   send_cmd(S1D15G14_COLUMN_ADDRESS_SET);      // column address set
   send_data(x);
   send_data(x+96); // КОличество пикселей в строке

   send_cmd(S1D15G14_MEMORY_WRITE);      // Memory Write

   while(*s)
   nokia_printchar(*s++);
}
