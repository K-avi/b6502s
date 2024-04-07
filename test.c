#include "src/common.h"
#include "src/cpu.h"
#include "src/decode.h"
#include "src/memory.h"
#include "src/opcode.h"
#include <stdio.h>
#include <string.h>

#include "src/cli.h"

int main(int argc, char **argv)
{
    
    CPU cpu;
    cpu_init(&cpu);
    MEMORY mem;

    printf("loading memory\n");
    mem_init(&mem, 256, RAM_MEM);
    mem_load_file(&mem, "refsheet/6502_functional_test.bin");

    //mem_print(&mem);

    //printf("Starting CPU\n");
    //cpu_start(&cpu, &mem, 0x400);

    printf("end of program\n");
    print_cpu(&cpu);
    mem_print(&mem);

    //mem_free(&mem);*/
    //cpu_free(&cpu);

    cli_t cli;
    cli_init(&cli);

    cpu.x = 255;
    cpu.a = 255;  

    cli_cpu_print(&cpu, &cli);
    
    getch(); 

    cpu.x = 0 ;
    cpu.a++;

     cli_cpu_print(&cpu, &cli);
    
    getch(); 


    cli_free(&cli);
    

    return 0;
}
