#ifndef _SYS_CALL_HELPER_H
#define _SYS_CALL_HELPER_H

#include "types.h"

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

#endif