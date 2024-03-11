#include "src/common.h"
#include "src/cpu.h"
#include "src/decode.h"
#include "src/memory.h"
#include "src/opcode.h"
#include <string.h>

int main(int argc, char **argv)
{

    CPU cpu;
    cpu_init(&cpu);
    MEMORY mem;

    mem_init(&mem, 256, RAM_MEM);

    byte data[256]; 
    memset(data, 0, 256);

    data[0] = ADC_IMD;
    data[1] = 12 ;

    data[2] = NOP_IMP;

   // data[3] = META_DIE ;

    for(uint16_t i = 32 ; i < 256 ; i++)
    {
        data[i] = i;
    }

    data[3] = ADC_AB ;
    data[4] = 32;
    data[5] = 0;

    data[6] = META_DIE ;

    mem_load(&mem, data, 256);

    cpu_start(&cpu, &mem);

    print_cpu(&cpu);
    mem_print(&mem);

    mem_free(&mem);
    //cpu_free(&cpu);
    return 0;
}
