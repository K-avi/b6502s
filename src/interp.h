#ifndef INTERP_H
#define INTERP_H

#include "common.h"
#include "cpu.h"
#include "memory.h"
#include <stdint.h>

/* 
should  I be writing an interpreter for this 
probably not. But I wanna do it cuz it's funny so on we go :O

tbh the "language" will be very minimalist and won't even support 
variables or control flow. It'll just a debugging tool for simple commands 

syntax : 

"dump [file]"
"load [file]" 
"run"
"step" 
"stop" 
"set a [value]" 
"set x [value]"
"set y [value]"
"set pc [value]"
"set p [value]" 
"set [adress] [value]"
"print [page index]"
"print cpu"
"quit"

instructions can be separated by '\n' or ';' 
filename doesn't support escaped characters.

*/

extern void interp_instr(char * instr, CPU * cpu, MEMORY * mem);


#endif
