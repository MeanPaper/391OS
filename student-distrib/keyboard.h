/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
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
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8
#define CONTROL_PRESSED 0x1D
#define CONTROL_RELEASED 0x9D
#define ENTER_RELEASE 0x9C
static uint8_t shift_pressed_cons = 0;
static uint8_t caps_pressed_cons = 0;
static uint8_t alt_pressed_cons = 0;
static uint8_t control_pressed_cons = 0;
volatile uint8_t key_buffer[128];
static uint8_t buffer_index = 0;
volatile uint8_t ENTER_PRESSED = 0;

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

void handle_enter();

#endif
