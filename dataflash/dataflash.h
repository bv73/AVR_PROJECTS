// DataFlash.h

// By (R)soft 26.5.2011
// DataFlash AT45DB321 (32 Mbit, 528 Bytes per 8192 Pages)

//#include "ina90.h"
//#pragma language=extended

// Параметры DataFlash
#define bytes_per_page 528
#define dataflash_pages 8192

// DataFlash вывод состояния порта готов/занят (PB 4)
#define DF_RDY_BUSY 0x10

// DataFlash вывод порта выбора микросхемы (PB 5)
#define DF_CHIP_SELECT 0x20

// буфер 1
#define BUFFER_1 0x00

// буфер 2
#define BUFFER_2 0x01

// определение всех кодов операций

// запись буфера 1
#define BUFFER_1_WRITE 0x84

// запись буфера 2
#define BUFFER_2_WRITE 0x87

// чтение буфера 1
#define BUFFER_1_READ 0x54

// чтение буфера 2
#define BUFFER_2_READ 0x56

// Буфер 1 в основную страницу памяти программы с встроенным стиранием
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83

// Буфер 2 в основную страницу памяти программы с встроенным стиранием
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86

// Буфер 1 в основную страницу памяти программы без встроенного стирания
#define B1_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x88

// Буфер 2 в основную страницу памяти программы без встроенного стирания
#define B2_TO_MM_PAGE_PROG_WITHOUT_ERASE 0x89

// Основная страница памяти программы сквозь буфер 1
#define MM_PAGE_PROG_THROUGH_B1 0x82

// Основная страница памяти программы сквозь буфер 2
#define MM_PAGE_PROG_THROUGH_B2 0x85

// автоматическая перезапись страницы через буфер 1
#define AUTO_PAGE_REWRITE_THROUGH_B1 0x58

// автоматическая перезапись страницы через буфер 2
#define AUTO_PAGE_REWRITE_THROUGH_B2 0x59

// сравнение основной страницы памяти с буфером 1
#define MM_PAGE_TO_B1_COMP 0x60

// сравнение основной страницы памяти с буфером 2
#define MM_PAGE_TO_B2_COMP 0x61

// передача основной страницы памяти в буфер 1
#define MM_PAGE_TO_B1_XFER 0x53

// передача основной страницы памяти в буфер 2
#define MM_PAGE_TO_B2_XFER 0x55

// регистр состояния DataFlash для чтения плотности, сравнения состояний,
// и состояния готов/занят
#define STATUS_REGISTER 0x57

// чтение основной страницы памяти
#define MAIN_MEMORY_PAGE_READ 0x52

// очистка 528 байт страницы
#define PAGE_ERASE 0x81

// очистка 512 страниц
#define BLOCK_ERASE 0x50

#define TRUE  0xFF
#define FALSE 0x00