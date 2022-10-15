void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

void keyboard_interrupt(){
	printf("Typed: %d \n", inb(KEYBOARD_PORT));
	send_eoi(KEYBOARD_IRQ);
}
