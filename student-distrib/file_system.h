#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "types.h"

// #define DENTRY_SIZE 64
// #define DENTRY_OFFSET (DENTRY_SIZE/4)   // 4 byte pointer + 1, addr increase by 4 address

// #define FOUR_BYTE_COUNT 1024

#define BLOCK_SIZE  4096
#define INODE_ARRAY_SIZE 1023
#define FILE_NAME_LENGTH 32
#define DENTRY_RESERVED 24
#define MAX_DENTRY_NUM  63

/* define the struct for Directory Entries */
typedef struct dentry{
    uint8_t file_name[FILE_NAME_LENGTH];  // file name is 32 byte
    uint32_t file_type;     // 4 byte
    uint32_t inode_num;     // 4 byte
    uint8_t reserved[DENTRY_RESERVED];
}dentry_t;


/* define the struct for index nodes (inode) */
typedef struct inode
{ 
    uint32_t length;            // 4B for the total length of bytes
    uint32_t content[INODE_ARRAY_SIZE]; // the first 4B block is used to specify the length in B 
    /* content:
     * each index of content contains a data block # 
     * this could be sparse, ex. 9, 13, 37
     * However, in the eye of the file, they are continous, it will be read continously
     */
}inode_t;

/* define the struck for boot block */
typedef struct boot_block
{   
    uint32_t    total_dentry_num;       // 4B for total directory entry number
    uint32_t    total_inode_num;        // 4B for total index nodes number
    uint32_t    total_data_block_num;   // 4B for total data block number
    uint8_t     reserved[52];
    dentry_t    files[63];  // index 0 of this array is file "."
                            // the rest is each file directory
}boot_block_t;


/* void init_file_system(uint32_t* file_system_ptr);
 * Inputs: 
 *      uint32_t* file_system_ptr: pointer to a section of memory
 * Return Value: none
 * Function: initialize the file system with the boot block */
extern void init_file_system(uint32_t* file_system_ptr);




/* int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
 * Inputs: 
 *      const uint8_t* fname: file name that we need to match
 *      dentry_t* dentry: the dentry ptr that we want to copy to
 * Outputs:
 *      dentry_t* dentry: should have the copy of the file now
 * Return Value:
 *      0: Success
 *      -1: Failed
 * Function: 
 *      Find the correct directory entry by its file name and return
 *      the copy of it.                                                 */
extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);




/* int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
 * Inputs: 
 *      uint32_t index: file index that we need to match
 *      dentry_t* dentry: the dentry ptr that we want to copy to
 * Outputs:
 *      dentry_t* dentry: should have the copy of the file now
 * Return Value:
 *      0: Success
 *      -1: Failed
 * Function: 
 *      Find the correct directory entry by its index and return
 *      the copy of it.                                                 */
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);




/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 * Inputs: 
 *      uint32_t inode: 
 *      uint32_t offset: 
 *      uint8_t* buf:
 *      uint32_t length:
 * Outputs:
 *      dentry_t* dentry: should have the copy of the file now
 * Return Value:
 *      0: Success
 *      -1: Failed
 * Function: 
 *                                                       */
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);




/* void dentry_read();
 * Inputs: none
 * Return Value: none
 * Function: read from directory entry */
extern int32_t directory_read(int fd, void *buf, uint32_t nbytes);
extern int32_t directory_write(int fd, void *buf, uint32_t nbytes);




/* void file_read();
 * Inputs: none
 * Return Value: none
 * Function: read from file */
extern int32_t file_read(int fd, void *buf, uint32_t nbytes);
extern int32_t file_write(int fd, void *buf, uint32_t nbytes);



#endif