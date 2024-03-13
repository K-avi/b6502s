#include "decode.h"
#include "common.h"
#include "cpu.h"
#include "memory.h"
#include "opcode.h"

#include <stdint.h>
#include <stdio.h>


/******ADRESSING MODE FUNCTION **********/

//these function implement 
//each adressing mode of the 6502 asm 
//they will be used by the opcode function 
//to get adresses 

//NB : the adressing mode functions
//will increment the program counter
//and return the adress to be used 
//the "increment the pc" part 
//might sound ugly bc it's a side effect 

typedef struct s_addressing_util{
    uint16_t address; //the actual address fetched
    uint8_t size; //Program counter increment
}ADR_UTIL;

static ADR_UTIL adressing_imp(CPU* cpu, MEMORY* mem)
{
    return (ADR_UTIL){0, 1};
}

static ADR_UTIL adressing_acc(CPU* cpu, MEMORY* mem)
{
    return (ADR_UTIL){0, 1};
}

static ADR_UTIL adressing_imd(CPU* cpu, MEMORY* mem)
{
    return (ADR_UTIL){cpu->pc + 1 & 0xFF , 2};
}//tested; works

static ADR_UTIL adressing_abs(CPU* cpu, MEMORY* mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint8_t high = mem_read(mem, cpu->pc + 2);

    return (ADR_UTIL){(high << 8) | low, 3};
}//tested; works

static ADR_UTIL adressing_xab(CPU* cpu, MEMORY* mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint8_t high = mem_read(mem, cpu->pc + 2);

    return (ADR_UTIL){((high << 8) | low) + cpu->x, 3};
}

static ADR_UTIL adressing_yab(CPU* cpu, MEMORY* mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint8_t high = mem_read(mem, cpu->pc + 2);

    return (ADR_UTIL){((high << 8) | low) + cpu->y, 3};
}

static ADR_UTIL adressing_abi(CPU* cpu, MEMORY* mem)
{

    return (ADR_UTIL){0,0};
}//not done

static ADR_UTIL adressing_z(CPU* cpu, MEMORY* mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    return (ADR_UTIL){low & 0xFF, 2};
}

static ADR_UTIL adressing_xz(CPU* cpu, MEMORY* mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    return (ADR_UTIL){(low + cpu->x) & 0xFF, 2};
}

static ADR_UTIL adressing_yz(CPU* cpu, MEMORY* mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    return (ADR_UTIL){(low + cpu->y) & 0xFF, 2};
}

static ADR_UTIL adressing_xzi(CPU* cpu, MEMORY* mem)
{
   //the intermediate value is the sum of the 
    //next byte and the x register
    uint8_t interm_addr = mem_read(mem, cpu->pc + 1);
    //the intermediate adress is used to fetch 
    //an adress from the zero page which is then 
    //used to fetch the value to be added to the accumulator
    uint16_t zp_adress = (interm_addr + cpu->x ) ;

    uint8_t low = mem_read(mem, zp_adress);
    uint8_t high = mem_read(mem, zp_adress + 1);

    #ifdef debug
    /*printf("intemr_addr %d zp.adr = %d, cpu.x = %d \n", interm_addr, zp_adress, cpu->x);
    printf("low = %d, high = %d\n", low, high);
    printf("adr xzi low = %d, high = %d\n", low, high);*/
    #endif

    uint16_t adress = (high << 8) | low;

    #ifdef debug
   // printf("adress = %d\n", adress);
    #endif

    return (ADR_UTIL){adress, 2};
}

static ADR_UTIL adressing_ziy(CPU* cpu, MEMORY* mem)
{
    //the intermediate value is the sum of the 
    //next byte and the x register
    uint16_t zp_adress = mem_read(mem, cpu->pc + 1 ) ;
    //the intermediate adress is used to fetch 
    //an adress from the zero page which is then 
    //used to fetch the value to be added to the accumulator
  
    uint8_t low = mem_read(mem, zp_adress);
    uint8_t high = mem_read(mem, zp_adress + 1);

    uint16_t effective_low = low + cpu->y;
    uint8_t effective_high = high + ((effective_low >> 8) & 0x1);

    uint16_t adress = (effective_high << 8) | (effective_low & 0xFF) ;

    return (ADR_UTIL){adress, 2};
}//might be wrong

static ADR_UTIL adressing_rel(CPU* cpu, MEMORY* mem)
{
    uint8_t offset = mem_read(mem, cpu->pc + 1);
    return (ADR_UTIL){cpu->pc + 2 + offset, 2}; 
    //+2 because the offset is relative to the next instruction
}

/*****************************************/
/*******INSTRUCTION FUNCTIONS************/
/****************************************/

