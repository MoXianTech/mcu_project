#ifndef __PRIV_MALLOC_H__
#define __PRIV_MALLOC_H__

#include "stdint.h"

#ifndef NULL
#define NULL 0
#endif

#define SRAM_1	 0
#define SRAM_2   1

#define SRAMBANK 	2

#define MEM1_BLOCK_SIZE			512
#define MEM1_MAX_SIZE			20 * 1024
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE / MEM1_BLOCK_SIZE


#define MEM2_BLOCK_SIZE			32
#define MEM2_MAX_SIZE			1 * 64
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE / MEM2_BLOCK_SIZE

struct _m_mallco_dev
{
	void (*init)(uint8_t);
	uint8_t  (*perused)(uint8_t);
	uint8_t  *membase[SRAMBANK];
	uint16_t *memmap[SRAMBANK];
	uint8_t  memrdy[SRAMBANK];
};
extern struct _m_mallco_dev mallco_dev;

void mymemset(void *s,uint8_t c,uint32_t count);
void mymemcpy(void *des,void *src,uint32_t n);
void my_mem_init(uint8_t memx);
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);
uint8_t my_mem_free(uint8_t memx,uint32_t offset);
uint8_t my_mem_perused(uint8_t memx);

void myfree(uint8_t memx,void *ptr);
void *mymalloc(uint8_t memx,uint32_t size);
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);
void *board_malloc(uint32_t size);
void board_free(void *ptr);
void *board_realloc(void *ptr, uint32_t size);
void *board_calloc(uint32_t size);
#endif


