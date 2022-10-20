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
            /* copying the entire struct */
            // destination: input dentry
            // source:  found dentry
            // size: always 32 bytes
            strncpy(dentry->file_name, temp.file_name, FILE_NAME_LENGTH);
            dentry->file_type = temp.file_type;
            dentry->inode_num = temp.inode_num;
            strncpy(dentry->reserved, temp.reserved, DENTRY_RESERVED);
            return 0;
        }
    }
    return -1;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

    dentry_t temp;
    
    if(index >= 63){
        return -1;
    }   

    // get the dentry
    temp = boot_block->files[index];

    // copy all the information to the dentry parameter
    strncpy(dentry->file_name, temp.file_name, FILE_NAME_LENGTH);
    dentry->file_type = temp.file_type;
    dentry->inode_num = temp.inode_num;
    strncpy(dentry->reserved, temp.reserved, DENTRY_RESERVED);

    return 0;
}



