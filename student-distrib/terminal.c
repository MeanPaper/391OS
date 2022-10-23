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
    if(!buf) return -1;
    while(!ENTER_PRESSED);
    
    cli();
    strncpy((int8_t*)(terminal.terminal_buf), (int8_t*)key_buffer, 127);

    if(n_bytes >= 128){
        n_bytes = 128;
    }
    terminal.terminal_buf[n_bytes-1] = '\0';
    strncpy((int8_t*)buf,(int8_t*)(terminal.terminal_buf),n_bytes);
    sti();

    return 0;
    
}

int32_t terminal_write(int fd,void * buf, uint32_t n_bytes){
    if(!buf) return -1;
    int i;
    uint8_t* temp = (uint8_t*) buf;
    for(i = 0; i < n_bytes; ++i){
        if(temp[i] == '\0') break;
        putc(temp[i]);
    }
    if(i == 127){
        putc('\n');
    }
    // strncpy(buf,terminal.terminal_buf,sizeof(terminal.terminal_buf));
    // puts(terminal.terminal_buf);
    return 0;
}
