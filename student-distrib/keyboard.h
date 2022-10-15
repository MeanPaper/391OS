#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ 1;
#define KEYBOARD_PORT 0x60;
void keyboard_init();

void keyboard_interrupt();