#include "keyboard.h"
const char keyboard_ch[59] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};

void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

// control, shift, caps lock
static uint32_t control_code;
void keyboard_interrupt(){
	cli();
	uint32_t scan_code = inb(KEYBOARD_PORT);
	if (scan_code>=0 && scan_code<59)
		putc(keyboard_ch[scan_code]);
	// printf("Typed: %d \n", keyboard_ch[scan_code]);
	send_eoi(KEYBOARD_IRQ);
	sti();
}
