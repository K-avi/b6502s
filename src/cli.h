#ifndef CLI_H 
#define CLI_H 

#include "common.h"
#include <ncurses.h>
#include <sys/types.h>
#include "cpu.h"


typedef struct s_window{
    WINDOW *win;
    int height;
    int width;
    int y;
    int x;
}window_t ;

typedef struct s_cli{ 
    
    window_t windows[4] ;
}cli_t;
//this is bad implem tbh 
#define w_count 4 
#define w_global 0
#define w_cpu 1
#define w_io 2
#define w_command 3 



/*
initializes the windows of the cli structure and draws funky boxes
*/
void cli_init(cli_t * cli);
void cli_free(cli_t * cli);
void cli_cpu_print(CPU * cpu, cli_t * cli);

#endif  
