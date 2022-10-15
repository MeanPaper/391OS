#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "i8259.h"

void system_call_helper();

// temp system call function
extern void system_call();

// rtc interrupt call function
extern void rtc_interrupt_call();

#endif 

