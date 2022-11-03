#ifndef _SYS_CALL_HELPER_H
#define _SYS_CALL_HELPER_H

#include "types.h"
#include "file_system.h"
#include "terminal.h"
#include "rtc.h"
#include "lib.h"

#define FD_ARRAY_SIZE       8
#define PROGRAM_ENTRY       24      // program eip starts at byte 24
#define ENTRY_ADDR_SIZE     4       // program eip range [24,27]
#define ELF_MAGIC_SIZE      4



typedef struct file_descriptor{
    uint32_t * file_op_ptr;  /* pointer to a struct of function pointer 
                              * to this specific file type */
    uint32_t inode;     /* this hold the index of inode for this file */
    uint32_t* file_pos; /* the current byte you are reading within the file */
    uint32_t flags;     /* the flag indicates whether this file is in 
                         * use or closed. Current implementation is that
                         * in use: flags != 0
                         * closed: flags = 0    */
}file_descriptor_t;

typedef struct pcb{
    uint32_t pid;                   // process id 
    uint32_t parent_pid;            // or uint32_t* paren_pcb 
    // uint32_t * process_addr;
    file_descriptor_t[FD_ARRAY_SIZE]; // file descriptor array for the current process
}pcb_t;





// handle system call for checkpoint 1
extern void system_call_helper();


// return 0 to 255 if system call run halt, failed return -1 (command cannot be execute)
// return 256 if program dies by an exception
extern int32_t halt(uint8_t status);      // 1

extern int32_t execute(const uint8_t * command);   // 2
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);      // 3
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);     // 4
extern int32_t open(const uint8_t* filename);      // 5
extern int32_t close(int32_t fd);     // 6


// for checkpoint 4
extern int32_t getargs(uint8_t* buf, int32_t nbytes);   // 7
extern int32_t vidmap(uint8_t** screen_start);    // 8
extern int32_t set_handler(int32_t signum,void*handler_address); // 9
extern int32_t sigreturn(void);   // 10
// file operations table
typedef struct Fod {
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
} fod;

fod rtc_fod {
    .read = ,
    .write = ,
    .open = ,
    .close =
}
#endif