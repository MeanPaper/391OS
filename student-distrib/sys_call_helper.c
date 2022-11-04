#include "sys_call_helper.h"
#include "lib.h"

#define FOUR_MB   0x400000
#define EIGHT_MB   0x80000
#define PROG_LOAD_ADDR  0x08048000
#define USER_PROG 0x840000 // 132 MB
#define EIGHT_KB      8192
// #define USER_CS     0x0023
// #define USER_DS     0x002B
#define GET_PCB(n) EIGHT_MB-(n)*FOUR_MB
// #define GET_
/* Local function declaration*/

/* file operations jump table associtated with the correct file type*/
fot_t rtc_fot, file_fot, dir_fot, stdin_fot, stdout_fot;

// the magic header of elf
uint8_t elf_magic[ELF_MAGIC_SIZE] = {0x7f, 0x45, 0x4c, 0x46}; // "DEL,E,L,F"
uint32_t current_pid_num = 1;
uint32_t exception = 0;

void set_exception_flag(uint32_t num){
    exception = num;
}
 
/*
 * system_call_helper 
 * Description: output system call message and put system in loop for now
 * Input: none
 * Output: none
 * Return value: none
 * 
*/
// void system_call_helper(){
//     clear();
//     printf(" System call, reach IDT 0x80 \n");
//     while(1);  
// }
/* stdin: read-only file corresponding to keyboard input */
file_descriptor_t set_up_stdin(){
    file_descriptor_t stdin;
    stdin.file_op_ptr = &stdin_fot;
    stdin.file_pos = 0;
    stdin.flags = 1;
    return stdin;
}
/* stdout: write-only file corresponding to terminal */
file_descriptor_t set_up_stdout(){
    file_descriptor_t stdout;
    stdout.file_op_ptr = &stdout_fot;
    stdout.file_pos = 0;
    stdout.flags = 1;
    return stdout;
}

int32_t halt (uint8_t status){
    /* halt must return a value to the parent execute system call so that 
     * we know how the program ended */
    
    // uint32_t return_value

    if(exception){
        exception = 0;
        return 256;
    }
    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    pcb_t * parent = (pcb_t*)(GET_PCB(current->parent_pid)); 

    /* halting the base shell should either not let the user halt at all or 
     * after halting must re-spawn a new base shell, so there's always one 
     * program that is running
     * 
     * we can achieve this by check the pid that we are trying to halt
     * if the pid is 1(the first pid) 
     *      then we will just return */

    // Restore parent data
    

    // Restore parent paging
    map_program_page(parent->pid);
    tss.esp0 = parent->save_esp;
    
    // Close all relevant FDs
    int i;
    for(i = 0; i < FD_ARRAY_SIZE; ++i){
        if(current->fd_array[i].flags){
            close(i);
        }
    }

    // set current process to non-active
    current->active = 0;

    // check if main shell
    if(current_pid_num == 1){
        // set up some return value
    }
    
    // Jump to execute return
    // TODO: need more information here 
    asm volatile(
        "jmp execute_end"
    );
    return 0;
}

int32_t execute (const uint8_t* command){ 
    dentry_t entry;     // file entry 
    pcb_t entry_pcb;    // the process block
    uint32_t s_ebp;
    uint32_t s_esp;
    uint8_t command_buf[128];
    int i;

    
    if(!command){
        return -1;
    }
    
    // we read the command until newline or null
    for(i = 0; i < strlen((int8_t*) command); ++i)
    {
        if(command[i] == 0x0a || command[i] == 0) break;
        command_buf[i] = command[i];
    }
    
    // Parse args ???
    if(-1 == read_dentry_by_name(command_buf, &entry)){ // we only consider the command is in one line for now
        // failed to find the file
        return -1;                                  
    }
    
    uint8_t elf_buffer[ELF_MAGIC_SIZE]; // elf buffer for the magic keyword         
    
    // read the first 4 byte of the file to see if the file has ELF
    read_data(entry.inode_num, 0, elf_buffer, ELF_MAGIC_SIZE);          
    if(strncmp((int8_t*)elf_magic,(int8_t*)elf_buffer,ELF_MAGIC_SIZE)){
        // failed to find magic ELF, the file is not executable
        return -1;                          
    }

    // clean up local var
    memset(&entry_pcb, 0, sizeof(entry_pcb));
    entry_pcb.active = 1; 
    if(current_pid_num == 1){
        entry_pcb.parent_pid = 1;
    }
    else{
        entry_pcb.parent_pid = current_pid_num - 1;
    }
    entry_pcb.pid = current_pid_num++;
    // TODO: save esp and save ebp
    asm volatile(
        "movl %%ebp, %0;"
        "movl %%esp, %1;"
        : "=r"(s_ebp), "=r"(s_esp)
    );
    entry_pcb.save_ebp = s_ebp;
    entry_pcb.save_esp = s_esp;

    // open stdin and stdout
    entry_pcb.fd_array[0] = set_up_stdin(); 
    entry_pcb.fd_array[1] = set_up_stdout();

    // find out the memory address for the process control block
    // 8MB - num * 8KB
    uint8_t * pcb_target_addr = (uint8_t *)(GET_PCB(current_pid_num));
    memcpy(pcb_target_addr, &entry_pcb, sizeof(entry_pcb));

    // Check for executable
    // Set up paging
    map_program_page(current_pid_num);  // set up paging
    flush_TLB();                        // flush tlb

    // Load file into memory
    read_data(entry.inode_num, 0, (uint8_t*)(PROG_LOAD_ADDR), FOUR_MB);
    
    // get 
    uint32_t user_code_start_addr;
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
    // context_switch(user_code_start_addr); // this function is not finish
    //register uint32_t saved_ebp asm("ebp");
    asm volatile (
        "pushl %0;
         pushl %1;
         pushfl;
         popl %%eax;
         orl $0x200, %%eax;
         pushl %%eax;
         pushl %2;
         pushl %3;
         iret;
         execute_end:
         leave;
         ret;"
        :           /* output */
        :"r"(USER_DS), "r"(USER_PROG - sizeof(uint32_t)), \
         "r"(USER_CS), "r"(user_code_start_addr)         /* input */
        :          /* clobbered register*/
    );
    
    // Push IRET context to kernel stack
    return 0;
}

