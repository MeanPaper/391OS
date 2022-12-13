#include "file_system.h"
#include "lib.h"
#include "sys_call_helper.h"

// this temporary dentry, only for checkpoint 2
dentry_t current_file;

// this temporary for now, checkpoint 2
int32_t file_counter = 0;   // keep track of the current file index


boot_block_t * boot_block; // pointer to the boot block of the file system


// initialize all the variable used by the file system
void init_file_system(uint32_t* file_system_ptr){
    boot_block = (boot_block_t*) file_system_ptr;
}



// read a dentry from the file system based on the file name
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    int i;
    dentry_t *temp;
    // sanity check for file name, dentry
    if(!fname || !dentry || !strlen((int8_t*)fname) || !strlen((int8_t*)fname) > FILE_NAME_LENGTH){
        return -1;
    }

    int total_file = boot_block->total_dentry_num;
    for(i = 0; i < total_file; ++i){
        // go the next file dentry
        temp = &(boot_block->files[i]);
        // using string compare to compare the file name and the name that it request
        // read the usage of the function, fname is need to be string 1 since it might be null terminated 
        if(!strncmp((int8_t*)fname, (int8_t*)(temp->file_name), FILE_NAME_LENGTH)){
            /* copying the entire struct */
            // destination: input dentry
            // source:  found dentry
            // size: always 32 bytes
            
            memcpy(dentry, temp, sizeof(dentry_t));
            return 0;
        }
    }
    return -1;
}




// read a dentry by the index
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    if(!dentry) return -1;

    dentry_t *temp;

    if(index >= MAX_DENTRY_NUM || !dentry){
        return -1;
    }   

    // get the dentry
    temp = &(boot_block->files[index]);

    // copy all the information to the dentry parameter
    memcpy(dentry, temp, sizeof(dentry_t));

    return 0;
}




int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    if(!buf) return -1;
    
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
    file_size = target_inode->length;                     // getting the file size

    // check offset validation and 
    if(offset > file_size){
        return -1;
    }
    // calculate the remaining data after the given offset
    remaining_file_size = file_size - offset;

    blocks = offset / BLOCK_SIZE;   // the block where the data will be store (in inode)
    remainder = offset % BLOCK_SIZE; // the data index within the block, that we will start copying in bytes

    // compute the start index of data_block
    data_block_start_index = boot_block->total_inode_num + 1;
    
    // check if it is read out of bounds
    if(length >= remaining_file_size){
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
        return remaining_file_size;
    }
    return length; // return the number of bytes copied
}



// open a directory and read the directory
int32_t directory_open(const uint8_t* file_name){
    if(!file_name) return -1;

    return read_dentry_by_index(0, &current_file);
}

// directory_close, do nothing
int32_t directory_close(int fd){
    return 0;
}

// directory_read, read a file name from the current directory
int32_t directory_read(int fd, void *buf, int32_t nbytes){
    if(!buf) return -1;
    uint32_t current_pid_num = get_current_pid();
    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    if(location->fd_array[fd].file_pos >= boot_block->total_dentry_num){
        return 0;
    }
    if (nbytes > 32) nbytes = 32;
    read_dentry_by_index(location->fd_array[fd].file_pos, &current_file);  // get the file by index  
    memcpy(buf, current_file.file_name, nbytes);        // get the name
    location->fd_array[fd].file_pos += 1;                                  // 
    return nbytes;
}

// directory_write, do nothing
int32_t directory_write(int fd, const void *buf, int32_t nbytes){
    if(!buf) return -1;
    return -1;
}

// open a file, and read the file entry by the file name, need to modify later on
int32_t file_open(const uint8_t* file_name){    
    if(!file_name) return -1;
    
    return read_dentry_by_name(file_name, &current_file);
}

// close a file
int32_t file_close(int fd){
    return 0;
}

// write to a file
int32_t file_write(int fd, const void *buf, int32_t nbytes){
    if(!buf) return -1;

    return -1;
}

// read the data from the file
// this is a simple implementation, need to modify later on
int32_t file_read(int fd, void *buf, int32_t nbytes){
    // if(!buf) return -1;
    uint32_t read_size;
 
    // inode_t * data_block_start_index = (inode_t*)(boot_block + 1 + current_file.inode_num);
    // return read_data(current_file.inode_num, 0, buf, data_block_start_index->length);
    if(fd < 0 || fd >= 8)return -1;
    if(nbytes< 0)return -1;
    if(buf == NULL) return -1;
    uint32_t current_pid_num = get_current_pid();
    uint8_t * temp = (uint8_t*)(EIGHT_MB - (EIGHT_KB * current_pid_num));
    pcb_t* location = (pcb_t*)temp;
    read_size = read_data(location->fd_array[fd].inode, location->fd_array[fd].file_pos, buf, nbytes);
    if (read_size == -1) return -1;
    location->fd_array[fd].file_pos += read_size;
    return read_size;
}
