#ifndef SOC_H 
#define SOC_H

#include "common.h"
#include "cpu.h"
#include "memory.h"


typedef struct s_keyboard KEYBOARD ; 
typedef struct s_display DISPLAY ;

typedef struct  S_SOC{
    CPU cpu ;
    MEMORY mem ; //atm the "memory" will be a simple array of bytes
    //but I'd like to make it more accurate with the different SoC components
    //like RAM, ROM, etc being their own structs that are mapped to the memory
    //without the CPU knowing the difference

    //which means I'll also need to implement a bus ig
}SOC ;
//keyboard and display are not implemented yet
//I'll focus on the CPU and memory first


#endif 
