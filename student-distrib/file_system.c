#include "file_system.h"
#include "lib.h"

dentry_t current_file;

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

// read by ls command
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
    uint32_t file_size;
    uint32_t blocks;
    uint32_t remainder;
    uint32_t remaining_file_size;

    // variable for storing the data block index
    uint32_t  data_block_start_index;
    uint8_t* data_block_ptr;

    // check for invalid inode number
    if(inode > boot_block->total_inode_num || inode < 0 || !buf){
        return -1;
    }

    target_inode = (inode_t*)((1 + inode) + boot_block);  // found out the data block location
    file_size = target_inode->length;

    // check offset validation and 
    if(offset >= file_size){
        return -1;
    }
    // calculate the remaining data after the given offset
    remaining_file_size = file_size - offset;

    blocks = offset / BLOCK_SIZE;   // the block where the data will be store (in inode)
    remainder = offset % BLOCK_SIZE; // the data index within the block, that we will start copying in bytes

    // compute the start index of data_block
    data_block_start_index = boot_block->total_inode_num + 1;
    
    // check if it is read out of bounds
    if(length > remaining_file_size){
        length = remaining_file_size;
    }

    // first type cast the 4kB pointer into 1B pointer base on the block index
    // and then adding the remaining bit base on the remainder of the offset to find the starting point
    // where data should be copied
    // data_block_ptr = (uint8_t*)(boot_block + data_block_start_index + target_inode->content[blocks]) + remainder;
    int i;
    for(i = 0; i < length; ++i){
        if(remainder >= BLOCK_SIZE){
            remainder = 0;
            blocks += 1;
        }
        data_block_ptr = (uint8_t*)(boot_block + data_block_start_index + target_inode->content[blocks]) + remainder;
        buf[i] = (*data_block_ptr);
        
        // ++data_block_ptr;
        ++remainder;
    }

    // if the length reach the remaining portion of the file
    // then end of file would be read and then return 
    if(length == remaining_file_size){ // return the end of file, if the end of file is reach
        return 0;
    }
    return length; // return the number of bytes copied
}

int32_t directory_open(const char* file_name){
    return read_dentry_by_name(file_name, &current_file);
}

int32_t directory_close(const char* file_name){
    return 0;
}

int32_t directory_read(int fd, void *buf, uint32_t nbytes){
}


int32_t directory_write(int fd, void *buf, uint32_t nbytes){
    return -1;
}


int32_t file_read(int fd, void *buf, uint32_t nbytes){
    dentry_t temp_dir_entry;
    return fd;
}


