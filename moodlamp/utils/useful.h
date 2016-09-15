/*************************************************************************************
*
*  Разнообразные полезные функции
*  Определения итд
*
*  ANSI C, MISRA-C 2004
*
*  by:      BSVi
*  version: 1.2
*
*************************************************************************************/

#ifndef _BSVI_INCLUDE_
#define _BSVI_INCLUDE_

/*----------------------------------------------------------------------------------*/
/* Настройки                                                                        */
/*----------------------------------------------------------------------------------*/

/* Архитектура */
#define AVR

/*----------------------------------------------------------------------------------*/
/* Разные инклуды                                                                   */
/*----------------------------------------------------------------------------------*/
#ifdef AVR
   #include <intrinsics.h>
   #include <ioavr.h>

   #define _DINT() __disable_interrupt()
   #define _EINT() __enable_interrupt()
   #define nop() __no_operation()
   #define wdr() __watchdog_reset()

   #define ENTER_CRITICAL_SECTION() uint8 __intState__ = __save_interrupt();  __disable_interrupt();
   #define LEAVE_CRITICAL_SECTION() __restore_interrupt( __intState__ );

   class CCriticalSection
   {
      unsigned char __intState__;
     public:
      inline CCriticalSection() { __intState__ = __save_interrupt();  __disable_interrupt(); }
      inline ~CCriticalSection() { __restore_interrupt( __intState__ ); }
   };
#endif /* AVR */


/*----------------------------------------------------------------------------------*/
/* Разные полезные типы данных                                                      */
/*----------------------------------------------------------------------------------*/

typedef unsigned char	      uint8;
typedef unsigned short        uint16;
typedef unsigned long         uint32;
/* typedef unsigned long long    uint64; */  /* Не совместимо с ANSI-C, но может пригодиться */

typedef signed char	      int8;
typedef signed short	      int16;
typedef signed long	      int32;
/* typedef signed long long      int64; */   /* Не совместимо с ANSI-C, но может пригодиться */

typedef unsigned int          uint;

/*----------------------------------------------------------------------------------*/
/* Разные константы                                                                 */
/*----------------------------------------------------------------------------------*/

#define uint32_MAXMAX 0xffffffff
#define uint16_MAXMAX 0xffff
#define uint8_MAXMAX  0xff

/* Коды ошибок */
typedef enum
{
   S_OK = 0,
   E_ERR = 1,
   E_USER = 2,
   E_NOSPACE = 3,
   E_IO = 4,
   E_COMM = 5
} Result;


/*----------------------------------------------------------------------------------*/
/* Полезные функции                                                                 */
/*----------------------------------------------------------------------------------*/

#define _BV(n) (1 << (n))

#define MIN(n,m) (((n) < (m)) ? (n) : (m))
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) (((n) < 0) ? -(n) : (n))

#define HIBYTE(a) (*((uint8*)&(a) + 1))
#define LOBYTE(a) (*((uint8*)&(a)))
#define HIWORD(a) (*((uint16*)&(a) + 1))
#define LOWORD(a) (*((uint16*)&(a)))

#define BYTE0(a) (*((uint8*)&(a) + 0))
#define BYTE1(a) (*((uint8*)&(a) + 1))
#define BYTE2(a) (*((uint8*)&(a) + 2))
#define BYTE3(a) (*((uint8*)&(a) + 3))
#define ARRAY_SIZE(a) ( sizeof(a)/sizeof(*(a)) )


/*----------------------------------------------------------------------------------*/
/* Биты                                                                             */
/*----------------------------------------------------------------------------------*/

#ifndef BIT0
#define BIT0                (1<<0)
#define BIT1                (1<<1)
#define BIT2                (1<<2)
#define BIT3                (1<<3)
#define BIT4                (1<<4)
#define BIT5                (1<<5)
#define BIT6                (1<<6)
#define BIT7                (1<<7)
#define BIT8                (1<<8)
#define BIT9                (1<<9)
#define BIT10               (1<<10)
#define BIT11               (1<<11)
#define BIT12               (1<<12)
#define BIT13               (1<<13)
#define BIT14               (1<<14)
#define BIT15               (1<<15)
#define BIT16               (1<<16)
#define BIT17               (1<<17)
#define BIT18               (1<<18)
#define BIT19               (1<<19)
#define BIT20               (1<<20)
#define BIT21               (1<<21)
#define BIT22               (1<<22)
#define BIT23               (1<<23)
#define BIT24               (1<<24)
#define BIT25               (1<<25)
#define BIT26               (1<<26)
#define BIT27               (1<<27)
#define BIT28               (1<<28)
#define BIT29               (1<<29)
#define BIT30               (1<<30)
#define BIT31               (1<<31)
#endif



/* Позволяет проверить размерность типов данных с новым компилятором, не совместимо  с ANSI-C */
/*
   #ifndef NO_ASSERTS
   #ifndef ASSERT_HANDLER
   #define ASSERT_HANDLER()       while(1)
   #endif

   #define ASSERT(expr)          st( if (!(expr)) BSP_ASSERT_HANDLER();)
   #define FORCE_ASSERT()        BSP_ASSERT_HANDLER()
   #else
   #define BSP_ASSERT(expr)
   #define BSP_FORCE_ASSERT()
   #endif

   #define STATIC_ASSERT(expr)   void DummyPrototype( char dummy[1/((expr)!=0)] );


   STATIC_ASSERT( sizeof(  uint8 ) == 1 );
   STATIC_ASSERT( sizeof(   int8 ) == 1 );
   STATIC_ASSERT( sizeof( uint16 ) == 2 );
   STATIC_ASSERT( sizeof(  int16 ) == 2 );
   STATIC_ASSERT( sizeof( uint32 ) == 4 );
   STATIC_ASSERT( sizeof(  int32 ) == 4 );
   STATIC_ASSERT( sizeof( uint64 ) == 8 );
   STATIC_ASSERT( sizeof(  int64 ) == 8 );
*/

#endif	/* _BSVI_INCLUDE_ */
