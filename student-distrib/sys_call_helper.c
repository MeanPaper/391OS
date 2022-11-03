#include "sys_call_helper.h"
#include "lib.h"


// the magic header of elf
uint8_t elf_magic[ELF_MAGIC_SIZE] = {0x7f, 0x45, 0x4c, 0x46}; // "DEL,E,L,F"


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
    // Restore parent data
    // Restore parent paging
    // Close any relevant FDs
    // Jump to execute return
    return 0;
}




int32_t execute (const uint8_t* command){
    dentry_t entry;     // file entry 
    pcb_t entry_pcb;    // the process block
    
    // Parse args
    if(-1 == read_dentry_by_name(command, &entry)){ // we only consider the command is in one line for now
        // failed to find the file
        return -1;                                  
    }
    
    uint8_t elf_buffer[ELF_MAGIC_SIZE];         

    // read the first 4 byte of the file to see if the file has ELF
    read_data(entry.inode_num, 0, elf_buffer, ELF_MAGIC_SIZE);          
    if(strncmp((int8_t*)elf_magic,(int8_t*)elf_buffer,ELF_MAGIC_SIZE)){
        // failed to find magic ELF, the file is not executable
        return -1;                          
    }


    // Check for executable
    // Set up paging
    // Losd file into memory
    // Create PCB
    // Prepare for Context Switch
    // Push IRET context to kernel stack
    return 0;
}




int32_t read(int32_t fd, void* buf, int32_t nbytes){
    
}




int32_t write(int32_t fd, const void* buf, int32_t nbytes){

}   




int32_t open(const uint8_t* filename){

} 




int32_t close(int32_t fd){

}    
