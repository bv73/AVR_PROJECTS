#ifndef DELAY_H
#define DELAY_H

#include <inavr.h>

#define CPU_CLK_Hz 8000000
//#define CPU_CLK_kHz (unsigned long)(CPU_CLK_Hz/1000)

#define delay_us(x) __delay_cycles(x*(CPU_CLK_Hz/1000000))
#define delay_ms(x) __delay_cycles(x*(CPU_CLK_Hz/1000))


#endif
