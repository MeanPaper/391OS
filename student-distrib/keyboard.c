static uint8_t scancode_hash[59] = 	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'};
	 
void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

void keyboard_interrupt(){
	uint8_t idx = inb(KEYBOARD_PORT)
	printf("Typed: %d \n", scancode_hash[idx]);
	send_eoi(KEYBOARD_IRQ);
}
