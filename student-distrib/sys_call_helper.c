#include "sys_call_helper.h"
#include "lib.h"


// #define GET_
/* Local function declaration*/

/* file operations jump table associtated with the correct file type*/
// fot_t rtc_fot, file_fot, dir_fot, stdin_fot, stdout_fot;

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

int32_t halt (uint8_t status){
    /* halt must return a value to the parent execute system call so that 
     * we know how the program ended */
    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    pcb_t * parent = (pcb_t*)(GET_PCB(current->parent_pid));
    
    // uint32_t return_value
    if(exception){
        asm volatile(
            "movl  %2, %%esp; "
            "movl  %1, %%ebp; "
            "movl  %0, %%eax; "
            "jmp execute_end; "
            :
            :"r"(256), "r"(parent->save_ebp), "r"(parent->save_esp)
            :"eax", "ebp", "esp"
            );
    }
    
    /* halting the base shell should either not let the user halt at all or 
     * after halting must re-spawn a new base shell, so there's always one 
     * program that is running
     * 
     * we can achieve this by check the pid that we are trying to halt
     * if the pid is 1(the first pid) 
     *      then we will just return */
    remove_program_page(current_pid_num);
    // Restore parent data
    

    // Restore parent paging
    map_program_page(parent->pid);
    tss.esp0 = parent->save_esp;
    
    // Close all relevant FDs
    int i;
    for(i = 0; i < FD_ARRAY_SIZE; ++i){
        if(current->fd_array[i].flags){
            close(i);
            current->fd_array[i].flags = 0;
        }
    }

    // set current process to non-active
    current->active = 0;

    // check if main shell
    if(current->parent_pid == parent->pid && current_pid_num == 1){
        execute((uint8_t*)"shell");
    }
    else{
        current_pid_num -= 1;
    }
    
    // Jump to execute return
    // TODO: need more information here 
    asm volatile(
        "movl  %2, %%esp; "
        "movl  %1, %%ebp; "
        "movl  %0, %%eax; "
        "jmp execute_end; "
        :
        :"r"((int32_t)status), "r"(parent->save_ebp), "r"(parent->save_esp)
        :"eax", "ebp", "esp"
    );
    return status;
}

int32_t execute (const uint8_t* command){ 
    dentry_t entry;     // file entry 
    pcb_t * entry_pcb;    // the process block
    // uint32_t s_ebp;
    // uint32_t s_esp;
    uint8_t command_buf[128];
    int i;

    memset(command_buf, 0, sizeof(command_buf));
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
    ++current_pid_num;
    entry_pcb = (pcb_t *)(GET_PCB(current_pid_num));
    memset(entry_pcb, 0, sizeof(pcb_t));
    // memcpy(pcb_target_addr, &entry_pcb, sizeof(entry_pcb));
    // clean up local var
    // memset(entry_pcb, 0, sizeof(entry_pcb));
    entry_pcb->pid = current_pid_num;
    entry_pcb->active = 1; 
    if(current_pid_num == 1){
        entry_pcb->parent_pid = 1;
    }
    else{
        entry_pcb->parent_pid = current_pid_num - 1;
    }
   
    // TODO: save esp and save ebp
    asm volatile(
        "movl %%ebp, %0;"
        "movl %%esp, %1;"
        : "=r"(entry_pcb->save_ebp), "=r"(entry_pcb->save_esp)
    );
    // entry_pcb->save_ebp = s_ebp;
    // entry_pcb->save_esp = s_esp;

    // open stdin and stdout
    entry_pcb->fd_array[0] = set_up_stdin(); 
    entry_pcb->fd_array[1] = set_up_stdout();

    // find out the memory address for the process control block
    // 8MB - num * 8KB


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
        "mov $0x2B, %%ax;"
        "mov %%ax, %%ds;"
        "pushl %0; "
        "pushl %1; "
        "pushfl;"
        "popl %%eax;"
        "orl $0x200, %%eax;"
        "pushl %%eax;"
        "pushl %2;"
        "pushl %3; "
        "iret;"
        "execute_end:;"
        "leave;"
        "ret;"
        :           /* output */
        :"r"(USER_DS), "r"(USER_PROG - sizeof(uint32_t)), "r"(USER_CS), "r"(user_code_start_addr) /* input */
        :"eax"          /* clobbered register*/
    );
    
    // Push IRET context to kernel stack
    return 0;
}

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
    .write = NULL,
    .open = &terminal_open,
    .close = terminal_close
};
fot_t stdout_fot = {
    .read = NULL,
    .write = &terminal_write,
    .open = &terminal_open,
    .close = &terminal_close
};

/* stdin: read-only file corresponding to keyboard input */
file_descriptor_t set_up_stdin(){
    file_descriptor_t stdin;
    stdin.file_op_ptr = stdin_fot;
    stdin.file_pos = 0;
    stdin.flags = 1;
    return stdin;
}
/* stdout: write-only file corresponding to terminal */
file_descriptor_t set_up_stdout(){
    file_descriptor_t stdout;
    stdout.file_op_ptr = stdout_fot;
    stdout.file_pos = 0;
    stdout.flags = 1;
    return stdout;
}

int32_t write(int fd, const void *buf, int32_t nbytes){
    if(fd < 0 || fd >= 8) return -1;
    if(nbytes< 0) return -1;
    if(buf == NULL) return -1;
    pcb_t* location = (pcb_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    return ((location->fd_array[fd]).file_op_ptr).write(fd, buf, nbytes);
}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
    if(fd < 0 || fd >= 8) return -1;
    if(nbytes< 0) return -1;
    if(buf == NULL) return -1;
    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    return ((location->fd_array[fd]).file_op_ptr).read(fd, buf, nbytes);
}

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
    for(i = 0 ; i < 8; i++){
        if(location->fd_array[i].flags == 0){
            fd = i;
            break;
        }
    }
    if(fd == -1)return -1; //too many files open

    location->fd_array[fd].flags = 1;       // set the fda entry as used
    location->fd_array[fd].file_pos = 0;    // set the pos to the start of the file
    if(entry.file_type == 1){ //directory
        location->fd_array[fd].file_op_ptr = dir_fot;  // set file operation table pointer based on file type of the dentry
    }
    else if(entry.file_type == 2){ //file
        location->fd_array[fd].file_op_ptr = file_fot;  // set file operation table pointer based on file type of the dentry
    }
    else if(entry.file_type == 0){ //RTC
        location->fd_array[fd].file_op_ptr = rtc_fot;  // set file operation table pointer based on file type of the dentry
    }
    else{
        return -1;
    }
    location->fd_array[fd].inode = entry.inode_num;       // read inode number from the dentry
    location->fd_array[fd].file_op_ptr.open(filename);
    return fd;
    
} 

int32_t close(int32_t fd){
    if(fd < 0 || fd >= 8){ // can't close stdin, stdout
        return -1;
    }
    //need to implement a pcb helper function. 
    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    location->fd_array[fd].flags = 0;
    return location->fd_array[fd].file_op_ptr.close(fd);
}    


int32_t getargs(uint8_t* buf, int32_t nbytes){
    return -1; // 7
}
int32_t vidmap(uint8_t** screen_start){
    return -1; // 8
}    
int32_t set_handler(int32_t signum, void*handler_address){
    return -1; // 9
}

int32_t sigreturn(void){
    return -1; // 10
}   
