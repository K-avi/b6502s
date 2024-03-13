#include "cpu.h"
#include "common.h"
#include "decode.h"
#include "memory.h"
#include "opcode.h"
#include <stdint.h>

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
    cpu->p = 0 ; // 00100100

    cpu->pc = mem_read(mem, 0xFFFC) | (mem_read(mem, 0xFFFD) << 8);
    
    #ifdef META_DATA
    cpu->ended = 0;
    cpu->cycles = 0;
    #endif
    
    return ERR_OK;
}//not sure of the value of p


uint8_t fetch_instruction(CPU *cpu, MEMORY *mem)
{
    return mem_read(mem, cpu->pc);
}


#ifdef debug 
void print_cpu(CPU *cpu);
#endif

err_flag cpu_start(CPU * cpu, MEMORY *mem)
{
    if (cpu == NULL || mem == NULL)
        return ERR_NULL;
    
    cpu_reset(cpu, mem);
    
    #ifdef META_DATA
    while (!cpu->ended)
    {
     
        uint8_t instr = fetch_instruction(cpu, mem); 

        exec_instruction(cpu, mem, instr);

        #ifdef debug    
        printf("after exec instruction %x " , instr);
        printf("cpu.a %x, %d\n" , cpu->a, cpu->a);
        print_cpu(cpu);      
        #endif
    }
    #else
    while (1)
    {
        uint8_t instr = fetch_instruction(cpu, mem);  
        decode_instruction(cpu, mem, instr);
    }
    #endif
    return ERR_OK;
}//simple enough

#ifdef debug
void print_cpu(CPU *cpu)
{
    printf("A: %02X X: %02X Y: %02X SP: %02X PC: %04X P: %02X\n", cpu->a, cpu->x, cpu->y, cpu->sp, cpu->pc, cpu->p);
    #ifdef META_DATA
    printf("Cycles: %lu\n", cpu->cycles);
    #endif
}
#endif
