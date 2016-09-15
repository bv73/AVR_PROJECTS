#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

typedef unsigned char u08;
typedef signed char s08;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned long u32;
typedef signed long s32;
typedef unsigned long long u64;

typedef void (*voidFuncPtr)(void);
typedef void (*voidFunc_char_Ptr)(char);


//u16 load_progmem_u16(const unsigned short __farflash *ptr);
//size_t strlenEx_P(const char __farflash *ptr, size_t max);
void delay_us(unsigned short time_us);


#ifndef cli
	#define cli()			asm("cli\n")
#endif

#ifndef sei
	#define sei()			asm("sei\n")
#endif


#define MBUS_DEVADDR  1u

#define F_CPU 8000000u
#define FCPU  F_CPU
#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond

#define ALLBITS(oftype, bits) (((oftype)0U-1)>>(sizeof(oftype)*8-(bits)))

#ifndef outb
	#define	outb(addr, data)	addr = (data)
#endif

#ifndef inb
	#define	inb(addr)			(addr)
#endif

#ifndef BV
	#define BV(bit)			(1U<<(bit))
#endif

#ifndef BVL
	#define BVL(bit)		(1LU<<(bit))
#endif

#ifndef cbi
	#define cbi(reg,bit)	(reg)&=~BVL((bit))
#endif

#ifndef sbi
	#define sbi(reg,bit)	(reg)|=BVL((bit))
#endif

#define isBit(reg, bit) (((reg)&BV((bit)))!=0)

#define LENDIANWORD(value)	((((value)&0xFFFF)>>8)|(((value)&0xFF)<<8))

//#define delay		delay_us


#ifndef __PI
      #define __PI            3.141592653589793238462643
#endif

