#include "interp.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static uint8_t error = 0 ;

typedef uint8_t token_t ;

//the keyword that started a fight on twitter :O
enum token_types{
    TOKEN_ERROR, 
    TOKEN_NUMBER,
    TOKEN_STR,
    TOKEN_KEYWORD,
    TOKEN_EOF, //every program should end w eof
};

enum keyword_vals{

    KEYWORD_ERR, //special error keyword

    KEYWORD_DUMP, //"dump" : used to write mem to file
    KEYWORD_LOAD, //"load" : used to load mem from file
    KEYWORD_RUN,  //"run" used to start the cpu in a thread
    KEYWORD_STEP, //"step" used to single step the cpu
    KEYWORD_STOP, //"stop" used to stop (pause) the cpu
    KEYWORD_SET, //"set" used to set a register or an address to a value

    //register names
    KEYWORD_A, // "a"
    KEYWORD_PC, // "pc"
    KEYWORD_SP, // "sp"
    KEYWORD_P,  // "p"
    KEYWORD_X, // "x"
    KEYWORD_Y, // "y"
    
    
    KEYWORD_PRINT, // "print" used to print a page or the cpu 
    KEYWORD_CPU,  // "cpu"

    KEYWORD_QUIT, // "quit" quit cli 
};




/*
simple union to store the value of a token 
*/
typedef union u_token_value{
    uint32_t number ; 
    char * str ; 
    uint8_t keyword ;
}token_u;
/*
struct for the token ; fairly straightforward 
type + value thing ; I'm not reinveting the wheel tbh
*/
typedef struct s_token{
    token_t type ; 
    token_u value ;
}token_s ; 
/*
program structure is simply a 
dynamic array of tokens
*/
typedef struct s_program{
    uint32_t size_max; 
    uint32_t size_cur ; 

    token_s * tokens ; 
}program_s;

/*****PARSE FUNCTIONS********/
static uint32_t parse_number(char * input){

    //error cases 
    if(!input){error = 1 ; return 0 ; } //nullptr 
    if(*input == '\0'){ error = 1 ; return 0 ; } //empty string
    if( (*(input+1) == 'x' || *(input+1) == 'b' ) ){ //'0x' or '0b' strings 
        if(*(input+2) == '\0'){ error = 1 ; return 0 ; }   
    }

    char * endptr = NULL ;
    if(*input == '0'){ //octal or binary of hex 
        //shouldn't use the base 0 thing ; i'll do it w a switch or something
        uint32_t ret = strtol(input, &endptr, 0) ;

        if(*endptr != ' ' || *endptr != '\n' || *endptr != '\0') error = 1 ;
        return ret ;
    }else{ //decimal 
        uint32_t ret = strtol(input, &endptr, 10) ;
        if(*endptr != ' ' || *endptr != '\n' || *endptr != '\0') error = 1 ;
        return ret ;
    }  
}//not tested

static char * parse_string(char * input){

    if(!input){error = 1 ; return NULL ; } //nullptr
    if(*input == '\0'){ error = 1 ; return NULL ; } //empty string
    if(*input != '"'){ error = 1 ; return NULL ; } //not a string

    char * endptr = input + 1 ;

    while(*endptr != '"' && *endptr != '\0') endptr++ ;
    char * ret = malloc(endptr - input + 1) ; 
    if(!ret){error = 1 ; return NULL ; } //malloc failed

    memcpy(ret, input, endptr - input) ;
    ret[endptr - input] = '\0' ;
    return ret ;
}//not tested; prolly wrong w quotes and stuff
//won't support escape characters bc why would 
//you name your file "file\".txt" ?

#define is_sep(chr) ((chr) == ' ')
#define is_newtok(chr) ((chr) == '\n' || (chr) == ';')

static inline uint8_t is_seq(char * str , char * substr ){
    char * s1 = str, *s2 = substr ;
    while(*s1 == *s2 && *s1 && *s2){
        s1++; 
        s2++;
    }
    if(!*s1 && *s2){
        return 0 ; 
    }else if(!*s1 && !*s2){
        return 1 ;
    }else if(*s1 && !*s2){
        return is_sep(*s1);
    }else{
        return 0;
    }
}//not tested; simple string util , returns true if str begins with substr + a separator character 
//false otherwise
static uint8_t lex_keyword(char * input){

    switch (*input) {
        case('a') : //register a
            {
                char next = *(input+1);
                if(is_sep(next)){
                    return KEYWORD_A ; 
                }
            }
            break; 
        case('c') : //cpu
            if(is_seq(input+1, "pu")){
                return KEYWORD_CPU ; 
            }
            break;
        case('d' ) : //dump
            if(is_seq(input+1, "ump")){
                return KEYWORD_DUMP ; 
            }
            break; 
        case('l') : //load
            if(is_seq(input+1, "oad")){
                return KEYWORD_LOAD ; 
            }
            break; 
        case('p') : //p or pc or print
            if(is_seq(input+1,"rint" )){
                return KEYWORD_PRINT ; 
            }else if (is_seq(input+1, "c")) {
                return KEYWORD_PC ; 
            }else if(*(input+1)== ' ' || *(input+1)==';' || *(input+1)=='\n'){
                    return KEYWORD_P ;
                
            }
            break;
        case('q') : // quit
            if(is_seq(input+1, "uit")){
                return KEYWORD_DUMP ; 
            }
            break;
        case('r') : //run 
            if(is_seq(input+1, "un")){
                return KEYWORD_DUMP ; 
            }
            break;
        case('s') : //step or stop or set or sp
            if(is_seq(input+1, "tep")){
                return KEYWORD_STEP ; 
            }else if(is_seq(input+1, "top")){
                return KEYWORD_STOP ; 
            }else if(is_seq(input+1, "p")){
                return KEYWORD_SP ; 
            }
            break; 
        case('x') : //x register
            {
                char next = *(input+1);
                if(is_sep(next)){
                    return KEYWORD_X ; 
                }
            }
            break;
        case('y'): //y register
            {
                char next = *(input+1);
                if(is_sep(next)){
                    return KEYWORD_Y ; 
                }
            }
            break;
        default : 
            {
            error = 1 ; 
            return KEYWORD_ERR ;   
            } 
    }

    error = 1 ; 
    return KEYWORD_ERR ;
}//not tested; 



