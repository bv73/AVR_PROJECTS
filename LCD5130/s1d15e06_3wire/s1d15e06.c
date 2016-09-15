
void send_cmd(unsigned char ch) 
{
  unsigned char i;

//  XCS_0;
  
  SI_0; // Признак команды A0=0
  delay_ms(1);
  SCL_1;
  delay_ms(1);
  SCL_0;
  delay_ms(1);

  for (i=8;i;i--)
    {
      if (ch&0x80)
        {
          SI_1;
        }
      else
        {
          SI_0;
        }

      delay_ms(1);
      SCL_1;
      delay_ms(1);
      SCL_0;
      delay_ms(1);

      ch<<=1;
     }

//  XCS_1;
}

void send_data(unsigned char ch)
{
  unsigned char i;

//  XCS_0;

  SI_1;  // Признак данных A0=1
  delay_ms(1);
  SCL_1;
  delay_ms(1);
  SCL_0;
  delay_ms(1);
  
  for (i=8;i;i--)
    {
      if (ch&0x80)
        {
          SI_1;
        }
      else
        {
          SI_0;
        }
      delay_ms(1);
      SCL_1;
      delay_ms(1);
      SCL_0;
      delay_ms(1);
      
      ch<<=1;
     }

//  XCS_1;
}


//=========================================================================================
void s1d15e06_init(void)
{
       
  PIN_SCL=0;
  PIN_SI=0;
  PIN_XCS=0;
  PIN_XRES=0;
  XCS_0;
  SCL_0;

  delay_ms(250);

  // LCD Hardware Reset
  XRES_0;
  delay_ms(1);
  XRES_1;
  delay_ms(10);
    
  send_cmd(S1D15E06_SOFTWARE_RESET);      // LCD Software Reset
  delay_ms(100);

  send_cmd(S1D15E06_COLUMN_ADDR_NORMAL);      // (12)
  
  send_cmd(S1D15E06_COMM_SLCT_NORMAL);      // (5)  
  
  send_cmd(S1D15E06_DISPLAY_NORMAL);      // (3)
  
  send_cmd(S1D15E06_ALL_PILEL_ON);      // (4)
  
  send_cmd(S1D15E06_DUTY_SET);      // (18)
  send_data(25);
  send_data(0x00);  
  
  send_cmd(S1D15E06_DISPLAY_OFF_MODE0);      // (2)
  
  send_cmd(S1D15E06_VOLTAGE_SELECT);      // (27)
  send_data(0x03); // 0,2,3 (0x00 by Reset)
  
  send_cmd(S1D15E06_ELECTRONIC_VOLUME);      // (28)
  send_data(63); // 0...127
  
//  send_cmd(S1D15E06_TEMP_GRADIENT_SET);      // (31)
//  send_data(0x00);

  send_cmd(S1D15E06_OSCILLATOR_FREQ_SLCT);      // (24)
  send_data(0x01); // 100 kHz

  send_cmd(S1D15E06_OSCILLATOR_ON);      // (23)

  send_cmd(S1D15E06_POWER_CONTROL_SET);      // (25)
  send_data(0x1F); // 0x1F - Triple step-up; 0x0F - Vout=Vdd; 0x17 - Double step-up

  send_cmd(S1D15E06_STEPUP_FREQ_SLCT); // (26)
  send_data(0x33); // Step-Up fosc/8 & Step-Down fosc/8


//  send_cmd(S1D15E06_DISPLAY_MODE);
//  send_data(0x00); // 0 - 4Grayscale; 1 - Binary Mode

  
  send_cmd(S1D15E06_DISPLAY_ON);           // Display On

}

