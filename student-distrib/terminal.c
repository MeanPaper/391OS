#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

terminal_t terminal;

/* void terminal_init();
 * Description: terminal_init, but do nothing.   
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: none
*/
void terminal_init(){
    return;
}

/* void terminal_open();
 * Description: terminal_open, but do nothing.   
 * 
 * Inputs: none
 * Output: 0
 * Return Value: none
 * Side Effects: none
*/
//function does nothing, return 0;
int32_t terminal_open(){
    return 0;
}

/* void terminal_close();
 * Description: terminal_close, but do nothing.   
 * 
 * Inputs: none
 * Output: 0
 * Return Value: none
 * Side Effects: none
*/
//function does nothing, return 0;
int32_t terminal_close(){
    return 0;
}


/* void terminal_read();
 * Description: read the keyboard buffer into the terminal buffer, than read into user buffer.  
 * 
 * Inputs: int fd,void * buf, uint32_t n_bytes
 * Output: none
 * Return Value: 0
 * Side Effects: write the terminal and user buffer
*/
int32_t terminal_read(int fd,void * buf, uint32_t n_bytes){
    if(!buf) return -1;
    while(!ENTER_PRESSED); //wait until user press enter. 
    
    cli();
    strncpy((int8_t*)(terminal.terminal_buf), (int8_t*)key_buffer, 127);

    if(n_bytes >= 128){ //buffer size is 128, if n_bytes >= 128, only write 128 bytes. 
        n_bytes = 128;
    }
    terminal.terminal_buf[n_bytes-1] = '\0';
    strncpy((int8_t*)buf,(int8_t*)(terminal.terminal_buf),n_bytes);
    sti();

    return 0;
    
}

/* void terminal_write();
 * Description: write from the user buffer to the screen using putc. .  
 * 
 * Inputs: int fd,void * buf, uint32_t n_bytes
 * Output: put user buffer information onto the screen
 * Return Value: 0
 * Side Effects: none
*/
int32_t terminal_write(int fd,void * buf, uint32_t n_bytes){
    if(!buf) return -1;
    int i;
    uint8_t* temp = (uint8_t*) buf;
    for(i = 0; i < n_bytes; ++i){
        if(temp[i] == '\0') break;
        putc(temp[i]);
    }
    if(i == 127){ //reach the end of line, add \n to go to next line. 
        putc('\n');
    }
    // strncpy(buf,terminal.terminal_buf,sizeof(terminal.terminal_buf));
    // puts(terminal.terminal_buf);
    return 0;
}
