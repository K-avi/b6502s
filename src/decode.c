#include "decode.h"
#include "common.h"
#include "cpu.h"
#include "memory.h"
#include "opcode.h"

#include <complex.h>
#include <stdint.h>
#include <sys/types.h>



//addressing mode functions / instruction functions 
typedef void (*addressing_fn)(MEMORY * mem, CPU * cpu);
typedef void (*instruction_fn)(MEMORY * mem, CPU * cpu, byte opcode);

//declarations for the big tables that are on the bottom of the file
static addressing_fn adressing_table[256]; 
static instruction_fn optable[256];
static const uint32_t ticktable[256];
static const byte nbbytes_table[256];


void exec_instruction(CPU *cpu, MEMORY *mem, byte opcode){
    //this modifies the global variable cur_addr, is_acc, is_imp
    adressing_table[opcode](mem, cpu);
    //this modifies the cpu struct
    optable[opcode](mem, cpu, opcode);

    //this increments the pc and cycles counters
    cpu->pc += nbbytes_table[opcode];
    #ifdef META_DATA
    cpu->cycles += ticktable[opcode];
    #endif
}


typedef uint16_t adress_t ; 
//global file variables to keep track of the current address bc it's easier 
static adress_t cur_addr = 0;
//global file variables to checkout wether an instruction is in acc or imp mode
static bool is_acc = false , is_imp = false;

/****** ADDRESSING MODE FUNCTIONS ******/

//implied
static void imp(MEMORY * mem, CPU * cpu){
    is_imp = true;
    is_acc = false;
}

//acc 
static void acc(MEMORY * mem, CPU * cpu){
    is_acc = true;
    is_imp = false;
}

//immediate
static void imd(MEMORY * mem, CPU * cpu){
    cur_addr = cpu->pc + 1;
    is_acc = is_imp = false;
}

//absolute 
static void abso(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1) | (mem_read(mem, cpu->pc + 2) << 8);
    is_acc = is_imp = false;
}

//absolute indexed x
static void absx(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1) | (mem_read(mem, cpu->pc + 2) << 8);
    cur_addr += cpu->x;
    is_acc = is_imp = false;
}

//absolute indexed y
static void absy(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1) | (mem_read(mem, cpu->pc + 2) << 8);
    cur_addr += cpu->y;
    is_acc = is_imp = false;
}

//absolute indirect 
static void absind(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1) | (mem_read(mem, cpu->pc + 2) << 8);
    cur_addr = mem_read(mem, cur_addr) | (mem_read(mem, cur_addr + 1) << 8);
    is_acc = is_imp = false;
}

//zero page
static void zp(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1);
    is_acc = is_imp = false;
}

//zero page indexed x
static void zpx(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1);
    cur_addr += cpu->x;
    is_acc = is_imp = false;
}

//zero page indexed y
static void zpy(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1);
    cur_addr += cpu->y;
    is_acc = is_imp = false;
}

//x indexed indirect
static void indx(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1);
    cur_addr += cpu->x;
    cur_addr = mem_read(mem, cur_addr) | (mem_read(mem, cur_addr + 1) << 8);
    is_acc = is_imp = false;
}

//indirect y indexed
static void indy(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1);
    cur_addr = mem_read(mem, cur_addr) | (mem_read(mem, cur_addr + 1) << 8);
    cur_addr += cpu->y;
    is_acc = is_imp = false;
}

//relative
static void rel(MEMORY * mem, CPU * cpu){
    cur_addr = mem_read(mem, cpu->pc + 1);
    is_acc = is_imp = false;
}

/****** DOCUMENTED INSTRUCTIONS FUNCTIONS ******/

