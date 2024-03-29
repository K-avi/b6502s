#ifndef DECODE_H 
#define DECODE_H 

#include "common.h"
#include "cpu.h"
#include "memory.h"


#define CLEAR_FLAG(cpu, flag) {cpu->p &= (byte)~(flag);}
#define RAISE_FLAG(cpu, flag) {cpu->p |= (byte)(flag);}

#define COND_RAISE_FLAG(cpu, flag, cond) {if(cond){ RAISE_FLAG(cpu, flag);}else {CLEAR_FLAG(cpu, flag);}}

extern void exec_instruction(CPU *cpu, MEMORY *mem, byte opcode) ;

#endif 