//these function will implement each 
//referenced instruction of 6502 asm 
//however they won't do the "adressing"
//part which is handled by 
//the next section of this file 

static uint8_t generic_adc(CPU* cpu, uint8_t val, uint8_t acc){
   
    uint8_t sign = acc & (1<<7);
    uint16_t result16 = val + acc;

    uint8_t result_16_bit_seven = result16 & (1<<7);

    COND_RAISE_FLAG(cpu, FOVERFLOW, (sign != result_16_bit_seven) );
    COND_RAISE_FLAG(cpu, FZERO, ((result16 & 0xFF) == 0)); 
    COND_RAISE_FLAG(cpu, FNEGATIVE, (result16 & 1<<7));

    uint8_t result = result16 & 0xFF;

    if(cpu->p & FDECIMAL){
        // if the result is greater than 9, add 6 to the result 
        //(because 6 is the diff between hex and decimal)
        //AWFUL CODE SHOULD SIMPLIFY !!!!

        //perforoms the addition on upper and lower bits 
        byte upper4 = ((val & 0xF0) + (acc & 0xF0) );
        byte lower4 = ((val & 0xF) + (acc & 0xF) + (cpu->p & FCARRY));

        result = (upper4 & 0xF0) | (lower4 & 0x0F);

        //if decimal overflow add 0x6
        if( lower4 > 0x9){
            result += 0x6;
        }

        //if the upper 4 bits are greater than 9 you must set 
        //the upper 4 to upper4 - 0x9 and set the carry 
        //this does the "set to upper4 -0x9" in a 
        //GODAWFUL way ; but it does it ig
        if( (result & 0xF0) > 0x90){
          
            upper4 = result & 0xF0;
            result &=  ~0xF0;
            upper4 = upper4 - 0x90 ;
            result |= upper4;
            COND_RAISE_FLAG(cpu, FCARRY, 1);
        }
    }else{      
        COND_RAISE_FLAG(cpu, FCARRY, (result16 > 0xFF) );     
    } 
  
    return result;
}/*fnegative, fcarry foverflow fzero seem to work
decimal mode seems fine*/


static byte generic_and(CPU* cpu, byte val, byte acc)
{
    byte result = val & acc;
    COND_RAISE_FLAG(cpu, FZERO, (result == 0));
    COND_RAISE_FLAG(cpu, FNEGATIVE, (result & 1<<7));
    return result;
}//tested; seems ok ; flags work 

static byte generic_asl(CPU * cpu , byte val)
{
    COND_RAISE_FLAG(cpu, FCARRY, (val & 1<<7));
    val = val << 1;
    COND_RAISE_FLAG(cpu, FZERO, (val == 0));
    COND_RAISE_FLAG(cpu, FNEGATIVE, (val & 1<<7));
    return val;
}// tested; seems ok ; flags work

static byte generic_bcc(CPU * cpu, byte val)
{
    if(!(cpu->p & FCARRY)){
        cpu->pc += val;
    }
    return 0;
}//not tested


/******************************************/
/*************OPCODE FUNCTIONS************/
/****************************************/


