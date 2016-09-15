// MPL3115A2 Routines v1.0 By (R)soft 15.12.2013

//--------------- Регистры MPL3115A2
#define STATUS     0x00
#define OUT_P_MSB  0x01
#define OUT_P_CSB  0x02
#define OUT_P_LSB  0x03
#define OUT_T_MSB  0x04
#define OUT_T_LSB  0x05
#define DR_STATUS  0x06
#define OUT_P_DELTA_MSB  0x07
#define OUT_P_DELTA_CSB  0x08
#define OUT_P_DELTA_LSB  0x09
#define OUT_T_DELTA_MSB  0x0A
#define OUT_T_DELTA_LSB  0x0B
#define WHO_AM_I   0x0C
#define F_STATUS   0x0D
#define F_DATA     0x0E
#define F_SETUP    0x0F
#define TIME_DLY   0x10
#define SYSMOD     0x11
#define INT_SOURCE 0x12
#define PT_DATA_CFG 0x13
#define BAR_IN_MSB 0x14
#define BAR_IN_LSB 0x15
#define P_TGT_MSB  0x16
#define P_TGT_LSB  0x17
#define T_TGT      0x18
#define P_WND_MSB  0x19
#define P_WND_LSB  0x1A
#define T_WND      0x1B
#define P_MIN_MSB  0x1C
#define P_MIN_CSB  0x1D
#define P_MIN_LSB  0x1E
#define T_MIN_MSB  0x1F
#define T_MIN_LSB  0x20
#define P_MAX_MSB  0x21
#define P_MAX_CSB  0x22
#define P_MAX_LSB  0x23
#define T_MAX_MSB  0x24
#define T_MAX_LSB  0x25
#define CTRL_REG1  0x26
#define CTRL_REG2  0x27
#define CTRL_REG3  0x28
#define CTRL_REG4  0x29
#define CTRL_REG5  0x2A
#define OFF_P      0x2B
#define OFF_T      0x2C
#define OFF_H      0x2D

#define MPL3115A2_ADDRESS 0xC0 // 7-bit I2C address

// Переменные режима
#define PRESSURE  0xFF // For Mode Select
#define ALTIMETER 0x00

//--------------------------------------------------------------------
// Чтение регистра из датчика

unsigned char mpl3115a2_read(unsigned char reg)
{                                                       
   unsigned char byte=0;
   i2c_start();
   i2c_byte_write(MPL3115A2_ADDRESS);
   i2c_byte_write(reg);
   i2c_start();
   i2c_byte_write(MPL3115A2_ADDRESS+1);
   byte=i2c_byte_read();
   i2c_nack();
   i2c_stop();
   return (byte);
}

//--------------------------------------------------------------------
// Запись регистра

void mpl3115a2_write(unsigned char reg, unsigned char value)
{                                                       
   i2c_start();
   i2c_byte_write(MPL3115A2_ADDRESS);
   i2c_byte_write(reg);
   i2c_byte_write(value);
   i2c_stop();
}

//---------------------------------------------------------------------------
//Clears then sets the OST bit which causes the sensor to immediately take another reading
//Needed to sample faster than 1Hz
void toggleOneShot(void)
{
  unsigned char tmp = mpl3115a2_read(CTRL_REG1); //Read current settings
  tmp &= ~(1<<1); //Clear OST bit
  mpl3115a2_write(CTRL_REG1, tmp);

  tmp = mpl3115a2_read(CTRL_REG1); //Read current settings to be safe
  tmp |= (1<<1); //Set OST bit
  mpl3115a2_write(CTRL_REG1, tmp);
}