void adc(MEMORY * mem, CPU * cpu, byte opcode){
    
    byte cpua_prev = cpu->a;
    cpu->a += mem_read(mem, cur_addr) + (cpu->p & FCARRY);

    COND_RAISE_FLAG(cpu, FCARRY, cpu->a < cpua_prev);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
    
    COND_RAISE_FLAG(cpu, FOVERFLOW, (! ((cpua_prev & 0x80) == (cpu->a & 0x80))));

    if(cpu->p & FDECIMAL){
        COND_RAISE_FLAG(cpu, FCARRY, 0);
        if(((cpua_prev & 0x0F) + (mem_read(mem, cur_addr) & 0x0F) + (cpu->p & FCARRY)) > 0x0F){
            cpu->a += 0x06;
        }
        if(cpu->a > 0x9F){
            cpu->a += 0x60;
            COND_RAISE_FLAG(cpu, FCARRY, 1);
            
        }
    }
}//tested ; decimal mode tested

void and(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->a &= mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
}//tested

void asl(MEMORY * mem, CPU * cpu, byte opcode){
    if(is_acc){
        COND_RAISE_FLAG(cpu, FCARRY, cpu->a & 0x80);
        cpu->a <<= 1;
        COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
        COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
    }else{
        byte value = mem_read(mem, cur_addr);
        COND_RAISE_FLAG(cpu, FCARRY, value & 0x80);
        value <<= 1;
        mem_write(mem, cur_addr, value);
        COND_RAISE_FLAG(cpu, FZERO, value == 0);
        COND_RAISE_FLAG(cpu, FNEGATIVE, value & 0x80);
    }
}//tested

void bcc(MEMORY * mem, CPU * cpu, byte opcode){
    if(!(cpu->p & FCARRY)){
        cpu->pc += cur_addr;
    }
}//tested

void bcs(MEMORY * mem, CPU * cpu, byte opcode){
    if(cpu->p & FCARRY){
        cpu->pc += cur_addr ;
    }
}//tested

void beq(MEMORY * mem, CPU * cpu, byte opcode){
    if(cpu->p & FZERO){
        cpu->pc += cur_addr;
    }
}//not tested (should be ok)

void bit(MEMORY * mem, CPU * cpu, byte opcode){
    byte value = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, (cpu->a & value) == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, value & 0x80);
    COND_RAISE_FLAG(cpu, FOVERFLOW, value & 0x40);
}//not tested (should be ok)

//branch minus
void bmi(MEMORY * mem, CPU * cpu, byte opcode){
    if(cpu->p & FNEGATIVE){
        cpu->pc += cur_addr;
    }
}//not tested (should be ok

void bne(MEMORY * mem, CPU * cpu, byte opcode){
    if(!(cpu->p & FZERO)){
        cpu->pc += cur_addr;
    }
}//not tested (should be ok)

//branch plus
void bpl(MEMORY * mem, CPU * cpu, byte opcode){
    if(!(cpu->p & FNEGATIVE)){
        cpu->pc += cur_addr;
    }
}//not tested (should be ok)

void brk_i(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->pc += 2;
    mem_write(mem, STACK_START + cpu->sp, cpu->pc >> 8);
    cpu->sp--;
    mem_write(mem, STACK_START + cpu->sp, cpu->pc & 0xFF);
    cpu->sp--;
    mem_write(mem, STACK_START + cpu->sp, cpu->p);
    cpu->sp--;
    cpu->p |= FBREAK;
    cpu->pc = mem_read(mem, INTERRUPT_VECTOR_LOW) | ((mem_read(mem, INTERRUPT_VECTOR_HIGH) << 8) );
}//not tested

void bvc(MEMORY * mem, CPU * cpu, byte opcode){
    if(!(cpu->p & FOVERFLOW)){
        cpu->pc += cur_addr;
    }
}//not tested (should be ok

void bvs(MEMORY * mem, CPU * cpu, byte opcode){
    if(cpu->p & FOVERFLOW){
        cpu->pc += cur_addr;
    }
}//not tested (should be ok)

void clc(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->p &= ~FCARRY;
}//not tested (should be ok)

void cld(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->p &= ~FDECIMAL;
}//not tested (should be ok)

void cli(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->p &= ~FINTERRUPT;
}//not tested (should be ok)

void clv(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->p &= ~FOVERFLOW;
}//not tested (should be ok)

void cmp(MEMORY * mem, CPU * cpu, byte opcode){
    byte value = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FCARRY, cpu->a >= value);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == value);
    COND_RAISE_FLAG(cpu, FNEGATIVE, (cpu->a - value) & 0x80);
}//not tested (should be ok)

