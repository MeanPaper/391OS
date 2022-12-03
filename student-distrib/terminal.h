#ifndef _TERMINAL_H
#define _TERMINAL_H
#include "types.h"

typedef struct terminal{
    uint8_t terminal_buf[128];
    uint32_t screen_x;
    uint32_t screen_y;
    int32_t rtc_frequency;  // rtc frequency; -1 means rtc not in use / not open
    int32_t rtc_counter;    // how many interrupts for the next handle
    int32_t rtc_flag;       // set to 1 when read, read till it is set to 0;
    uint32_t terminal_id;
    uint8_t current_process_id; 
    int32_t key_index;
    int32_t read;
}terminal_t;

extern uint32_t current_term_id;
extern uint32_t display_terminal;
extern terminal_t terminal;

terminal_t terms[3];

extern void terminal_init();

//function does nothing, return 0;
extern int32_t terminal_open();

//function does nothing, return 0;
extern int32_t terminal_close();
extern int32_t terminal_read(int fd,void * buf, int32_t n_bytes);
extern int32_t terminal_write(int fd, const void * buf, int32_t n_bytes);
extern int32_t set_display_term(int32_t term_index); // setting current term index, use to switch terminal
#endif /* _TERMINAL_H */
