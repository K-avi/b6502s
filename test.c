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

    data[0] = ADC_IMD;
    data[1] = 0 ;

    data[2] = NOP_IMP;

   // data[3] = META_DIE ;

    for(uint16_t i = 32 ; i < 256 ; i++)
    {
        data[i] = i - 32 ;
    }

    data[3] = ADC_AB ;
    data[4] = 32;
    data[5] = 0;

   

    data[6] = ADC_XAB ;
    data[7] = 32;
    data[8] = 0;

  
    data[9] = ADC_YAB ;
    data[10] = 32;
    data[11] = 0;

 

    data[12] = ADC_Z ;
    data[13] = 32;


    data[14] = ADC_XZ ;
    data[15] = 32;

    memset(&data[0], NOP_IMP, 16);

   // data[16] = ADC_XZI ;
    //data[17] = 32;
   // printf("data[32]= %d data[33]= %d\n", data[32], data[33]);
    
    data[16] = NOP_IMP ; 
    data[17] = NOP_IMP ;

    data[32] = 32 ;
    data[33] = 0 ; 

  
    //data[19] = 32;

    data[18] = ADC_ZIY;
    data[19] = 32;

    //data[18] = NOP_IMP ; 
    //data[19] = NOP_IMP ;

    data[20] = NOP_IMP;


    data[21] = META_DIE ;

    mem_load(&mem, data, 256);

    cpu_start(&cpu, &mem);

    print_cpu(&cpu);
    mem_print(&mem);

    mem_free(&mem);
    //cpu_free(&cpu);
    return 0;
}
