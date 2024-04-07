#include "cli.h"
#include <ncurses.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "cpu.h"


static void window_derwin
(window_t * win_child, window_t * wind_orig, int nlines, int ncols, int begin_y, int begin_x)
{
    win_child->win = derwin(wind_orig->win, nlines, ncols, begin_y, begin_x);
    win_child->width = ncols;
    win_child->height = nlines;
    win_child->x = begin_x;
    win_child->y = begin_y;

    box(win_child->win, ACS_VLINE, ACS_HLINE);
    wrefresh(win_child->win);
}

static void init_global_win(cli_t * cli) 
{
    cli->windows[w_global].win = stdscr;
    getmaxyx(stdscr, cli->windows[w_global].height, cli->windows[w_global].width);
    cli->windows[w_global].x = 0;
    cli->windows[w_global].y = 0;
    box(cli->windows[w_global].win, ACS_VLINE, ACS_HLINE);
    wrefresh(cli->windows[w_global].win);
}

static void init_cpu_win(cli_t * cli)
{
    window_derwin(&cli->windows[w_cpu], &cli->windows[w_global], 5, cli->windows[w_global].width - 2, 1, 1);
    box(cli->windows[w_cpu].win, ACS_VLINE, ACS_HLINE);

    wrefresh(cli->windows[w_cpu].win);
}

static void init_io_win(cli_t * cli)
{
    window_derwin(&cli->windows[w_io], &cli->windows[w_global], cli->windows[w_global].height - 7, cli->windows[w_global].width/2, 6, cli->windows[w_global].width/2);
    wrefresh(cli->windows[w_io].win);
}

static void init_command_win(cli_t * cli)
{
    window_derwin(&cli->windows[w_command], &cli->windows[w_global],cli->windows[w_global].height - 7, cli->windows[w_global].width/2 - 1, 6, 1);
    wrefresh(cli->windows[w_command].win);
}

void cli_init(cli_t * cli)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    //nodelay(stdscr, TRUE);
    curs_set(0);
    init_global_win(cli);
    init_cpu_win(cli);
    init_io_win(cli);
    init_command_win(cli); 
}

void cli_free(cli_t * cli)
{
    for(uint8_t i = 0; i < w_count; i++){
        delwin(cli->windows[i].win);
    }
    endwin();
}


void cli_cpu_print(CPU * cpu, cli_t * cli)
{   
   window_t win_cpu = cli->windows[w_cpu];

    uint8_t flusher[win_cpu.width-5];
    memset(flusher, ' ', win_cpu.width-5);
    flusher[win_cpu.width-5] = 0; 

   mvwprintw(win_cpu.win,win_cpu.y + 1 ,win_cpu.x, "%s", flusher); 
   mvwprintw( win_cpu.win,win_cpu.y + 1 ,win_cpu.x, "REGISTER VALUES : A=%u X=%u Y=%u P=%u PC=%u SP=%u",cpu->a,cpu->x, cpu->y, cpu->p, cpu->pc, cpu->sp);
   wrefresh(cli->windows[w_cpu].win);
}
