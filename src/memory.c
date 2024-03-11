#include "memory.h"
#include "common.h"
#include <stdint.h>
#include <string.h>

err_flag mem_init(MEMORY *mem, uint32_t size, uint8_t type)
{
    if (mem == NULL)
        return ERR_NULL ;
    
    if (size > MEM_MAX_SIZE)
        size = MEM_MAX_SIZE ;

    //malloc instead of calloc bc a "real" memory wouldn't be zeroed
    mem->data = (byte *)malloc(size*sizeof(byte)) ;
    if (mem->data == NULL)
        return ERR_ALLOC ;
    
    mem->size = size ;
    mem->type = type ;
    return ERR_OK ;
}//should do error handling

void mem_free(MEMORY *mem)
{
    if(mem){
        if(mem->data){
            free(mem->data) ;
            mem->data = NULL ;
        }
        mem->size = 0 ;
    }
}//:thumbsup:


byte mem_read(MEMORY *mem, uint16_t address)
{
    if (mem == NULL)
        return 0 ;
    if (address >= mem->size)
        return 0 ;
    
    return mem->data[address] ;
}//works ; should do propper error handling


#ifdef debug
void mem_print(MEMORY *mem)
{
    if (mem == NULL)
        return ;
    uint16_t elem = 0 ; 
    uint16_t page = 0 ;

    //print each page
    //as a 16*16 grid for pagination 
    for(uint32_t i = 0 ; i < mem->size ; i++)
    {
        if(elem%256 == 0){
            printf("----page : %u----\n", page) ;
            page++ ;
        }
        printf("%02X ", mem->data[i]) ;
        if (elem % 16 == 15){
            printf("\n") ;
        }
        elem++ ;
    }
    printf("\n");
}

void mem_load(MEMORY *mem, byte *data, uint32_t size)
{//highly unsafe
    if(mem->data)
        free(mem->data) ;

    mem->data = (byte *)malloc(size*sizeof(byte)) ;
    memcpy(mem->data, data, size) ;
    mem->size = size ;
 
}//debug helper please do not use it 
#endif
