#include "sys_call_helper.h"
#include "lib.h"
#include "keyboard.h"

#define MAX_PROCESS 6
// uint8_t * vram = (uint8_t*)(VIDEO_VIR + FOUR_KB);

// the magic header of elf
uint8_t elf_magic[ELF_MAGIC_SIZE] = {0x7f, 0x45, 0x4c, 0x46}; // "DEL,E,L,F"
uint32_t current_pid_num = 0;           // this is kinda cheating, since there are 3 processes use by the terminal, there will be 16kb gap between
uint32_t exception = 0;
uint32_t process_in_use = 0;
int32_t process_active[MAX_PROCESS] = {-1, -1, -1, -1, -1, -1}; // the index + 1 will the pcb location
int32_t active_terminal[3] = {-1,-1,-1};   // -1 means unactive, having pid number means active

void set_exception_flag(uint32_t num){
    exception = num;
}

uint32_t get_process_total(){
    return process_in_use;
}

uint32_t get_current_pid() {
    return current_pid_num;
}

int get_availiable_pid(){
    int i;
    for(i = 0; i < MAX_PROCESS; ++i){
        if(process_active[i]==-1) return i+1;
    }
    return -1;
}

file_descriptor_t set_up_stdin();
file_descriptor_t set_up_stdout();
int32_t stdout_read(int fd,void * buf, int32_t n_bytes);
int32_t stdin_write(int fd, const void * buf, int32_t n_bytes);
int32_t close_helper(int32_t fd);

/*
 * halt 
 * Description: 
 *      Terminate the current program and close all the relevant fd. Report 
 *      back to the current execute and then return back to parent execute
 * Input: uint8_t status
 * Output: none
 * Return value: status code from halt
 */

// page fault if execute fish after exiting the seventh shell
int32_t halt (uint8_t status){
    /* halt must return a value to the parent execute system call so that 
     * we know how the program ended */

    // how do we grap the program
    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    // pcb_t * parent = (pcb_t*)(GET_PCB(current->parent_pid));
    int32_t status_32 = (int32_t)status;
    if(exception) status_32 = 256; //if exception, status is 256
    exception = 0;

    // Restore parent paging
    map_program_page(current->parent_pid);
    flush_TLB();
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (current->parent_pid -1));
    
    // Close all relevant FDs
    int i;
    for(i = 0; i < FD_ARRAY_SIZE; ++i){
        if(current->fd_array[i].flags){
            close_helper(i);
            current->fd_array[i].flags = 0;
        }
    }

    // set current process to non-active
    process_active[current->pid-1] = -1;
    active_terminal[current->terminal_idx] = current->parent_pid;

    // update the terminal current process when the current process in the current terminal halts
    terms[current->terminal_idx].current_process_id = active_terminal[current->terminal_idx];
    terms[current->terminal_idx].read = 0;
    terms[current->terminal_idx].key_index =0;
    memset((uint8_t*)terms[current->terminal_idx].terminal_buf, 0, sizeof(terms[current->terminal_idx].terminal_buf));
    if(current->terminal_idx == display_terminal) {reset_keybuf();}

    process_in_use--;
    current_pid_num = current->parent_pid;

    // check if main shell
    if(current->pid == current->parent_pid ){ //&& current->pid == active_terminal[current->terminal_idx]
        active_terminal[current->terminal_idx] = -1;
        execute((uint8_t*)"shell");
    }
    // Jump to execute return with proper ebp and esp
    asm volatile(
        "movl  %2, %%esp; "
        "movl  %1, %%ebp; "
        "movl  %0, %%eax; "
        "jmp execute_return;"
        :
        :"r"(status_32), "r"(current->save_ebp), "r"(current->save_esp)
    );
    return status_32;
}
int32_t execute (const uint8_t* command){
    return execute_on_term(command, current_term_id);
}
/*
 * execute 
 * Description: Attempt to load and execute a new program,
 *      the command life is a space seperated string. The first part is the file name to 
 *      be executed, the second part will be used in checkpoint4, and this will be updated 
 *      later. 
 * Input: const uint8_t* command
 * Output: none
 * Return value: -1 on failure, 256 on exception, other for success. 
 * 
 */
