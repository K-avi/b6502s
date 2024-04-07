#ifndef CPU_H 
#define CPU_H

#include "common.h"
#include "memory.h"

#define RESET_VECTOR_LOW 0xFFFC
#define RESET_VECTOR_HIGH 0xFFFD

#define INTERRUPT_VECTOR_LOW 0xFFFE
#define INTERRUPT_VECTOR_HIGH 0xFFFF

#define FCARRY (1<<0) 
#define FZERO (1<<1)
#define FINTERRUPT_DISABLE (1<<2)
#define FINTERRUPT FINTERRUPT_DISABLE

#define FDECIMAL_MODE (1<<3)
#define FDECIMAL FDECIMAL_MODE

#define FBREAK (1<<4)
#define FUNUSED (1<<5)
#define FOVERFLOW (1<<6)
#define FNEGATIVE (1<<7)

typedef struct s_reg{
    byte a; //accumulator
    byte x; //x index
    byte y; //y index
    byte sp; //stack pointer
    uint16_t pc; //program counter
    byte p; //processor status

    byte opcode; //current opcode

    #ifdef META_DATA
    uint8_t ended;
    uint64_t cycles; //number of cycles 
    #endif
}CPU;

extern err_flag cpu_init(CPU *cpu);
extern err_flag cpu_reset(CPU *cpu, MEMORY *mem, uint16_t start_addr);
extern err_flag cpu_start(CPU * cpu, MEMORY *mem, uint16_t start_addr);

#ifdef debug 
extern void print_cpu(CPU *cpu);
#endif

#endif