static uint8_t parse_keyword(char * input){
    return 0 ;
}

/*
static function to interpret a single token from a given string 
also sets the string to the next separator character or end of string
*/
static token_s tokenize(char * input ){
    token_s ret;

    if(!input){
        ret.type = TOKEN_ERROR ;
        ret.value.number = 0 ;
        return ret ;
    }

    while(*input == ' ') input++ ; 
    
    if(*input == '\0'){
        ret.type = TOKEN_EOF ;
        ret.value.number = 0 ;
        return ret ;
    }

    switch(*input){
        case '0' ... '9': //GNU is so real for this
            ret.type = TOKEN_NUMBER ;
            ret.value.number = parse_number(input) ;
            if(error) ret.type = TOKEN_ERROR ;
            break ; 
        case '"':
            ret.type = TOKEN_STR ;
            ret.value.str = parse_string(input) ;
            if(error) ret.type = TOKEN_ERROR ;
            break ;
        case 'a' ... 'z':
            ret.type = TOKEN_KEYWORD ;
            ret.value.keyword = parse_keyword(input) ;
            if(error) ret.type = TOKEN_ERROR ;
            break ;
        default:
            ret.type = TOKEN_ERROR ;
            ret.value.number = 0 ;
            return ret ;
            break ;
    }
    while(*input != ' ' && *input != ';' && *input != '\0') input++ ;
    if(*input) input++ ;

    return ret ; 
}//not done


/*
function to execute ONE full instruction 
*/
void interp_instr(char * instr, CPU * cpu, MEMORY * mem){

    token_s ins = tokenize(instr); 

    if(ins.type = TOKEN_ERROR) return ; 

    return ; 
    
}

static uint8_t size_tab[16] = {
    1, //err 
    2, //dump 
    2, //load 
    1, //run 
    1, //step 
    1, //stop 
    3, //set 
    0, //A
    0,  //pc
    0, //sp
    0, //p
    0, //x
    0, //y 
    2, //print 
    0, //cpu
    1, //quit
};

uint8_t rule_err(token_s token, char * input);
uint8_t rule_dump(token_s token, char * input);
uint8_t rule_load(token_s token, char * input);

uint8_t rule_run(token_s token, char * input);
uint8_t rule_step(token_s token, char * input);

uint8_t rule_stop(token_s token, char * input);
uint8_t rule_set(token_s token, char * input);

uint8_t rule_print(token_s token, char * input);
uint8_t rule_quit(token_s token, char * input);



typedef uint8_t (*parse_fn)(token_s token, char * input);

static parse_fn parse_table[16] = {

    rule_err, 
    rule_dump,
    rule_load,
    rule_run, 
    rule_step, 
    rule_stop, 
    rule_set, 
    rule_err,
    rule_err,
    rule_err, 
    rule_err,
    rule_print, 
    rule_err, 
    rule_quit,
};


typedef struct prg{

    uint32_t max ; 
    uint32_t cur ; 

    token_s * program ; 
}s_program ; 



typedef struct strtab{
    uint32_t max ; 
    uint32_t cur ; 

    char ** elems ; 
}str_tab;


#define DEF_STRTAB_SIZE 8
static str_tab * init_strtab(uint32_t size ){

    str_tab * ret = malloc(sizeof(str_tab));
    if(!ret){
        error = 1 ; 
        return NULL ;
    }
    ret->elems = malloc(sizeof(char*) * size );
    if(!ret->elems){
        free(ret); 
        error = 1 ; 
        return NULL ; 
    }

    ret->max = size ; 
    ret->cur = 0 ; 

    return ret ; 
}

static str_tab * cut_string(char * input){

    
    str_tab * ret  = init_strtab(DEF_STRTAB_SIZE); 


    while(*input){
        input ++ ;
    }

    return ret ; 
}

/*
turns a string of a program into an tokenized program
*/
uint8_t tokenize_program(s_program * ret_program, char * input ){


   
}

