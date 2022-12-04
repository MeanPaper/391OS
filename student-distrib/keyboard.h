/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60
#define LEFT_SHIFT_PRESSED 0x2A
#define RIGHT_SHIFT_PRESSED 0x36
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_RELEASEED 0xB6
#define CAPS_PRESSED 0x3A
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LEFT_ALT_PRESSED 0x38
#define LEFT_ALT_RELEASED 0xB8
#define LEFT_CONTROL_PRESSED 0x1D
#define LEFT_CONTROL_RELEASED 0x9D
#define ENTER_RELEASE 0x9C
#define TAB_PRESSED 0x0F
#define F1_pressed 0x3B
#define F2_pressed 0x3C
#define F3_pressed 0x3D
//status flag.
volatile uint8_t key_buffer[128];
// volatile uint8_t ENTER_PRESSED;

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
void display_on_screen(uint32_t scan_code);

void append_to_buffer(uint8_t keyword);
void handle_backspace();
void reset_keybuf();
void handle_enter();
void handle_tab();
#endif