int32_t execute_on_term (const uint8_t* command, int32_t term_index){ 
    cli();
    dentry_t entry;     // file entry 
    pcb_t * entry_pcb;    // the process block
    uint32_t user_code_start_addr;
    uint8_t command_buf[128]; //buff size has a maximum of 128
    uint8_t elf_buffer[ELF_MAGIC_SIZE]; // elf buffer for the magic keyword         
    int32_t ret;
    uint32_t command_len = strlen((int8_t*) command);
    int i, arg_idx;
    arg_idx = 0;


    if(term_index > 2 || term_index < 0){
        return -1;
    }

    if(process_in_use + 1 > MAX_PROCESS){   // to many
        printf("Too many processes \n");
        return -1;
    }

    memset(command_buf, 0, sizeof(command_buf));
    if(!command){
        return -1;
    }
    
    // we read the command until newline or null or space
    for(i = 0; i < command_len; ++i){
        if(command[i] == 0x0a || command[i] == 0) break;

        if(command[i] == 0x20){
            arg_idx = i+1;
            break;
        }

        command_buf[i] = command[i];
    }

    
    if(-1 == read_dentry_by_name((uint8_t*) command_buf, &entry)){ // we only consider the command is in one line for now
        // failed to find the file
        return -1;                                  
    }
    
    // read the first 4 byte of the file to see if the file has ELF
    read_data(entry.inode_num, 0, elf_buffer, ELF_MAGIC_SIZE);          
    if(strncmp((int8_t*)elf_magic,(int8_t*)elf_buffer,ELF_MAGIC_SIZE)){
        // failed to find magic ELF, the file is not executable
        return -1;                          
    }


    // all new process should start at offset 4, because the first 3 is used by the terminals
    // ++current_pid_num;
    // we mind not use the table by any chance
    // we can use process active array index as the pid
    // if the terminal is not running, get the pcb for the terminal
    // cli();
    if(active_terminal[term_index] == -1){
        for(i=0; i<6; ++i){ 
            // find available process active slots
            if(process_active[i]==-1) break;
        }
        entry_pcb = (pcb_t *)(GET_PCB(i + 1));
        entry_pcb->pid = i + 1; 
        process_active[i] = 1;  // set to active
        ++process_in_use;   // increase number of process
        entry_pcb->terminal_idx = term_index;
        entry_pcb->parent_pid = entry_pcb->pid;
    }
    else
    {
        for(i=0; i<6; ++i){
            // find available process active slots
            if(process_active[i]==-1) break;
        } 
        process_active[i] = 1;    // set to active
        ++process_in_use;         // increase the number of process
        entry_pcb = (pcb_t *)(GET_PCB(i+1));
        entry_pcb->pid = i+1;
        entry_pcb->terminal_idx=term_index;
        // the logic of this is the following, assuming that we have the a shell running in index 0
        // the current shell pid will be placed inside the active_terminal array
        // when another process comes it will set its pid first, then it will look up the pid in the 
        // array and use that as the parent
        entry_pcb->parent_pid = active_terminal[term_index];
        // active_terminal[term_index] = entry_pcb->pid;
        
    }
    
    active_terminal[term_index] = entry_pcb->pid; // save the current process number to the array
    current_pid_num = entry_pcb->pid;
    
    // setting the current_pid for the terminal
    terms[term_index].current_process_id = active_terminal[term_index];

    memset(entry_pcb->args, 0, sizeof(entry_pcb->args));
    if (arg_idx != 0) {
        // there are arguments
        for(i = arg_idx; i < strlen((int8_t*) command); ++i){
            if(command[i] == 0x0a || command[i] == 0) break;
            entry_pcb->args[i-arg_idx] = command[i];
        }
    }

    // TODO: save esp and save ebp
    asm volatile(
        "movl %%ebp, %0;"
        "movl %%esp, %1;"
        : "=r"(entry_pcb->save_ebp), "=r"(entry_pcb->save_esp)
    );
    // open stdin and stdout
    entry_pcb->fd_array[0] = set_up_stdin(); 
    entry_pcb->fd_array[1] = set_up_stdout();


    // Check for executable
    // Set up paging
    map_program_page(current_pid_num);  // set up paging, need to change later
    // flush_TLB();                        // flush tlb

    // Load file into memory
    read_data(entry.inode_num, 0, (uint8_t*)(PROG_LOAD_ADDR), FOUR_MB);
    
    // get 
    read_data(entry.inode_num, PROGRAM_ENTRY, (uint8_t*)(&user_code_start_addr), 4);
    
    // TODO: tss for context switching
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (current_pid_num -1)); // use the entry_pcb->pid, because the current pid
    tss.ss0 = KERNEL_DS;

    /* Prepare for Context Switch 
     * USER DS
     * ESP
     * EFLAG
     * CS
     * EIP
     */
    asm volatile (
        "pushl %1; "
        "pushl %2; "
        "pushfl;"
        "popl %%eax;"
        "orl $0x200, %%eax;"
        "pushl %%eax;"
        "pushl %3;"
        "pushl %4; "
        "iret;"
        "execute_return:;"
        "movl %%eax, %0;"
        :"=r"(ret)         /* output */
        :"r"(USER_DS), "r"(USER_PROG - sizeof(uint32_t)), "r"(USER_CS), "r"(user_code_start_addr) /* input */
        :"eax"          /* clobbered register*/
    );
    
    // Push IRET context to kernel stack
    return ret;
}


