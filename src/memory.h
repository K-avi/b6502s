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


#define ROM_MEM 0x01
#define RAM_MEM 0x02

typedef struct s_mem{
    byte *data ; 
    uint32_t size ;

    uint8_t type ;
}MEMORY ; 
typedef MEMORY ROM ; 
typedef MEMORY RAM ;

extern err_flag mem_init(MEMORY *mem, uint32_t size, uint8_t type);
extern void mem_free(MEMORY *mem);

extern byte mem_read(MEMORY *mem, uint16_t addr);
extern void mem_write(MEMORY *mem, uint16_t addr, byte data);

#ifdef debug
extern void mem_print(MEMORY *mem);
extern void mem_load(MEMORY *mem, byte *data, uint32_t size);
extern void mem_load_file(MEMORY *mem, char *filename);

#endif

#endif
