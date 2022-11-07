#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "i8259.h"
#include "rtc.h"
#include "keyboard.h"

// rtc interrupt call function
extern void rtc_interrupt_call();

// keyboard interrupt call function
extern void keyboard_intr_call();
#endif 
