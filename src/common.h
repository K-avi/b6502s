#ifndef COMMON_H 
#define COMMON_H 

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8 ;
typedef uint8_t byte ;

typedef uint8_t err_flag ; 
enum {
    ERR_OK = 0, ERR_DEF,
    ERR_NULL, ERR_ALLOC, 
    ERR_VAL
}; 


#define debug
#define META_DATA

#endif 