void cpx(MEMORY * mem, CPU * cpu, byte opcode){
    byte value = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FCARRY, cpu->x >= value);
    COND_RAISE_FLAG(cpu, FZERO, cpu->x == value);
    COND_RAISE_FLAG(cpu, FNEGATIVE, (cpu->x - value) & 0x80);
}//not tested (should be ok)

void cpy(MEMORY * mem, CPU * cpu, byte opcode){
    byte value = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FCARRY, cpu->y >= value);
    COND_RAISE_FLAG(cpu, FZERO, cpu->y == value);
    COND_RAISE_FLAG(cpu, FNEGATIVE, (cpu->y - value) & 0x80);
}//not tested (should be ok)

void dec(MEMORY * mem, CPU * cpu, byte opcode){
    byte value = mem_read(mem, cur_addr);
    value--;
    mem_write(mem, cur_addr, value);
    COND_RAISE_FLAG(cpu, FZERO, value == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, value & 0x80);
}//not tested (should be ok)

void dex(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->x--;
    COND_RAISE_FLAG(cpu, FZERO, cpu->x == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->x & 0x80);
}//not tested (should be ok)

void dey(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->y--;
    COND_RAISE_FLAG(cpu, FZERO, cpu->y == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->y & 0x80);
}//not tested (should be ok)

void eor(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->a ^= mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
}//not tested (should be ok)

void inc(MEMORY * mem, CPU * cpu, byte opcode){
    byte value = mem_read(mem, cur_addr);
    value++;
    mem_write(mem, cur_addr, value);
    COND_RAISE_FLAG(cpu, FZERO, value == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, value & 0x80);
}//not tested (should be ok)

void inx(MEMORY * mem , CPU * cpu , byte opcode){
    cpu->x++;
    COND_RAISE_FLAG(cpu, FZERO, cpu->x == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->x & 0x80);
}//not tested (should be ok)

void iny(MEMORY * mem, CPU * cpu , byte opcode){
    cpu->y++; 
    COND_RAISE_FLAG(cpu, FZERO, cpu->y == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->y & 0x80);
}

void jmp(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->pc = cur_addr;
}//not tested (should be ok

void jsr(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->pc += 2;
    mem_write(mem, STACK_START + cpu->sp, cpu->pc >> 8);
    cpu->sp--;
    mem_write(mem, STACK_START + cpu->sp, cpu->pc & 0xFF);
    cpu->sp--;
    cpu->pc = cur_addr;
}//not tested (should be ok

void lda(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->a = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
}//not tested (should be ok

void ldx(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->x = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, cpu->x == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->x & 0x80);
}//not tested (should be ok)

void ldy(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->y = mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, cpu->y == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->y & 0x80);
}//not tested (should be ok)


void lsr(MEMORY * mem , CPU * cpu, byte opcode){
    if(is_acc){
        COND_RAISE_FLAG(cpu, FCARRY, cpu->a & 0x80);
        cpu->a >>= 1;
        COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
        COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
    }else{
        byte value = mem_read(mem, cur_addr);
        COND_RAISE_FLAG(cpu, FCARRY, value & 0x80);
        value >>= 1;
        mem_write(mem, cur_addr, value);
        COND_RAISE_FLAG(cpu, FZERO, value == 0);
        COND_RAISE_FLAG(cpu, FNEGATIVE, value & 0x80);
    }
}//not tested 

void nop(MEMORY * mem, CPU * cpu, byte opcode){
    //do nothing
}//ok

void ora(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->a |= mem_read(mem, cur_addr);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
}//not tested

void pha(MEMORY * mem, CPU * cpu, byte opcode){
    mem_write(mem, STACK_START + cpu->sp, cpu->a);
    cpu->sp--;
}//not tested

void php(MEMORY * mem, CPU * cpu, byte opcode){
    mem_write(mem, STACK_START + cpu->sp, cpu->p);
    cpu->sp--;
}//not tested

