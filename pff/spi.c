//#define SPI_SD             SPCR = 0x52
//#define SPI_HIGH_SPEED     SPCR = 0x50; SPSR |= (1<<SPI2X)

//SPI initialize for SD card
//clock rate: 125Khz

void init_spi(void)
{
  SPCR = 0x52; //setup SPI: Master mode, MSB first, SCK phase low, SCK idle low
  SPSR = 0x00;
}

unsigned char xmit_spi(unsigned char data)
{

  SPDR = data;
  while(!(SPSR & 0x80));
  data = SPDR;

  return(data);
}

unsigned char rcv_spi(void)
{
  unsigned char data;

  SPDR = 0xff;
  while(!(SPSR & 0x80));
  data = SPDR;
  
  return data;
}


void port_init(void)
{
  PORTB = 0xFF; // 1111 1111    
  DDRB  = 0xF7; // 1111 0111    MISO на ввод, остальные на вывод
}

//--------------------------------------------
// Initialize all peripherals
void init_devices(void)
{
 __disable_interrupt();
 port_init();
 init_spi();

 MCUCR = 0x00;
 EICRA = 0x00;
 EICRB = 0x00;
 TIMSK0 = 0x00; //timer interrupt sources
}
