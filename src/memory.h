#ifndef MEMORY_H 
#define MEMORY_H

#include "common.h"

#define MEM_MAX_SIZE 0x10000
#define MEM_MAX_ADDR 0xFFFF

#define ZP_START 0x0000
#define ZP_END 0x00FF
#define ZP_SIZE 0x0100

#define STACK_START 0x0100
#define STACK_END 0x01FF
#define STACK_SIZE 0x0100


#define ROM_FLAG 0x01
#define RAM_FLAG 0x02

typedef struct s_mem{
    byte *data ; 
    uint16_t size ;

    uint8_t type ;
}MEMORY ; 
typedef MEMORY ROM ; 
typedef MEMORY RAM ;

extern err_flag mem_init(MEMORY *mem, uint16_t size, uint8_t type);
extern void mem_free(MEMORY *mem);

extern byte mem_read(MEMORY *mem, uint16_t addr);

#endif
