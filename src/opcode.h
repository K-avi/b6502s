#ifndef OPCODE_H 
#define OPCODE_H 
/*
    Adressing modes naming convention : 
    (follows the order from  https://www.pagetable.com/c64ref/6502/?tab=3)

    IMP : Implied
    AC : Accumulator
    IMD : Immediate

    AB : Absolute
    XAB : X-Indexed Absolute
    YAB : Y-Indexed Absolute
    
    ABI : Absolute Indirect
    
    Z : Zero Page  
    XZ : X-Indexed Zero Page
    YZ : Y-Indexed Zero Page
    XZI : X-Indexed Zero Page Indirect
    ZIY : Zero Page Indirect Y-Indexed

    REL : Relative

    
*/

//why does this bad boy have so many addressing modes 
//MOS was crazy for this ngl

//Add to Accumullator with carry
#define ADC_IMD 0x69
#define ADC_AB 0x6D
#define ADC_XAB 0x7D
#define ADC_YAB 0x79
#define ADC_Z 0x65
#define ADC_XZ 0x75
#define ADC_XZI 0x61
#define ADC_ZIY 0x71 

//And memory with Accumulator
#define AND_IMD 0x29
#define AND_AB 02D
#define AND_XAB 0x3D
#define AND_YAB 0x39
#define AND_Z 0x25
#define ADC_XZ 0x75
#define AND_XZI 0x21
#define AND_ZIY 0x31

//Arithmetic Shift Left
#define ASL_ACC 0x0A 
#define ASL_AB 0x0E 
#define ASL_XAB 0x1E 
#define ASL_Z 0x06 
#define ASL_XZ 0x16

//Branch on Carry Clear
#define BCC_R 0x90 //Blackpool Combat Club instruction :O

//Branch on Carry Set
#define BCS_R 0xB0

//Branch Equal 
#define BEQ_R 0xF0 //F-zero instruction :O

//Test Bits in Memory with Accumulator 
#define BIT_AB 0x2C 
#define BIT_Z 0x24 

//Branch on Minus Result 
#define BMI_R 0x30

//Branch on Result Not Zero 
#define BNE_R 0xD0

//Branch on Result Plus 
#define BPL_R 0x10

//Break 
//ignored if IRQ at the same time
#define BRK_IMP 0x00 

//Branch on Overflow Clear 
#define BVC_R 0x50

//Branch on Overflow Set 
#define BVS 0x70 

//Clear Carry Flag 
#define CLC_IMP 0x18

//Clear Decimal Mode
//The value of the decimal mode flag is indeterminate after a RESET. 
#define CLD_IMP 0xD8

//Clear Interrupt Disable 
#define CLI_IMP 0x58

//Clear Overflow Flag 
#define CLV_IMP 0xB8

//Compare Memory and Accumulator 
#define CMP_IMD 0xC9
#define CMP_AB 0CD
#define CMP_XAB 0xDD
#define CMP_YAB 0xD9
#define CMP_Z 0xC5
#define CMP_XZ 0xD5
#define CMP_XZI 0xC1
#define CMP_ZIY 0xD1

//Compare Index Register X to memory 
#define CPX_IMD 0xE0
#define CPX_ABS 0xEC 
#define CPX_Z 0xE4

//Compare Index Register Y to memory 
#define CPY_IMD 0xC0
#define CPY_ABS 0xCC 
#define CPY_Z 0xC4

//Decrement Memory By one 
#define DEC_ABS 0xCE 
#define DEC_XABS 0xC6
#define DEC_Z 0xDE 
#define DEC_XZ 0xD6 

//Decrement Index Register X by one 
#define DEX_IMP 0xCA 

//Decrement Index Register Y by one 
#define DEY_IMP 0x88 

//Exclusive or Memory with Accumulator 
#define EORD_IMP 0x49
#define EOR_AB 0x4D
#define EOR_XAB 0x5D
#define EOR_YAB 0x59
#define EOR_Z 0x45
#define EOR_XZ 0x55
#define EOR_XZI 0x41
#define EOR_ZIY 0x51

//Increment Memory by one
#define INC_ABS 0xEE
#define INC_XABS 0xFE
#define INC_Z 0xE6
#define INC_XZ 0xF6

//Increment Index Register X by one
#define INX_IMP 0xE8

