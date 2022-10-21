/* keyboard.h - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"

/* Mapping scancode to ascii */
const char keyboard_ch[59] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};

/* Initialize keyboard */
void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

// control, shift, caps lock
// static uint32_t control_code;

/* Handle keyboard interrupt */
void keyboard_interrupt(){
	cli();
	uint32_t scan_code = inb(KEYBOARD_PORT);
	if (scan_code>=0 && scan_code<59)
		putc(keyboard_ch[scan_code]);
	send_eoi(KEYBOARD_IRQ);
	sti();
}
