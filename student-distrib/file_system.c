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

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    
    inode_t* target_inode;
    uint32_t inode_size;
    uint32_t blocks;
    uint32_t remaining;

    uint32_t  data_block_start_index;
    uint8_t* data_block_ptr;
    // check for invalid inode number
    if(inode > boot_block->total_inode_num || inode < 0){
        return -1;
    }

    target_inode = (inode_t*)((1 + inode) + boot_block);  // found out the data block location
    inode_size = target_inode->length;

    // check for buffer
    if(!buf || offset >= inode_size){
        return -1;
    }

    blocks = offset / BLOCK_SIZE;   // the block where the data will be store
    remaining = offset % BLOCK_SIZE; // the data index within the block, that we will start copying in bytes

    // compute the start index of data_block
    data_block_start_index = boot_block->total_inode_num + 1;
    

    // first type cast the 4kB pointer into 1B pointer base on the block index
    // and then adding the remaining bit base on the remainder of the offset to find the starting point
    // where data should be copied
    data_block_ptr = (uint8_t*)(boot_block + data_block_start_index + target_inode->content[blocks]) + remaining;
    
    memcpy(buf, data_block_ptr, length);
    
    
    return 0;
}