//adc opcode functions 
static void fn_adc_imd(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_imd(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_adc(cpu, value, cpu->a);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 2;
    #endif
}//tested ; seems ok 

static void fn_adc_ab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_abs(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);

    cpu->a = generic_adc(cpu, value, cpu->a);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//tested; seems ok 

static void fn_adc_xab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_xab(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);

    cpu->a = generic_adc(cpu, value, cpu->a);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//the number of cycles can be 5 if the page is crossed
//which is not implemented yet
//tested; seems ok

static void fn_adc_yab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_yab(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);

    cpu->a = generic_adc(cpu, value, cpu->a);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//the number of cycles can be 5 if the page is crossed
//tested; seems ok

static void fn_adc_z(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_z(cpu, mem);

    uint8_t value = mem_read(mem, adr.address);
    cpu->a = generic_adc(cpu, value, cpu->a);

    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 3;
    #endif
}//tested ; seems ok

static void fn_adc_xz(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_xz(cpu, mem);

    uint8_t value = mem_read(mem, adr.address);
    cpu->a = generic_adc(cpu, value, cpu->a);

    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//tested; seems ok

static void fn_adc_xzi(CPU *cpu, MEMORY *mem)
{

    ADR_UTIL adr = adressing_xzi(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);

    cpu->a = generic_adc(cpu, value, cpu->a);

    cpu->pc += 2;
    #ifdef META_DATA
    cpu->cycles += 6;
    #endif
}//tested; seems ok

static void fn_adc_ziy(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_ziy(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);

    cpu->a = generic_adc(cpu, value, cpu->a);

    cpu->pc += 2;
    #ifdef META_DATA
    cpu->cycles += 5;
    #endif
}//tested; seems ok
//cpu cycles can be 6 if the page is crossed
//so it's not accurate atm


//and opcode functions
static void fn_and_imd(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_imd(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
   
    #ifdef META_DATA
    cpu->cycles += 2;
    #endif
}//tested; seems ok

//the adressing mode functions for and are not tested however 
//each individual adressing mode is tested and 
//the generic_and function is tested so they  **should** be ok
static void fn_and_ab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_abs(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//not tested

static void fn_and_xab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_xab(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//not tested

static void fn_and_yab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_yab(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//not tested

static void fn_and_z(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_z(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 3;
    #endif
}//not tested

static void fn_and_xz(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_xz(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//not tested

static void fn_and_xzi(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_xzi(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 6;
    #endif
}//not tested

static void fn_and_ziy(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_ziy(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    
    cpu->a = generic_and(cpu, value, cpu->a);
    cpu->pc += adr.size;
    
    #ifdef META_DATA
    cpu->cycles += 5;
    #endif
}//not tested

static void fn_asl_acc(CPU *cpu, MEMORY *mem)
{
    cpu->a = generic_asl(cpu, cpu->a);
    
    cpu->pc += 1;
    #ifdef META_DATA
    cpu->cycles += 2;
    #endif
}//tested

static void fn_asl_ab(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_abs(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    value = generic_asl(cpu, value);
    
    mem_write(mem, adr.address, value);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 6;
    #endif
}//tested

//other asl adressing mode functions are not tested
//however the generic_asl function is tested so they should be ok
static void fn_asl_xab(CPU * cpu , MEMORY * mem){

    ADR_UTIL adr = adressing_xab(cpu, mem); 
    uint8_t value = mem_read(mem, adr.address);
    value = generic_asl(cpu, value);

    mem_write(mem, adr.address, value);

    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 7;    
    #endif
}

static void fn_asl_z(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_z(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    value = generic_asl(cpu, value);
    
    mem_write(mem, adr.address, value);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 5;
    #endif
}//not tested

static void fn_asl_xz(CPU *cpu, MEMORY *mem)
{
    ADR_UTIL adr = adressing_xz(cpu, mem);
    uint8_t value = mem_read(mem, adr.address);
    value = generic_asl(cpu, value);
    
    mem_write(mem, adr.address, value);
    
    cpu->pc += adr.size;
    #ifdef META_DATA
    cpu->cycles += 6;
    #endif
}//not tested

//nop opcode function
static void fn_nop(CPU *cpu, MEMORY *mem)
{
    cpu->pc += 1;
    #ifdef META_DATA
    cpu->cycles += 2;
    #endif
}//tested; works

//special instruction for the VM to end the program
static void fn_endprog(CPU *cpu, MEMORY *mem)
{
    //cpu->pc = 0;
    #ifdef META_DATA
    cpu->ended = 1;
    #endif

}//tested; works

instruction_fn instruction_func_array[256] = {

    //Add with carry instructions
    [ADC_IMD] = &fn_adc_imd,
    [ADC_AB] = &fn_adc_ab,
    [ADC_XAB] = &fn_adc_xab,
    [ADC_YAB] = &fn_adc_yab,
    [ADC_Z] = &fn_adc_z,
    [ADC_XZ] = &fn_adc_xz,
    [ADC_XZI] = &fn_adc_xzi,
    [ADC_ZIY] = &fn_adc_ziy,
    //and memory with accumulator instructions
    [AND_IMD] = &fn_and_imd,
    [AND_AB] = &fn_and_ab,
    [AND_XAB] = &fn_and_xab,
    [AND_YAB] = &fn_and_yab,
    [AND_Z] = &fn_and_z,
    [AND_XZ] = &fn_and_xz,
    [AND_XZI] = &fn_and_xzi,
    [AND_ZIY] = &fn_and_ziy,
    //asl instructions
    [ASL_ACC] = &fn_asl_acc,
    [ASL_AB] = &fn_asl_ab,
    [ASL_XAB] = &fn_asl_xab,
    [ASL_Z] = &fn_asl_z,
    [ASL_XZ] = &fn_asl_xz,
    //nop and end of program
    [NOP_IMP] = &fn_nop,
    [META_DIE] = &fn_endprog,   
};

extern void exec_instruction(CPU *cpu, MEMORY *mem, byte opcode){
    instruction_fn fn = instruction_func_array[opcode];
    if(fn){
        fn(cpu, mem);
    }
}//tested; works
