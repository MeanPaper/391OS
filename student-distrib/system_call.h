#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H
#include "sys_call_helper.h"

#define LAST_SYS_CALL 10
#define FIRST_SYS_CALL 1


//system call function
extern int32_t system_call();
extern void flush_TLB();
extern void context_switch(uint32_t entry_start_addr);

#endif