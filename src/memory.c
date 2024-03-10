#include "memory.h"
#include "common.h"

err_flag mem_init(MEMORY *mem, uint16_t size)
{
    if (mem == NULL)
        return ERR_NULL ;
    
    //malloc instead of calloc bc a "real" memory would be full of garbage
    mem->data = (byte *)malloc(size*sizeof(byte)) ;
    if (mem->data == NULL)
        return ERR_ALLOC ;
    
    mem->size = size ;
    return ERR_OK ;
}//trivial

void mem_free(MEMORY *mem)
{
    if(mem){
        if(mem->data){
            free(mem->data) ;
            mem->data = NULL ;
        }
        mem->size = 0 ;
    }
}//trivial
