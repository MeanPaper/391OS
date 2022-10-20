#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "types.h"

// #define DENTRY_SIZE 64
// #define DENTRY_OFFSET (DENTRY_SIZE/4)   // 4 byte pointer + 1, addr increase by 4 address
// #define BLOCK_SIZE  4096
// #define FOUR_BYTE_COUNT 1024

#define INODE_ARRAY_SIZE 1023
#define FILE_NAME_LENGTH 32
#define DENTRY_RESERVED 24
#define MAX_DENTRY_NUM  63


typedef struct dentry{
    uint8_t file_name[FILE_NAME_LENGTH];  // file name is 32 byte
    uint32_t file_type;     // 4 byte
    uint32_t inode_num;     // 4 byte
    uint8_t reserved[DENTRY_RESERVED];
}dentry_t;



typedef struct inode
{ 
    uint32_t length;
    uint32_t content[INODE_ARRAY_SIZE];
}inode_t;

typedef struct boot_block
{   
    uint32_t    total_dentry_num;
    uint32_t    total_inode_num;
    uint32_t    total_data_block_num;
    uint8_t     reserved[52];
    dentry_t    files[63];  // index 0 of this array is file "."
}boot_block_t;

extern void init_file_system(uint32_t* file_system_ptr);



extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern void dentry_read();

extern void file_read();


#endif