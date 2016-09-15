#define OWIREOUTPORT		PORTD
#define OWIREOUTPIN		PD7

#define OWIREINPORT		PIND
#define OWIREDDR		DDRD
#define OWIREINPIN		PD7

// Return codes for OWFirst()/OWNext()
#define OW_BADWIRE		0xFD  //-3
#define OW_BADCRC		0xFE  //-2
#define OW_NOPRESENCE		0xFF  // -1
#define OW_NOMODULES		0
#define OW_FOUND		1

// General 1 wire commands
#define OW_OVRDRV_SKIP_CMD	0x3C
#define OW_SEARCH_ALRM_CMD	0xEC
#define OW_SEARCH_ROM_CMD	0xF0
#define OW_READ_ROM_CMD		0x33
#define OW_MATCH_ROM_CMD	0x55
#define OW_SKIP_ROM_CMD		0xCC

// DS1820 commands
#define OW_CONVERTT_CMD		0x44
#define OW_RD_SCR_CMD		0xBE
#define OW_WR_SCR_CMD		0x4E
#define OW_CPY_SCR_CMD		0x48
#define OW_RECALL_CMD		0xB8
#define OW_RD_PSU_CMD		0xB4

// Family codes
#define OW_FAMILY_ROM		0x09
#define OW_FAMILY_TEMP		0x10

/* 6 usec */
#define DELAY_A	delay_us(6)

/* 64 usec */
#define DELAY_B delay_us(64)

/* 60 usec */
#define DELAY_C delay_us(60)

/* 10 usec */
#define DELAY_D delay_us(10)

/* 9 usec */
#define DELAY_E delay_us(9)

/* 55 usec */
#define DELAY_F delay_us(55)

/* 480 usec */
#define DELAY_H delay_us(480)

/* 70 usec */    
#define DELAY_I delay_us(70)


