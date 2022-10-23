#ifndef _TERMINAL_H
#define _TERMINAL_H
#include "types.h"

extern void terminal_init();

//function does nothing, return 0;
extern int32_t terminal_open();

//function does nothing, return 0;
extern int32_t terminal_close();
extern int32_t terminal_read(int fd,void * buf, uint32_t n_bytes);
extern int32_t terminal_write(int fd,void * buf, uint32_t n_bytes);

typedef struct terminal{
    uint8_t terminal_buf[128];
    uint32_t screen_x;
    uint32_t screen_y;
    uint32_t terminal_id;
}terminal_t;

#endif /* _TERMINAL_H */
