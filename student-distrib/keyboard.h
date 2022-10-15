#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60

extern void keyboard_init();

extern void keyboard_interrupt();

#endif
