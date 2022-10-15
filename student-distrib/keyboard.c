#include "keyboard.h"

void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

void keyboard_interrupt(){
	cli();
	uint32_t typed_char = inb(KEYBOARD_PORT);
	clear();
	printf("Typed: %d \n", typed_char);
	send_eoi(KEYBOARD_IRQ);
	sti();
}
