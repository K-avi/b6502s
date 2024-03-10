#include "cpu.h"
#include "common.h"
#include "memory.h"

err_flag cpu_init(CPU *cpu)
{
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->sp = 0;
    cpu->pc = 0;
    cpu->p = 0 ;

    #ifdef META_DATA
    cpu->cycles = 0;
    #endif

    return ERR_OK;
}//simple enough

err_flag cpu_reset(CPU *cpu, MEMORY *mem)
{
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;

    cpu->sp = 0xFD;
    cpu->p = 0x24; // 00100100

    cpu->pc = mem_read(mem, 0xFFFC) | (mem_read(mem, 0xFFFD) << 8);
    
    #ifdef META_DATA
    cpu->cycles = 0;
    #endif
    
    return ERR_OK;
}//not sure of the value of p