/* All the different function operation jump table depending on the function type */
fot_t rtc_fot = {
    .read = &rtc_read,
    .write = &rtc_write,
    .open = &rtc_open,
    .close = &rtc_close
};
fot_t file_fot = {
    .read = &file_read,
    .write = &file_write,
    .open = &file_open,
    .close = &file_close
};
fot_t dir_fot = {
    .read = &directory_read,
    .write = &directory_write,
    .open = &directory_open,
    .close = &directory_close
};
fot_t stdin_fot = {
    .read = &terminal_read,
    .write = &stdin_write,
    .open = &terminal_open,
    .close = terminal_close
};
fot_t stdout_fot = {
    .read = &stdout_read,
    .write = &terminal_write,
    .open = &terminal_open,
    .close = &terminal_close
};


/*
 * stdout_read 
 * Description: ghost function. 
 * Input: int32_t fd, void* buf, int32_t nbytes
 * Output: none
 * Return value: -1. 
 */
int32_t stdout_read(int fd,void * buf, int32_t n_bytes) {
    return -1;
}


/*
 * stdout_write 
 * Description: ghost function.
 * Input: int32_t fd, void* buf, int32_t nbytes
 * Output: none
 * Return value: -1. 
  */
int32_t stdin_write(int fd, const void * buf, int32_t n_bytes) {
    return -1;
}


/*
 * set_up_stdin 
 * Description: Set up the fd for stdin, set all the attribute of file_descriptor_t.
 * Input: none
 * Output: none
 * Return value: stdin. 
 */
/* stdin: read-only file corresponding to keyboard input */
file_descriptor_t set_up_stdin(){
    file_descriptor_t stdin;
    stdin.file_op_ptr = stdin_fot;
    stdin.file_pos = 0;
    stdin.flags = 1;
    return stdin;
}



/*
 * set_up_stdout 
 * Description: Set up the fd for stdout, set all the attribute of file_descriptor_t.
 * Input: none
 * Output: none
 * Return value: stdout. 
 */
/* stdout: write-only file corresponding to terminal */
file_descriptor_t set_up_stdout(){
    file_descriptor_t stdout;
    stdout.file_op_ptr = stdout_fot;
    stdout.file_pos = 0;
    stdout.flags = 1;
    return stdout;
}

/*
 * write 
 * Description: Writes data to the terminal or to a device (RTC). 
 * Input: int32_t fd, void* buf, int32_t nbytes
 * Output: none
 * Return value: # of bytes written on sucess, and -1 on fail. 
 */
