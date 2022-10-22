#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

terminal_t terminal;
void terminal_init(){
    return;
}

//function does nothing, return 0;
int32_t terminal_open(){
    return 0;
}

//function does nothing, return 0;
int32_t terminal_close(){
    return 0;
}

int32_t terminal_read(int fd,void * buf, uint32_t n_bytes){
    while(!ENTER_PRESSED);
    // buf = (uint8_t*) buf;
    strncpy(terminal.terminal_buf,key_buffer,127);
    terminal.terminal_buf[127] = '\0';
    return 0;
    
}

int32_t terminal_write(int fd,void * buf, uint32_t n_bytes){
    // strncpy(buf,terminal.terminal_buf,sizeof(terminal.terminal_buf));
    puts(terminal.terminal_buf);
}