//--------------------------------------------------
//Returns the number of meters above sea level
float readAltitude()
{
  toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

  //Wait for PDR bit, indicates we have new pressure data
  int counter = 0;
  while( (mpl3115a2_read(STATUS) & (1<<1)) == 0)
  {
      if(++counter > 1000) return(0); //Error out=0
      delay_ms(1);
  }
  
  // Read pressure registers
  i2c_start();
  i2c_byte_write(MPL3115A2_ADDRESS);
  i2c_byte_write(OUT_P_MSB);  // Address of data to get
  i2c_start(); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
  i2c_byte_write(MPL3115A2_ADDRESS+1); // Request three bytes

  unsigned char msb, csb, lsb;
  msb = i2c_byte_read();
  i2c_ack();
  csb = i2c_byte_read();
  i2c_ack();  
  lsb = i2c_byte_read();
  i2c_nack();
  i2c_stop(); // добавлено позже, для завершения транзакции i2c

  toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

  // The least significant bytes l_altitude and l_temp are 4-bit,
  // fractional values, so you must cast the calulation in (float),
  // shift the value over 4 spots to the right and divide by 16 (since 
  // there are 16 values in 4-bits). 
  float tempcsb = (lsb>>4)/16.0;

  float altitude = (float)( (msb << 8) | csb) + tempcsb;

  return(altitude);
}

/*
//-------------------------------------------------------------
//Returns the number of feet above sea level
float readAltitudeFt()
{
  return(readAltitude() * 3.28084);
}
*/

//-------------------------------------------------------------
//Reads the current pressure in Pa
//Unit must be set in barometric pressure mode

float readPressure()
{
  toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

  //Wait for PDR bit, indicates we have new pressure data
  int counter = 0;
  while( (mpl3115a2_read(STATUS) & (1<<2)) == 0)
  {
      if(++counter > 1000) return(0); //Error out=0
      delay_ms(1);
  }

  // Read pressure registers
  i2c_start();
  i2c_byte_write(MPL3115A2_ADDRESS);
  i2c_byte_write(OUT_P_MSB);  // Address of data to get
  i2c_start(); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
  i2c_byte_write(MPL3115A2_ADDRESS+1); // Request three bytes

  unsigned char msb, csb, lsb;
  msb = i2c_byte_read();
  i2c_ack();
  csb = i2c_byte_read();
  i2c_ack();
  lsb = i2c_byte_read();
  i2c_nack();
  i2c_stop(); // добавлено позже
  
  toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

  // Pressure comes back as a left shifted 20 bit number
  long pressure_whole = (long)msb<<16 | (long)csb<<8 | (long)lsb;
  pressure_whole >>= 6; //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.

  lsb &= 0x30; //Bits 5/4 represent the fractional component
  lsb >>= 4; //Get it right aligned
  float pressure_decimal = (float)lsb/4.0; //Turn it into fraction

  float pressure = (float)pressure_whole + pressure_decimal;

  return(pressure);
}

//-----------------------------------------------
// Read Temperature in Celsius

float readTemp()
{
  toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

  //Wait for TDR bit, indicates we have new temp data
  int counter = 0;
  while( (mpl3115a2_read(STATUS) & (1<<1)) == 0)
  {
      if(++counter > 1000) return(0); //Error out=0
      delay_ms(1);
  }
  
  // Read temperature registers
  i2c_start();
  i2c_byte_write(MPL3115A2_ADDRESS);
  i2c_byte_write(OUT_T_MSB);  // Address of data to get
  i2c_start(); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
  i2c_byte_write(MPL3115A2_ADDRESS+1); // Request two bytes

  unsigned char msb, lsb;
  msb = i2c_byte_read();
  i2c_ack();
  lsb = i2c_byte_read();
  i2c_nack();
  i2c_stop(); // добавлено позже
  
  // The least significant bytes l_altitude and l_temp are 4-bit,
  // fractional values, so you must cast the calulation in (float),
  // shift the value over 4 spots to the right and divide by 16 (since 
  // there are 16 values in 4-bits). 
  float templsb = (lsb>>4)/16.0; //temp, fraction of a degree

  float temperature = (float)(msb + templsb);

  return(temperature);
}

/*
//-----------------------------------------------------------------------
//Give me temperature in fahrenheit!
float readTempF()
{
  return((readTemp() * 9.0)/ 5.0 + 32.0); // Convert celsius to fahrenheit
}
*/

//-----------------------------------------------------------------------
//Sets the mode to Barometer
//CTRL_REG1, ALT bit
void setModeBarometer()
{
  unsigned char tmp = mpl3115a2_read(CTRL_REG1); //Read current settings
  tmp &= ~(1<<7); //Clear ALT bit
  mpl3115a2_write(CTRL_REG1, tmp);
}

