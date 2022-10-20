#include "file_system.h"

#define DENTRY_SIZE 64
#define BLOCK_SIZE  4096
#define FOUR_BYTE_COUNT 1024

uint32_t * file_system_addr;    // the addr of the file system
uint32_t total_dentry_num;      // total number of dentry block, the offset is pointer + 0
uint32_t total_inode_num;       // total number of inodes, the offset is pointer + 1
uint32_t total_data_block_num;  // total number of data block, the offset is pointer + 2

void init_file_system(uint32_t* file_system_ptr){
    file_system_addr = file_system_ptr;
    total_dentry_num = *(file_system_ptr);
    total_inode_num = *(file_system_ptr + 1);
    total_data_block_num = *(file_system_ptr + 2);
}
