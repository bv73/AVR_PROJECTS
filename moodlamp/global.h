/*************************************************************************************
*
*  Глобальные переменные, назначения ножек и общие функции
*
*  Совместимо с Strict ANSI C, MISRA-C 2004
*
*  by:      BSVi
*  version: 1.00
*
*************************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "utils/useful.h"

#define F_CPU 14.31818

#define	_delay_us(us)	__delay_cycles (F_CPU * us);
#define _delay_ms(ms)	_delay_us (1000.0f * ms)


/*----------------------------------------------------------------------------------*/
/* Назначения ножек                                                                 */
/*----------------------------------------------------------------------------------*/

// PORTB
#define ENABLE2_PB   BIT1
#define ENABLE1_PB   BIT2

// PORTC
#define GAIN10_PC    BIT1
#define GAIN2_PC     BIT2
#define GAIN100_PC   BIT3
#define MIC_PC       BIT4


// PORTD
#define ENABLE3_PD   BIT3


#endif
