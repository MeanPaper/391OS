#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "system_call.h"
#include "paging.h"

terminal_t terminal;
uint32_t current_term_id = 0;

int32_t term_video_switching(uint8_t next_term);
/* void terminal_init();
 * Description: terminal_init, but do nothing.   
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: none
*/
void terminal_init(){
    int i;  // initialize three terminal and assign the terminal id for it
    for(i = 0; i < 3; ++i){
        memset(terms + i, 0, sizeof(terminal_t));
        terms[i].terminal_id = i;
    }
    current_term_id = 0;
    return;
}

int32_t set_current_term(uint8_t term_index){
    if(term_index > 2 || term_index < 0){
        return -1;
    }
    terminal = terms[term_index];
    

    // switch the page and do the work

    current_term_id = term_index;
    return 0;
}

int32_t term_video_switching(uint8_t next_term){
    memcpy(vram_addrs[current_term_id], VIDEO_PHYS, FOUR_KB);   // copy the current view to the copy
    memcpy(VIDEO_PHYS, vram_addrs[next_term], FOUR_KB);         // load the next view to the position
    map_program_page(terms[next_term].current_process_id);      // remap prog virtual memory
    flush_TLB; 
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
 * Return Value: the number of byte read
 * Side Effects: write the terminal and user buffer
*/
int32_t terminal_read(int fd,void * buf, int32_t n_bytes){
    if(!buf) return -1;
    sti();
    while(!ENTER_PRESSED); //wait until user press enter.
    ENTER_PRESSED = 0; 
    cli();
    strncpy((int8_t*)(terminal.terminal_buf), (int8_t*)key_buffer, 127);

    if(n_bytes >= 128){ //buffer size is 128, if n_bytes >= 128, only write 128 bytes. 
        n_bytes = 128;
    }
    terminal.terminal_buf[n_bytes-1] = '\0';
    strncpy((int8_t*)buf,(int8_t*)(terminal.terminal_buf),n_bytes);
    sti();
    // return the number of byte read
    uint32_t read_size = strlen((const int8_t*) buf);
    if (read_size > n_bytes) return n_bytes;
    else return read_size;    
}

/* void terminal_write();
 * Description: write from the user buffer to the screen using putc. .  
 * 
 * Inputs: int fd,void * buf, uint32_t n_bytes
 * Output: put user buffer information onto the screen
 * Return Value: the number of byte write
 * Side Effects: none
*/
int32_t terminal_write(int fd, const void * buf, int32_t n_bytes){
    if(!buf) return -1;
    int i;
    uint8_t* temp = (uint8_t*) buf;
    for(i = 0; i < n_bytes; ++i){
       // if(temp[i] == '\0') break;
        putc(temp[i]);
    }
    if(i == 127){ //reach the end of line, add \n to go to next line. 
        putc('\n');
    }
    // strncpy(buf,terminal.terminal_buf,sizeof(terminal.terminal_buf));
    // puts(terminal.terminal_buf);
    return i;
}