void pla(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->sp++;
    cpu->a = mem_read(mem, STACK_START + cpu->sp);
    COND_RAISE_FLAG(cpu, FZERO, cpu->a == 0);
    COND_RAISE_FLAG(cpu, FNEGATIVE, cpu->a & 0x80);
}//not tested

void plp(MEMORY * mem, CPU * cpu, byte opcode){
    cpu->sp++;
    cpu->p = mem_read(mem, STACK_START + cpu->sp);
}//not tested

void rol(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done 

void ror(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void rti(MEMORY * mem, CPU * cpu, byte opcode){
 
}//not done 

void rts(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sbc(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sec(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sed(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sei(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sta(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void stx(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sty(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void tax(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void tay(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void tsx(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void txa(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void txs(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void tya(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

#ifdef UNDOCUMENTED_OPCODES 

void anc(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void arr(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void asr(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void dcp(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void isc(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void jam(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void las(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void lax(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void rla(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void rra(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sax(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sbx(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sha(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void shs(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void shx(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void shy(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void slo(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void sre(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

void xaa(MEMORY * mem, CPU * cpu, byte opcode){
    
}//not done

#else 



#endif 



void meta_die(MEMORY * mem, CPU * cpu, byte opcode){
    #ifdef META_DATA
    cpu->ended = 1;
    #endif
}//ok


static instruction_fn optable[256] =
{
/*      |   0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |      */
/* 0 */ brk_i,  ora,  nop,  slo,  nop,  ora,  asl,  slo,  php,  ora,  asl,  nop,  nop,  ora,  asl,  slo, /* 0 */
/* 1 */   bpl,  ora,  nop,  slo,  nop,  ora,  asl,  slo,  clc,  ora,  nop,  slo,  nop,  ora,  asl,  slo, /* 1 */
/* 2 */   jsr,  and,  nop,  rla,  bit,  and,  rol,  rla,  plp,  and,  rol,  nop,  bit,  and,  rol,  rla, /* 2 */
/* 3 */   bmi,  and,  nop,  rla,  nop,  and,  rol,  rla,  sec,  and,  nop,  rla,  nop,  and,  rol,  rla, /* 3 */
/* 4 */   rti,  eor,  nop,  sre,  nop,  eor,  lsr,  sre,  pha,  eor,  lsr,  nop,  jmp,  eor,  lsr,  sre, /* 4 */
/* 5 */   bvc,  eor,  nop,  sre,  nop,  eor,  lsr,  sre,  cli,  eor,  nop,  sre,  nop,  eor,  lsr,  sre, /* 5 */
/* 6 */   rts,  adc,  nop,  rra,  nop,  adc,  ror,  rra,  pla,  adc,  ror,  nop,  jmp,  adc,  ror,  rra, /* 6 */
/* 7 */   bvs,  adc,  nop,  rra,  nop,  adc,  ror,  rra,  sei,  adc,  nop,  rra,  nop,  adc,  ror,  rra, /* 7 */
/* 8 */   nop,  sta,  nop,  sax,  sty,  sta,  stx,  sax,  dey,  nop,  txa,  nop,  sty,  sta,  stx,  sax, /* 8 */
/* 9 */   bcc,  sta,  nop,  nop,  sty,  sta,  stx,  sax,  tya,  sta,  txs,  nop,  nop,  sta,  nop,  nop, /* 9 */
/* A */   ldy,  lda,  ldx,  lax,  ldy,  lda,  ldx,  lax,  tay,  lda,  tax,  nop,  ldy,  lda,  ldx,  lax, /* A */
/* B */   bcs,  lda,  nop,  lax,  ldy,  lda,  ldx,  lax,  clv,  lda,  tsx,  lax,  ldy,  lda,  ldx,  lax, /* B */
/* C */   cpy,  cmp,  nop,  dcp,  cpy,  cmp,  dec,  dcp,  iny,  cmp,  dex,  nop,  cpy,  cmp,  dec,  dcp, /* C */
/* D */   bne,  cmp,  nop,  dcp,  nop,  cmp,  dec,  dcp,  cld,  cmp,  nop,  dcp,  nop,  cmp,  dec,  dcp, /* D */
/* E */   cpx,  sbc,  nop,  isc,  cpx,  sbc,  inc,  isc,  inx,  sbc,  nop,  sbc,  cpx,  sbc,  inc,  isc, /* E */
/* F */   beq,  sbc,  nop,  isc,  nop,  sbc,  inc,  isc,  sed,  sbc,  nop,  isc,  nop,  sbc,  inc,  isc  /* F */
};


static const uint32_t ticktable[256] = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */      7,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    4,    4,    6,    6,  /* 0 */
/* 1 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 1 */
/* 2 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    4,    4,    6,    6,  /* 2 */
/* 3 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 3 */
/* 4 */      6,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    3,    4,    6,    6,  /* 4 */
/* 5 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 5 */
/* 6 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    5,    4,    6,    6,  /* 6 */
/* 7 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 7 */
/* 8 */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* 8 */
/* 9 */      2,    6,    2,    6,    4,    4,    4,    4,    2,    5,    2,    5,    5,    5,    5,    5,  /* 9 */
/* A */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* A */
/* B */      2,    5,    2,    5,    4,    4,    4,    4,    2,    4,    2,    4,    4,    4,    4,    4,  /* B */
/* C */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* C */
/* D */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* D */
/* E */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* E */
/* F */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7   /* F */
};      

//table that takes into account the undocumented instructions
static const byte nbbytes_table[256] = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |    */
/* 0 */      1,    2,    1,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 0 */
/* 1 */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 1 */
/* 2 */      3,    2,    1,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 2 */
/* 3 */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 3 */
/* 4 */      1,    2,    1,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 4 */
/* 5 */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 5 */
/* 6 */      1,    2,    1,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 0 */
/* 7 */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 0 */
/* 8 */      2,    2,    2,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 0 */
/* 9 */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 0 */
/* A */      2,    2,    2,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 0 */
/* B */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 0 */
/* C */      2,    2,    2,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 0 */
/* D */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 0 */
/* E */      2,    2,    2,    2,    2,    2,    2,    2,    1,    2,    1,    2,    3,    3,    3,    3, /* 0 */
/* F */      2,    2,    1,    2,    2,    2,    2,    2,    1,    3,    1,    3,    3,    3,    3,    3, /* 0 */
};//done ; should prolly double check

static addressing_fn adressing_table[256] = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imd,  acc,  imd, abso, abso, abso, abso, /* 0 */
/* 1 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 1 */
/* 2 */    abso, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imd,  acc,  imd, abso, abso, abso, abso, /* 2 */
/* 3 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 3 */
/* 4 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imd,  acc,  imd, abso, abso, abso, abso, /* 4 */
/* 5 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 5 */
/* 6 */     imp, indx,  imp, indx,   zp,   zp,   zp,   zp,  imp,  imd,  acc,  imd,  absind, abso, abso, abso, /* 6 */
/* 7 */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* 7 */
/* 8 */     imd, indx,  imd, indx,   zp,   zp,   zp,   zp,  imp,  imd,  imp,  imd, abso, abso, abso, abso, /* 8 */
/* 9 */     rel, indy,  imp, indy,  zpx,  zpx,  zpy,  zpy,  imp, absy,  imp, absy, absx, absx, absy, absy, /* 9 */
/* A */     imd, indx,  imd, indx,   zp,   zp,   zp,   zp,  imp,  imd,  imp,  imd, abso, abso, abso, abso, /* A */
/* B */     rel, indy,  imp, indy,  zpx,  zpx,  zpy,  zpy,  imp, absy,  imp, absy, absx, absx, absy, absy, /* B */
/* C */     imd, indx,  imd, indx,   zp,   zp,   zp,   zp,  imp,  imd,  imp,  imd, abso, abso, abso, abso, /* C */
/* D */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx, /* D */
/* E */     imd, indx,  imd, indx,   zp,   zp,   zp,   zp,  imp,  imd,  imp,  imd, abso, abso, abso, abso, /* E */
/* F */     rel, indy,  imp, indy,  zpx,  zpx,  zpx,  zpx,  imp, absy,  imp, absy, absx, absx, absx, absx  /* F */
};
