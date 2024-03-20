#include "src/common.h"
#include "src/cpu.h"
#include "src/decode.h"
#include "src/memory.h"
#include "src/opcode.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{

    CPU cpu;
    cpu_init(&cpu);
    MEMORY mem;

    mem_init(&mem, 256, RAM_MEM);

    byte data[256]; 
    memset(data, 0, 256);

    data[0] = ADC_IMD ;
    data[1] = 0xFF;

    data[2] = NOP_IMP;

    data[3] = ADC_IMD;
    data[4] = 0x00;

    data[5] = BCS_R;
    data[6] = 0xE;
    data[7] = META_DIE ;

    data[0x15] = ADC_IMD; 
    data[0x16] = 0x01;
    data[0x17] = META_DIE ;





    

    mem_load(&mem, data, 256);

    cpu_start(&cpu, &mem);

    print_cpu(&cpu);
    mem_print(&mem);

    mem_free(&mem);
    //cpu_free(&cpu);
    return 0;
}
