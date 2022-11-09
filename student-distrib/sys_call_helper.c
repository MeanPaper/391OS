#include "sys_call_helper.h"
#include "lib.h"

// the magic header of elf
uint8_t elf_magic[ELF_MAGIC_SIZE] = {0x7f, 0x45, 0x4c, 0x46}; // "DEL,E,L,F"
uint32_t current_pid_num = 0;
uint32_t exception = 0;

void set_exception_flag(uint32_t num){
    exception = num;
}

uint32_t get_current_pid() {
    return current_pid_num;
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
int32_t halt (uint8_t status){
    /* halt must return a value to the parent execute system call so that 
     * we know how the program ended */
    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    pcb_t * parent = (pcb_t*)(GET_PCB(current->parent_pid));
    int32_t status_32 = (int32_t)status;
    if(exception) status_32 = 256; //if exception, status is 256
    exception = 0;

    // Restore parent paging
    map_program_page(parent->pid);
    flush_TLB();
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (parent->pid -1));
    
    // Close all relevant FDs
    int i;
    for(i = 0; i < FD_ARRAY_SIZE; ++i){
        if(current->fd_array[i].flags){
            close_helper(i);
            current->fd_array[i].flags = 0;
        }
    }

    // set current process to non-active
    current->active = 0;

    // check if main shell
    if(current->pid == current->parent_pid && current_pid_num == 1){
        current_pid_num -= 1;
        execute((uint8_t*)"shell");
    }
    else{
        current_pid_num -= 1;
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
int32_t execute (const uint8_t* command){ 
    dentry_t entry;     // file entry 
    pcb_t * entry_pcb;    // the process block
    // uint32_t s_ebp;
    // uint32_t s_esp;
    uint32_t user_code_start_addr;
    uint8_t command_buf[128]; //buff size has a maximum of 128
    uint8_t elf_buffer[ELF_MAGIC_SIZE]; // elf buffer for the magic keyword         
    int32_t ret;
    int i, arg_idx;
    arg_idx = 0;

    memset(command_buf, 0, sizeof(command_buf));
    if(!command){
        return -1;
    }
    
    // we read the command until newline or null or space
    for(i = 0; i < strlen((int8_t*) command); ++i){
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
    ++current_pid_num;
    entry_pcb = (pcb_t *)(GET_PCB(current_pid_num));
    entry_pcb->pid = current_pid_num;
    entry_pcb->active = 1; 
    if(current_pid_num == 1){
        entry_pcb->parent_pid = 1;
    }
    else{
        entry_pcb->parent_pid = current_pid_num - 1;
    }

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
    map_program_page(current_pid_num);  // set up paging
    flush_TLB();                        // flush tlb

    // Load file into memory
    read_data(entry.inode_num, 0, (uint8_t*)(PROG_LOAD_ADDR), FOUR_MB);
    
    // get 
    read_data(entry.inode_num, PROGRAM_ENTRY, (uint8_t*)(&user_code_start_addr), 4);
    
    // TODO: tss for context switching
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (current_pid_num -1));
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
 * Input: none
 * Output: none
 * Return value: none
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
    if (buf == NULL || nbytes < 0) return -1;
    pcb_t* location = (pcb_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    if (location->args[nbytes] != 0) return -1;   // the arguments and a terminal NULL do not fit in the buffer
    if (location->args[0] == 0) return -1;        // empty arguments
    memcpy(buf, location->args, nbytes);        // get the name
    return 0; // 7
}

/*
 * vidmap 
 * Description: 
 * Input: none
 * Output: none
 * Return value: none
 */
int32_t vidmap(uint8_t** screen_start){
    return -1; // 8
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