/**
 * file operation for RTC
 */
int32_t rtc_read_handler (int32_t fd, void* buf, int32_t nbytes){
    return rtc_read();
}

int32_t rtc_write_handler (int32_t fd, const void* buf, int32_t nbytes){
    return rtc_write(buf);
}

int32_t rtc_open_handler (const uint8_t* filename){
    int tmp = common_open(filename);
    if (tmp == 0) {
        return rtc_open();
    } else {
        return -1;
    }
}

int32_t rtc_close_handler(int32_t fd){
    common_close(fd);
    return rtc_close();
}


/**
 * file operation for regular file
 */
int32_t file_read_handler (int32_t fd, void* buf, int32_t nbytes){
    // to the end of the file OR to the end of buffer
    common_read(fd, buf, nbytes);
}

int32_t file_write_handler (int32_t fd, const void* buf, int32_t nbytes){
    return -1;  // the file system is read-only
}

int32_t file_open_handler (const uint8_t* filename){
    common_open(filename);
}

int32_t file_close_handler(int32_t fd){
    common_close(fd);
}

/**
 * file operation for directory
 */
int32_t dir_read_handler (int32_t fd, void* buf, int32_t nbytes){
    // provide the filename (as much as fits, or all 32 bytes)
    // subsequent reads should read from successive directory entries until the last is reached
    // then, repeatedly return 0
    dir_read(fd,buf,nbytes);
}

int32_t dir_write_handler (int32_t fd, const void* buf, int32_t nbytes){
    return -1;  // the file system is read-only
}

int32_t dir_open_handler (const uint8_t* filename){
    
}

int32_t dir_close_handler(int32_t fd){
    
}

/**
 * file operation for stdin
 */
int32_t stdin_read_handler (int32_t fd, void* buf, int32_t nbytes){
    // read data from the keyboard
    // return data from one line that has been terminated by pressing Enter
    // or as much as fits in the buffer from one such line
    // include the line feed character
    return terminal_read(fd, buf, n_bytes);
}

/**
 * file operation for stdout
 */
int32_t stdout_write_handler (int32_t fd, const void* buf, int32_t nbytes){
    // write data to terminal, display to the screen immediately
    return terminal_write(fd, buf, n_bytes);
}

rtc_fot = {
    .read = &rtc_read_handler,
    .write = &rtc_write_handler,
    .open = &rtc_open_handler,
    .close = &rtc_close_handler
};
file_fot = {
    .read = &file_read_handler,
    .write = &file_write_handler,
    .open = &file_open_handler,
    .close = &file_close_handler
};
dir_fot = {
    .read = &dir_read_handler,
    .write = &dir_write_handler,
    .open = &dir_open_handler,
    .close = &dir_close_handler
};
stdin_fot = {
    .read = &dir_read_handler,
    .write = NULL,
    .open = NULL,
    .close = NULL
};
stdout_fot = {
    .read = NULL,
    .write = &dir_write_handler,
    .open = NULL,
    .close = NULL
};

int32_t common_read(int32_t fd, void* buf, int32_t nbytes){
    // for normal files and the directory
    //  If the initial file position is at or beyond the end of file, return -1
    if(fd < 0 || fd >= 8)return -1;
    if(nbytes< 0)return -1;
    uint8_t * temp = (EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    dentry_t entry;     // file entry

    // find the dentry in the boot block fo the file system base on the file name
    // Parse args ???
    // if(entry->file_type == 2){ //file
        
    // }
    return read_data(location->fd_array[fd].inode,location->fd_array[fd].file_pos,buf,nbytes);
}

int32_t open(const uint8_t* filename){
    if(filename == NULL)return -1;
    // find a unused entry in the fda, the index is the file descriptor
    int i;
    int fd = -1;

    uint8_t * temp = (EIGHT_MB - (EIGHT_KB * current_pid_num));
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
    for(i = 0 ; i < 8; i++){
        if(location->fd_array[fd].flags == 0){
            fd = i;
            break;
        }
    }
    if(fd == -1)return -1; //too many files open

    location->fd_array[fd].flags = 1;       // set the fda entry as used
    location->fd_array[fd].file_pos = 0;    // set the pos to the start of the file
    if(entry.file_name == 1){ //directory
        location->fd_array[fd].file_op_ptr = &dir_fot;  // set file operation table pointer based on file type of the dentry
    }
    else if(entry.file_type == 2){ //file
        location->fd_array[fd].file_op_ptr = &file_fot;  // set file operation table pointer based on file type of the dentry
    }
    else if(entry.file_type == 0){ //RTC
        location->fd_array[fd].file_op_ptr = &rtc_fot;  // set file operation table pointer based on file type of the dentry
    }
    else{
        return -1;
    }
    location->fd_array[fd].inode = entry.inode_num;       // read inode number from the dentry
    location->fd_array[fd].file_op_ptr->open(filename);
    return 0;
} 

int32_t common_close(int32_t fd){
    if(fd < 0 || fd >= 8){
        return -1;
    }
    //need to implement a pcb helper function. 
    uint8_t * temp = (EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    location->fd_array[fd].flags = 0;
    return 0;
}    