int32_t write(int fd, const void *buf, int32_t nbytes){
    if(fd < 0 || fd >= 8) return -1;  //maximum 8 files per fd, so fd cannot be larger than 8
    if(nbytes< 0) return -1;
    if(buf == NULL) return -1;
    pcb_t* location = (pcb_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    if (location->fd_array[fd].flags == 0) return -1;   // attempt to write unopened fd
    return ((location->fd_array[fd]).file_op_ptr).write(fd, buf, nbytes);
}

/*
 * read 
 * Description: read the data from the keyboard/RTC/file/directory. 
 * Input: int32_t fd, void* buf, int32_t nbytes
 * Output: none
 * Return value: 0 on sucess, and -1 on fail. 
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    if(fd < 0 || fd >= 8) return -1; //maximum 8 files per fd, so fd cannot be larger than 8
    if(nbytes< 0) return -1;
    if(buf == NULL) return -1;
    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    if (location->fd_array[fd].flags == 0) return -1;   // attempt to read unopened fd
    return ((location->fd_array[fd]).file_op_ptr).read(fd, buf, nbytes);
}

/*
 * open 
 * Description: read the filename and find it's corresponding dentry,
 *              then allocate new unused fd for the file. 
 *              Then based on the different file type, do various settings for it.  
 * Input: filename
 * Output: none
 * Return value: 0 on sucess, and -1 on fail. 
 */

int32_t open(const uint8_t* filename){
    if(filename == NULL)return -1;
    // find a unused entry in the fda, the index is the file descriptor
    int i;
    int fd = -1;

    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    dentry_t entry;     // file entry

    // find the dentry in the boot block fo the file system base on the file name
    // Parse args ???
    if(-1 == read_dentry_by_name(filename, &entry)){ // we only consider the command is in one line for now
        // failed to find the file
        return -1;                                  
    }
    //use entry to access inode
    // set the file discriptor array entry
    for(i = 0 ; i < 8; i++){ //maximum 8 files per fd, so fd cannot be larger than 8
        if(location->fd_array[i].flags == 0){
            fd = i;
            break;
        }
    }
    if(fd == -1)return -1; //too many files open

    location->fd_array[fd].flags = 1;       // set the fda entry as used
    location->fd_array[fd].file_pos = 0;    // set the pos to the start of the file
    if(entry.file_type == 1){ //directory type = 1
        location->fd_array[fd].file_op_ptr = dir_fot;  // set file operation table pointer based on file type of the dentry
    }
    else if(entry.file_type == 2){ //file = 2
        location->fd_array[fd].file_op_ptr = file_fot;  // set file operation table pointer based on file type of the dentry
    }
    else if(entry.file_type == 0){ //RTC = 0
        location->fd_array[fd].file_op_ptr = rtc_fot;  // set file operation table pointer based on file type of the dentry
    }
    else{
        return -1;
    }
    location->fd_array[fd].inode = entry.inode_num;       // read inode number from the dentry
    location->fd_array[fd].file_op_ptr.open(filename);
    return fd;
    
} 

/*
 * close_helper
 * Description: a helper function for our close function
 * Input: fd
 * Output: none
 * Return value: other on sucess, and -1 on fail. 
 */
int32_t close_helper(int32_t fd){
    if(fd < 0 || fd >= 8){ // can't close stdin, stdout, maximum 8 files, so fd cannot be larger than 8
        return -1;
    }
    //need to implement a pcb helper function. 
    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    if (location->fd_array[fd].flags == 0) return -1;   // attempt to close unopened fd
    location->fd_array[fd].flags = 0;
    return location->fd_array[fd].file_op_ptr.close(fd);
}  

/*
 * close 
 * Description: Close the specified file discriptor, 
 *              and make it available for other open fd. 
 * Input: fd
 * Output: none
 * Return value: 0 on sucess, and -1 on fail. 
 */
int32_t close(int32_t fd){
    // can't close stdin, stdout
    if (fd == 0 || fd == 1) return -1;
    return close_helper(fd);
}  
/*
 * getargs 
 * Description: read the program's command line arguments into a user-level buffer
 * Input: uint8_t* buf, int32_t nbytes
 * Output: none
 * Return value: -1 if fail, 0 if success
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
    if (buf == NULL || nbytes < 0) return -1;
    pcb_t* location = (pcb_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    if (nbytes > FILE_NAME_LENGTH){nbytes = FILE_NAME_LENGTH;}
    if (location->args[nbytes] != 0) return -1;   // the arguments and a terminal NULL do not fit in the buffer
    if (location->args[0] == 0) return -1;        // empty arguments
    memcpy(buf, location->args, nbytes);        // get the name
    return 0; // 7
}

/*
 * vidmap 
 * Description: map the text-mode video memory into user space at a preset virtual address
 * Input: none
 * Output: 
 *      screen_start: 
 *          after the function call, screen start will contain the address of the video map
 *          address in virtual space where as this points to the video memory in actual 
 *          physical memory. 
 * Return value: none
 */
int32_t vidmap(uint8_t** screen_start){
    // null check
    if(!screen_start) return -1;
    // user program page is from prog_load_addr virtual address to the address + 4MB
    if ((uint32_t)screen_start < PROG_LOAD_ADDR || (uint32_t)screen_start > PROG_LOAD_ADDR + FOUR_MB - 4) return -1;

    map_vidmap_page((PROG_128MB << 1) + (current_term_id * FOUR_KB), current_term_id);    // loading new video page                      // flush TLB
    *screen_start = (uint8_t*)(PROG_128MB << 1) + (current_term_id * FOUR_KB);    // virtual address is 256MB double the size of PROG_LOAD_ADDR = 128MB
    
    // the vedio memory requires you to add another page mapping for the program (4kB)
    return 0; // 8
}    

/*
 * set_handler 
 * Description: 
 * Input: none
 * Output: none
 * Return value: none
 */
int32_t set_handler(int32_t signum, void*handler_address){
    return -1; // 9
}

/*
 * sigreturn 
 * Description: 
 * Input: none
 * Output: none
 * Return value: none
 */
int32_t sigreturn(void){
    return -1; // 10
}   
