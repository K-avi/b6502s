#include "decode.h"
#include "common.h"
#include "cpu.h"
#include "memory.h"
#include "opcode.h"


static uint8_t generic_adc(CPU* cpu, uint8_t val1, uint8_t val2){
   
    uint8_t sign = val1 & 1<<7;
    uint16_t result16 = val1 + val2;

    COND_RAISE_FLAG(cpu, FOVERFLOW, (sign & result16) != sign)
    COND_RAISE_FLAG(cpu, FZERO, (result16 & 0xFF) == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, (result16 & 1<<7));

    uint8_t result = result16 & 0xFF;

    if(cpu->p & FDECIMAL){
        // if the result is greater than 9, add 6 to the result 
        //(because 6 is the diff between hex and decimal)

        if(((val1 & 0xF) + (val2 & 0xF) + (cpu->p | FCARRY)) > 9){
            result += 6;
        }

        COND_RAISE_FLAG(cpu, FCARRY, result16 > 0x99);
        
    }else{
        COND_RAISE_FLAG(cpu, FCARRY, result16 > 0xFF);
        
    }   
    return result;
}//not tested

static void fn_adc_imd(CPU *cpu, MEMORY *mem)
{
    uint8_t value = mem_read(mem, cpu->pc + 1);
    
    cpu->a = generic_adc(cpu, value, cpu->a);
    cpu->pc += 2;

    #ifdef META_DATA
    cpu->cycles += 2;
    #endif
}//not tested

static void fn_adc_ab(CPU *cpu, MEMORY *mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint8_t high = mem_read(mem, cpu->pc + 2);

    uint16_t address = (high << 8) | low;
    uint8_t value = mem_read(mem, address);

    cpu->a = generic_adc(cpu, value, cpu->a);
    cpu->pc += 3;

    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//not tested

static void fn_adc_xab(CPU *cpu, MEMORY *mem)
{
   uint8_t low = mem_read(mem, cpu->pc + 1);
   uint8_t high = mem_read(mem, cpu->pc + 2);

   uint16_t address = ((high << 8) | low )+ cpu->x;
   uint8_t value = mem_read(mem, address);

    cpu->a = generic_adc(cpu, value, cpu->a);
    cpu->pc += 3;

    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//the number of cycles can be 5 if the page is crossed
//which is not implemented yet

static void fn_adc_yab(CPU *cpu, MEMORY *mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint8_t high = mem_read(mem, cpu->pc + 2);

    uint16_t address = ((high << 8) | low )+ cpu->y;
    uint8_t value = mem_read(mem, address);

    cpu->a = generic_adc(cpu, value, cpu->a);
    cpu->pc += 3;

    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//the number of cycles can be 5 if the page is crossed

static void fn_adc_z(CPU *cpu, MEMORY *mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint16_t adress = low & ZP_START; 

    uint8_t value = mem_read(mem, adress);
    cpu->a = generic_adc(cpu, value, cpu->a);

    cpu->pc += 2;

    #ifdef META_DATA
    cpu->cycles += 3;
    #endif
}//not tested

static void fn_adc_xz(CPU *cpu, MEMORY *mem)
{
    uint8_t low = mem_read(mem, cpu->pc + 1);
    uint16_t adress = (low + cpu->x) & ZP_START; 

    uint8_t value = mem_read(mem, adress);
    cpu->a = generic_adc(cpu, value, cpu->a);

    cpu->pc += 2;

    #ifdef META_DATA
    cpu->cycles += 4;
    #endif
}//not tested


static void fn_adc_xzi(CPU *cpu, MEMORY *mem)
{
 
}//what is this adressing mode lmao 

static void fn_adc_ziy(CPU *cpu, MEMORY *mem)
{
 
}



static void fn_nop(CPU *cpu, MEMORY *mem)
{
    return ;
    cpu->pc += 1;
}

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


    [NOP_IMP] = &fn_nop,
} ;


