#include "1wire.h"

static unsigned char OW_LastDevice = 0;
static unsigned char OW_LastDiscrepancy = 0;
static unsigned char OW_LastFamilyDiscrepancy = 0;

void OWdelay(void) 
{
    DELAY_I;
}

unsigned char __flash dscrc_table[] = { 
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133,103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140,210, 48, 110, 237, 179, 81, 15, 78, 16, 242,  172, 47, 113,147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211,141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82,176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

void OWCRC(unsigned char x, unsigned char *crc) 
{
    *crc = dscrc_table[(*crc) ^ x];
}

//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 0 if presence pulse was found,
// return 1 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)

unsigned char OWTouchReset(void) 
{

    OWIREOUTPORT &= ~(1<<OWIREOUTPIN);
    OWIREDDR |= (1<<OWIREOUTPIN);
    DELAY_H;
    OWIREDDR &= ~(1<<OWIREOUTPIN);
    DELAY_I;
    if (OWIREINPORT & (1<<OWIREINPIN))
      {
        return (1);
      }
    else
      {
        return (0);
      }
    
    
//    return(OWIREINPORT & (1<<OWIREINPIN) ? 1 : 0);
}

//-----------------------------------------------------------------------------
// Send a 1-wire write bit.

void OWWriteBit(unsigned char bit) 
{
    OWdelay();
    if (bit) 
    {
	OWIREOUTPORT &= ~(1<<OWIREOUTPIN);
	OWIREDDR |= (1<<OWIREOUTPIN);
	DELAY_A;
	OWIREDDR &= ~(1<<OWIREOUTPIN);
	DELAY_B;
    } 
    else 
    {
	OWIREOUTPORT &= ~(1<<OWIREOUTPIN);
	OWIREDDR |= (1<<OWIREOUTPIN);
	DELAY_C;
	OWIREDDR &= ~(1<<OWIREOUTPIN);
	DELAY_D;
    }
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-wire bus and return it.

unsigned char OWReadBit (void) 
{
    OWdelay();

    OWIREOUTPORT &= ~(1<<OWIREOUTPIN);
    OWIREDDR |= (1<<OWIREOUTPIN);
    DELAY_A;
    OWIREDDR &= ~(1<<OWIREOUTPIN);
    DELAY_E;
    if (OWIREINPORT & (1<<OWIREINPIN))
      {
        return (1);
      }
    else
      {
        return (0);
      }
}

//-----------------------------------------------------------------------------
// Write a byte to the 1-wire bus

void OWWriteByte(unsigned char data) 
{
    unsigned char i;
    
    // Send LSB first
    for (i = 0; i < 8; i++) 
    {
	OWWriteBit(data & 0x01);
	data >>= 1;
    }
}

//-----------------------------------------------------------------------------
// Read a byte from the 1-wire bus

unsigned char OWReadByte(void) 
{
    int i, result = 0;
    
    for (i = 0; i < 8; i++) 
    {
	result >>= 1;
	if (OWReadBit())
        {
	    result |= 0x80;
        }
    }
    return(result);
}

//-----------------------------------------------------------------------------
// Write a 1-wire data byte and return the sampled result.

unsigned char OWTouchByte(unsigned char data) 
{
    unsigned char i, result = 0;
    
    for (i = 0; i < 8; i++) 
    {
	result >>= 1;
	
	// If sending a 1 then read a bit, otherwise write a 0
	if (data & 0x01) 
        {
	    if (OWReadBit())
            {
		result |= 0x80;
            }
	} 
        else
        {
	    OWWriteBit(0);
        }
	
	data >>= 1;
    }

    return(result);
}

//-----------------------------------------------------------------------------
// Write a block of bytes to the 1-wire bus and return the sampled result in
// the same buffer

void OWBlock(unsigned char *data, int len) 
{
    int i;
    
    for (i = 0; i < len; i++)
    {
	data[i] = OWTouchByte(data[i]);
    }
}


//-----------------------------------------------------------------------------
// Send a 1 wire command to a device, or all if no ROM ID provided

void OWSendCmd(unsigned char *ROM, unsigned char cmd) 
{
    unsigned char i;

    OWTouchReset();
    
    if (ROM == NULL)
      {
  	OWWriteByte(OW_SKIP_ROM_CMD);
      }
    else 
      {
	OWWriteByte(OW_MATCH_ROM_CMD);
	for (i = 0; i < 8; i++)
          {
	    OWWriteByte(ROM[i]);
          }
      }

    OWWriteByte(cmd);
}


//-------------------------------------------------------
// Returns 1 when something is found, 0 if nothing left

unsigned char OWNext(unsigned char *ROM, unsigned char do_reset, unsigned char alarm_only) 
{
    unsigned char	bit_test, search_direction, bit_number;
    unsigned char	last_zero, rom_byte_number, rom_byte_mask;
    unsigned char	lastcrc8, crcaccum;
    unsigned char	next_result;

   // Init for search
   bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   next_result = OW_NOMODULES;
   lastcrc8 = 0;
   crcaccum = 0;

   // if the last call was not the last one
   if (!OW_LastDevice) 
   {
       // check if reset first is requested
       if (do_reset) 
       {
	   // reset the 1-wire
	   // if there are no parts on 1-wire, return 0
         if (OWTouchReset()) 
         {
	     // reset the search
	     OW_LastDiscrepancy = 0;
	     OW_LastFamilyDiscrepancy = 0;
	     return OW_NOPRESENCE;
         }
       }

       // If finding alarming devices issue a different command
      if (alarm_only)
      {
	  OWWriteByte(OW_SEARCH_ALRM_CMD);	// issue the alarming search command
      }
      else
      {
	  OWWriteByte(OW_SEARCH_ROM_CMD);	// issue the search command
      }

      // pause before beginning the search
      OWdelay();
      OWdelay();
      OWdelay();
      
      // loop to do the search
      do 
      {
	  // read a bit and its compliment
	  bit_test = OWReadBit() << 1;
	  bit_test |= OWReadBit();


	  // check for no devices on 1-wire
	  if (bit_test == 3) 
            {
	      return(OW_BADWIRE);
            }
	  else 
          {
	      // all devices coupled have 0 or 1
	      if (bit_test > 0)
                {
		  search_direction = !(bit_test & 0x01);  // bit write value for search
                }
	      else 
              {
		  // if this discrepancy is before the Last Discrepancy
		  // on a previous OWNext then pick the same as last time
		  if (bit_number < OW_LastDiscrepancy)
                    {
		      search_direction = ((ROM[rom_byte_number] & rom_byte_mask) > 0);
                    }
		  else
		      // if equal to last pick 1, if not then pick 0
                    {
			 search_direction = (bit_number == OW_LastDiscrepancy);
                    }

		  // if 0 was picked then record its position in LastZero
		  if (search_direction == 0) 
                  {
		      last_zero = bit_number;

		      // check for Last discrepancy in family
		      if (last_zero < 9)
                        {
			  OW_LastFamilyDiscrepancy = last_zero;
                        }
		  }
	      }

	      // set or clear the bit in the ROM byte rom_byte_number
	      // with mask rom_byte_mask
	      if (search_direction == 1)
                {
		  ROM[rom_byte_number] |= rom_byte_mask;
                }
	      else
                {
		  ROM[rom_byte_number] &= ~rom_byte_mask;
                }

	      // serial number search direction write bit
	      OWWriteBit(search_direction);

	      // increment the byte counter bit_number
	      // and shift the mask rom_byte_mask
	      bit_number++;
	      rom_byte_mask <<= 1;

	      // if the mask is 0 then go to new ROM byte rom_byte_number
	      // and reset mask
	      if (rom_byte_mask == 0) 
                {
		  OWCRC(ROM[rom_byte_number], &crcaccum);  // accumulate the CRC
		  lastcrc8 = crcaccum;
		  
		  rom_byte_number++;
		  rom_byte_mask = 1;
    	        }
	  }
      } while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!(bit_number < 65) || lastcrc8) 
      {
	  if (lastcrc8) 
            {
	      next_result = OW_BADCRC;
            }
          else 
            {
	      //  search successful so set LastDiscrepancy,LastDevice,next_result
	      OW_LastDiscrepancy = last_zero;
	      OW_LastDevice = (OW_LastDiscrepancy == 0);
	      next_result = OW_FOUND;
            }
      }
   }

   // if no device found then reset counters so next 'next' will be
   // like a first
   if (next_result != OW_FOUND || ROM[0] == 0) 
   {
       OW_LastDiscrepancy = 0;
       OW_LastDevice = 0;
       OW_LastFamilyDiscrepancy = 0;
   }

   if (next_result == OW_FOUND && ROM[0] == 0x00)
   {
       next_result = OW_BADWIRE;
   }
   
   return next_result;

}


//-----------------------------------------------------------------------------
// Search algorithm from App note 187 (and 162)
//
// OWFirst/OWNext return..
//         1 when something is found,
//         0 no more modules
//	   FF if no presence pulse,
//         FE if bad CRC,
//         FD if bad wiring.

unsigned char OWFirst(unsigned char *ROM, unsigned char do_reset, unsigned char alarm_only) 
{
    // Reset state
    OW_LastDiscrepancy = 0;
    OW_LastDevice = 0;
    OW_LastFamilyDiscrepancy = 0;
    
    // Go looking
    return (OWNext(ROM, do_reset, alarm_only));
}
