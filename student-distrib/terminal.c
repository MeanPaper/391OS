#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "system_call.h"
#include "paging.h"


// terminal_t terminal;
uint32_t current_term_id = 0;
uint32_t display_terminal = 0;
uint32_t user_video_pg = VIDEO_PHYS_ALTER;


/* void terminal_init();
 * Description: terminal_init, but do nothing.   
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: 
 *      Set the current terminal to the 0th shell and display_terminal to the 0th terminal
 * as well. 
*/
void terminal_init(){
    int i;  // initialize three terminal and assign the terminal id for it
    for(i = 0; i < 3; ++i){
        memset(terms + i, 0, sizeof(terminal_t));
        terms[i].terminal_id = i;
        terms[i].rtc_frequency = -1;
    }
    current_term_id = 0;
    display_terminal = 0;
    terms[display_terminal].read = 0;
    memcpy((uint8_t*)VIDEO_PHYS, (uint8_t*)vram_addrs[display_terminal], FOUR_KB);  // restore the content from the physical
    return;
}



/* int32_t set_display_term(int32_t term_index)
 * Description: 
 *      Once our keyboard receive ALT + F_num, it will call the function to set the 
 * display terminal. This function will first check if the input is valid. If not it will
 * return -1. We will unmap the current display terminal. Then we will map the terminal 
 * we want to display. Then we will set the display terminal to the input terminal. 
 * 
 * Inputs: 
 *      int32_t term_index
 *          this is terminal index that we want to display
 * Output: none
 * Return Value: 
 *      0: on success
 *      -1: on failure
 * Side Effects: 
 *      We will unmap the current display terminal. Then we will map the terminal 
 * we want to display. Then we will set the display terminal to the input terminal. 
 */
int32_t set_display_term(int32_t term_index){
    if(term_index > 2 || term_index < 0){
        return -1;
    }

    map_sched_video_page(display_terminal);   
    memcpy((uint8_t*)vram_addrs[display_terminal],(uint8_t*)VIDEO_PHYS, FOUR_KB);   // save the video content to the memory
    memcpy((uint8_t*)VIDEO_PHYS, (uint8_t*)vram_addrs[term_index], FOUR_KB);  // restore the content from the physical
    display_terminal = term_index;
    map_sched_video_page(current_term_id);    
    return 0;
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
    while(!terms[current_term_id].read); //wait until user press enter.
    terms[current_term_id].read = 0;

    strncpy((int8_t*)(terms[current_term_id].terminal_buf), (int8_t*)key_buffer, 127);

    if(n_bytes >= 128){ //buffer size is 128, if n_bytes >= 128, only write 128 bytes. 
        n_bytes = 128;
    }
    terms[current_term_id].terminal_buf[n_bytes-1] = '\0';
    strncpy((int8_t*)buf,(int8_t*)(terms[current_term_id].terminal_buf),n_bytes);
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

    return i;
}