//Increment Index Register Y by one
#define INY_IMP 0xC8

//Jump to New Location
#define JMP_ABS 0x4C
#define JMP_ABI 0x6C

//Jump to Subroutine
#define JSR_ABS 0x20

//Load Accumulator with Memory
#define LDA_IMD 0xA9
#define LDA_AB 0xAD
#define LDA_XAB 0xBD
#define LDA_YAB 0xB9
#define LDA_Z 0xA5
#define LDA_XZ 0xB5
#define LDA_XZI 0xA1
#define LDA_ZIY 0xB1

//Load Index Register X from Memory
#define LDX_IMD 0xA2
#define LDX_AB 0xAE
#define LDX_YAB 0xBE
#define LDX_Z 0xA6
#define LDX_ZPY 0xB6

//Load Index Register Y from Memory
#define LDY_IMD 0xA0
#define LDY_AB 0xAC
#define LDY_XAB 0xBC
#define LDY_Z 0xA4
#define LDY_XZ 0xB4




//Logical Shift Right 
#define LSR_AC 0x4A
#define LSR_AB 0x4E
#define LSR_XAB 0x5E
#define LSR_Z 0x46
#define LSR_XZ 0x56

//No Operation
#define NOP_IMP 0xEA

//OR Memory with Accumulator
#define ORA_IMD 0x09
#define ORA_AB 0x0D
#define ORA_XAB 0x1D
#define ORA_YAB 0x19
#define ORA_Z 0x05
#define ORA_XZ 0x15
#define ORA_XZI 0x01
#define ORA_ZIY 0x11

//Push Accumulator on Stack
#define PHA_IMP 0x48

//Push Processor Status on Stack
#define PHP_IMP 0x08

//Pull Accumulator from Stack
#define PLA_IMP 0x68

//Pull Processor Status from Stack
#define PLP_IMP 0x28

//Rotate Left
#define ROL_AC 0x2A //rotate the air conditionner :O
#define ROL_AB 0x2E
#define ROL_XAB 0x3E
#define ROL_Z 0x26
#define ROL_XZ 0x36

//Rotate Right
#define ROR_AC 0x6A // RAWR :O
#define ROR_AB 0x6E
#define ROR_XAB 0x7E
#define ROR_Z 0x66
#define ROR_XZ 0x76

//Return from Interrupt
#define RTI_IMP 0x40

//Return from Subroutine
#define RTS_IMP 0x60

//Subtract Memory from Accumulator with Borrow
#define SBC_IMD 0xE9 //Single Board Computer Instruction ????
#define SBC_AB 0xED
#define SBC_XAB 0xFD
#define SBC_YAB 0xF9
#define SBC_Z 0xE5
#define SBC_XZ 0xF5
#define SBC_XZI 0xE1
#define SBC_ZIY 0xF1

//Set Carry Flag
#define SEC_IMP 0x38 //gimme a sec

//Set Decimal Mode
#define SED_IMP 0xF8 //Stream EDitor instruction (very real)

//Set Interrupt Disable
#define SEI_IMP 0x78 

//Store Accumulator in Memory
#define STA_AB 0x8D
#define STA_XAB 0x9D
#define STA_YAB 0x99
#define STA_Z 0x85
#define STA_XZ 0x95
#define STA_XZI 0x81
#define STA_ZIY 0x91

//Store Index Register X in Memory
#define STX_AB 0x8E
#define STX_Z 0x86 //the beer opcode
#define STX_ZIY 0x96

//Store Index Register Y in Memory
#define STY_AB 0x8C
#define STY_Z 0x84
#define STY_XZ 0x94

//Transfer Accumulator to Index X
#define TAX_IMP 0xAA //Double A Arn Anderson mentionned

//Transfer Accumulator to Index Y
#define TAY_IMP 0xA8

//Transfer Stack Pointer to Index X
#define TSX_IMP 0xBA

//Transfer Index X to Accumulator
#define TXA_IMP 0x8A

//Transfer Index X to Stack Pointer
#define TXS_IMP 0x9A

//Transfer Index Y to Accumulator
#define TYA_IMP 0x98

//Special opcode to indicate the end of the program
#define META_DIE 0xFF 

//FINALLY ITS OVER FFS 
#endif 
