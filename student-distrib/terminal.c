#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
#include "system_call.h"
#include "paging.h"


terminal_t terminal;
uint32_t current_term_id = 0;
uint8_t terminal_active_count = 1;
uint32_t display_terminal = 0;

// const uint32_t vram_addrs[3] = {TERM1_VIDEO, TERM2_VIDEO, TERM3_VIDEO};

/* void term_video_unmap(uint32_t current_term);
 * Description:  
 *      First, we find the page_table_entry for the current terminal virtual address.
 * the address >> 12 is the index inside our first page_table. It retrieves the page 
 * through the page_table. Then we change the page_base_addr to the terminal's own
 * video page. Then we send this bask again. since we change the mapping, now we need
 * to flush_TLB() and copy the display video memory to the terminal video memory. 
 *  
 * Inputs: uint32_t current_term
 * Output: none
 * Return Value: 0
 * Side Effects: none
*/
int32_t term_video_unmap(uint32_t current_term){
    int32_t term_page_table_entry = vram_addrs[current_term] >> 12;
    page_table_entry_t temp;
    temp.val = first_page_table[term_page_table_entry];
    temp.page_base_addr = vram_addrs[current_term] >> 12;
    first_page_table[term_page_table_entry] = temp.val;
    flush_TLB();
    memcpy((uint8_t*)vram_addrs[current_term],(uint8_t*)VIDEO_PHYS, FOUR_KB);
    return 0;
}

/* void term_video_map(uint32_t current_term);
 * Description:  
 *      First, we copy the terminal video memory to the terminal memory. Then, we 
 *      find the page_table_entry for the current terminal virtual address.
 *      the address >> 12 is the index inside our first page_table. It retrieves the page 
 *      through the page_table. Then we change the page_base_addr to the terminal's own
 *      video page. Then we send this bask again. since we change the mapping, now we need
 *      to flush_TLB()
 * Inputs: uint32_t current_term
 * Output: none
 * Return Value: 0
 * Side Effects: none
 */
int32_t term_video_map(uint32_t current_term){
    // clear();d
    memcpy((uint8_t*)VIDEO_PHYS, (uint8_t*)vram_addrs[current_term], FOUR_KB);
    int32_t term_page_table_entry = vram_addrs[current_term] >> 12;
    page_table_entry_t temp;
    temp.val = first_page_table[term_page_table_entry];
    temp.page_base_addr = VIDEO_PHYS >> 12;
    first_page_table[term_page_table_entry] = temp.val;
    flush_TLB();
    return 0;
}

/* void save_current_cursor(int x, int y)
 * Description: Store the cursor position of the current terminal
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: none
 */
void save_current_cursor(int x, int y){
    terms[display_terminal].screen_x = x;
    terms[display_terminal].screen_y = y;
}
// int32_t term_video_switching(uint8_t next_term);


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
    display_terminal = 0;
    term_video_map(display_terminal);
    terminal = terms[display_terminal];
    return;
}

int32_t set_display_term(int32_t term_index){
    if(term_index > 2 || term_index < 0){
        return -1;
    }

    cli();
    term_video_unmap(display_terminal);
    term_video_map(term_index);
    // flush_TLB();
    terminal = terms[term_index];
    display_terminal = term_index;
    // current_pid_num = terms[term_index].current_process_id;
    if(terminal_active_count < 3 && active_terminal[term_index] == -1){
        ++terminal_active_count;
        sti();
        execute_on_term((uint8_t*)"shell", term_index);
    }
    sti();
    return 0;
}

// int32_t term_video_switching(uint8_t next_term){
//     // video_mem_swap(current_term_id, next_term);
//     map_program_page(terms[next_term].current_process_id);      // remap prog virtual memory
//     flush_TLB(); 
//     return 0;
// }


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
