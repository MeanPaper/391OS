#include "keyboard.h"
const char keyboard_ch[256] = {
	'\0', '1', '\0'
};
void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

// control, shift, caps lock
static uint32_t control_code;
void keyboard_interrupt(){
	cli();
	uint32_t scan_code = inb(KEYBOARD_PORT);
	if(scan_code == 2){
		putc(keyboard_ch[1]);
	}
	// printf("Typed: %d \n", typed_char);
	send_eoi(KEYBOARD_IRQ);
	sti();
}
