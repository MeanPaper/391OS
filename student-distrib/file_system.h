#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "types.h"


typedef struct dentry{
    uint8_t file_name[32];  // file name is 32 byte
    uint32_t file_type;     // 4 byte
    uint32_t inode_num;     // 4 byte
    uint8_t reserved[24];   // reserve 24 byte
}dentry_t;


// typedef struct inode
// {   // index 0 of the content: the size of the file = sum(number of bytes used in the data blocks)
//     // index > 0: the data block index number
//     uint32_t content[1024];
// }inode_t;

typedef struct boot_block
{   
    uint32_t    total_dentry_num;
    uint32_t    total_inode_num;
    uint32_t    total_data_block_num;
    uint8_t     reserved[52];
    dentry_t    files[63];  // index 0 of this array is file "."
}boot_block_t;

extern void init_file_system(uint32_t* file_system_ptr);

// open file system
extern int32_t open();

// close file system
extern int32_t close();

// read from file system
extern int32_t read();

extern int32_t write();

extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


#endif