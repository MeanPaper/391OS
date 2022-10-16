/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60

/* keyboard_init(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: Initialize keyboard */
extern void keyboard_init(void);

/* keyboard_interrupt(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: Handle keyboard interrupts */
extern void keyboard_interrupt(void);

#endif
