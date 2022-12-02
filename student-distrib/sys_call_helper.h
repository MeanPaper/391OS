#ifndef _SYS_CALL_HELPER_H
#define _SYS_CALL_HELPER_H

#include "types.h"
#include "file_system.h"
#include "terminal.h"
#include "rtc.h"
#include "lib.h"
#include "paging.h"
#include "system_call.h"
#include "x86_desc.h"

#define FD_ARRAY_SIZE       8
#define PROGRAM_ENTRY       24      // program eip starts at byte 24
#define PROGRAM_ENTRY_END   27
#define ENTRY_ADDR_SIZE     4       // program eip range [24,27]
#define ELF_MAGIC_SIZE      4

#define FOUR_MB         0x400000
#define EIGHT_MB        0x800000
#define PROG_LOAD_ADDR  0x08048000
#define PROG_128MB      0x08000000
#define USER_PROG       0x8400000 // 132 MB
#define EIGHT_KB        8192

#define VIDEO_VIR       0xB8000
// #define USER_CS     0x0023
// #define USER_DS     0x002B
#define GET_PCB(n)  EIGHT_MB-(n)*EIGHT_KB

extern uint32_t get_current_pid();
extern int32_t active_terminal[3];
/* file operations jump table */
typedef struct Fot {
    int32_t (*read)(int32_t, void*, int32_t);
    int32_t (*write)(int32_t, const void*, int32_t);
    int32_t (*open)(const uint8_t*);    // type-specific initialization
    int32_t (*close)(int32_t);
} fot_t;

typedef struct file_descriptor{
    fot_t file_op_ptr;     /* pointer to a struct of function pointer 
                             * to this specific file type */
    uint32_t inode;     /* this hold the index of inode for this file */
    uint32_t file_pos; /* the current byte you are reading within the file */
    uint32_t flags;     /* the flag indicates whether this file is in 
                         * use or closed. Current implementation is that
                         * in use: flags != 0
                         * closed: flags = 0    */
}file_descriptor_t;

typedef struct pcb{
    uint32_t pid;                   // process id 
    uint32_t parent_pid;            // or uint32_t* paren_pcb 
    uint32_t save_ebp;
    uint32_t save_esp;
    uint32_t sched_ebp;
    uint32_t sched_esp;
    uint8_t args[128];
    int32_t terminal_idx;
    file_descriptor_t fd_array[FD_ARRAY_SIZE]; // file descriptor array for the current process
}pcb_t;

uint32_t current_pid_num;
// handle system call for checkpoint 1
extern void system_call_helper();
extern int get_availiable_pid();
extern uint32_t get_process_total();
extern void set_exception_flag(uint32_t num);

// return 0 to 255 if system call run halt, failed return -1 (command cannot be execute)
// return 256 if program dies by an exception
extern int32_t halt(uint8_t status);      // 1

extern int32_t execute(const uint8_t * command);   // 2
/* read
 * Description: a generic handler that call into a file-type-specific function
                using fop jump table
 * Input: int32_t fd, void* buf, int32_t nbytes
 * Return value: the number of byte read
 */
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);      // 3
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);     // 4
/* open
 * Description: provides access to the file system
 * Input: const uint8_t* filename
 * Return value: -1 if fail
 */
extern int32_t open(const uint8_t* filename);      // 5
extern int32_t close(int32_t fd);     // 6


// for checkpoint 4
extern int32_t getargs(uint8_t* buf, int32_t nbytes);   // 7
extern int32_t vidmap(uint8_t** screen_start);    // 8
extern int32_t set_handler(int32_t signum,void*handler_address); // 9
extern int32_t sigreturn(void);   // 10
extern int32_t execute_on_term(const uint8_t * command, int32_t term_idx);
#endif