//-----------------------------------------------------------------
//Sets the mode to Altimeter
//CTRL_REG1, ALT bit
void setModeAltimeter()
{
  unsigned char tmp = mpl3115a2_read(CTRL_REG1); //Read current settings
  tmp |= (1<<7); //Set ALT bit
  mpl3115a2_write(CTRL_REG1, tmp);
}

//----------------------------------------------------------------------
//Puts the sensor in standby mode
//This is needed so that we can modify the major control registers
void setModeStandby()
{
  unsigned char tmp = mpl3115a2_read(CTRL_REG1); //Read current settings
  tmp &= ~(1<<0); //Clear SBYB bit for Standby mode
  mpl3115a2_write(CTRL_REG1, tmp);
}

//------------------------------------------------------------------------
//Puts the sensor in active mode
//This is needed so that we can modify the major control registers
void setModeActive()
{
  unsigned char tmp = mpl3115a2_read(CTRL_REG1); //Read current settings
  tmp |= (1<<0); //Set SBYB bit for Active mode
  mpl3115a2_write(CTRL_REG1, tmp);
}

//--------------------------------------------------------------------
//Setup FIFO mode to one of three modes. See page 26, table 31
//From user jr4284
void setFIFOMode(unsigned char f_Mode)
{
  if (f_Mode > 3) f_Mode = 3; // FIFO value cannot exceed 3.
  f_Mode <<= 6; // Shift FIFO byte left 6 to put it in bits 6, 7.

  unsigned char tmp = mpl3115a2_read(F_SETUP); //Read current settings
  tmp &= ~(3<<6); // clear bits 6, 7
  tmp |= f_Mode; //Mask in new FIFO bits
  mpl3115a2_write(F_SETUP, tmp);
}

//---------------------------------------------------------------------------
//Call with a rate from 0 to 7. See page 33 for table of ratios.
//Sets the over sample rate. Datasheet calls for 128 but you can set it 
//from 1 to 128 samples. The higher the oversample rate the greater
//the time between data samples.
// 7 - maximum resolution, 0 - maximum speed
void setOversampleRate(unsigned char sampleRate)
{
  if(sampleRate > 7) sampleRate = 7; //OS cannot be larger than 0b.0111
  sampleRate <<= 3; //Align it for the CTRL_REG1 register

  unsigned char tmp = mpl3115a2_read(CTRL_REG1); //Read current settings
  tmp &= 0xC7; //Clear out old OS bits
  tmp |= sampleRate; //Mask in new OS bits
  mpl3115a2_write(CTRL_REG1, tmp);
}


//------------------------------------------------------------------------------
//Enables the pressure and temp measurement event flags so that we can
//test against them. This is recommended in datasheet during setup.
void enableEventFlags(void)
{
  mpl3115a2_write(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags 
}

/*

void mpl3115a2_setup()
{
  i2c_init();        // join i2c bus
//  Serial.begin(57600);  // start serial for output

//  if(IIC_Read(WHO_AM_I) == 196) 
//    Serial.println("MPL3115A2 online!");
//  else
//    Serial.println("No response - check connections");

  // Configure the sensor
//  setModeAltimeter(); // Measure altitude above sea level in meters
  setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa

  setOversampleRate(7); // Set Oversample to the recommended 128
  enableEventFlags(); // Enable all three pressure and temp event flags
}

*/


/*
void loop()
{
//  startTime = millis();

  float altitude = readAltitude();
//  Serial.print("Altitude(m):");
//  Serial.print(altitude, 2);

  //altitude = readAltitudeFt();
  //Serial.print(" Altitude(ft):");
  //Serial.print(altitude, 2);

//  float pressure = readPressure();
//   Serial.print(" Pressure(Pa):");
//   Serial.println(pressure, 2);

  //float temperature = readTemp();
  //Serial.print(" Temp(c):");
  //Serial.print(temperature, 2);

  //float temperature = readTempF();
  //Serial.print(" Temp(f):");
  //Serial.print(temperature, 2);
  
//  Serial.print(" time diff:");
//  Serial.print(millis() - startTime);

//  Serial.println();

  //delay(1);
}
*/



