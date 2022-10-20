#include "file_system.h"
#include "lib.h"




boot_block_t * boot_block;
void init_file_system(uint32_t* file_system_ptr){
    boot_block = (boot_block_t*) file_system_ptr;
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int i;
    dentry_t temp;
    
    for(i = 0; i < boot_block->total_dentry_num - 1; ++i){
        // go the next file dentry
        temp = boot_block->files[i];


        // using string compare to compare the file name and the name that it request
        // read the usage of the function, fname is need to be string 1 since it might be null terminated 
        if(!strncmp(fname, temp.file_name, strlen(fname))){
        
        }
    }
    return -1;
}




