/* keyboard.h - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
static uint8_t shift_pressed_cons = 0;
static uint8_t caps_pressed_cons = 0;
static uint8_t alt_pressed_cons = 0;
static uint8_t control_pressed_cons = 0;
volatile uint8_t key_buffer[127];
static uint8_t buffer_index = 0;
/* Mapping scancode to ascii */
const char keyboard_ch[4][59] = {{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'},
	// no caps / shift
	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V', 
	 'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'},
	// caps / no shift
	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V', 
	 'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'},
	// caps / shift
	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'}};

/* Initialize keyboard */
void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
}

// control, shift, caps lock
// static uint32_t control_code;

/* Handle keyboard interrupt */

void keyboard_interrupt(){
	cli();
	uint32_t scan_code;
	
	scan_code = inb(KEYBOARD_PORT);
			
		
	

	switch(scan_code){
		case LEFT_SHIFT_PRESSED:
		case RIGHT_SHIFT_PRESSED:
			shift_pressed_cons = 1;
			break;
		case LEFT_SHIFT_RELEASED:
		case RIGHT_SHIFT_RELEASEED:
			shift_pressed_cons = 0;
			break;
		case CAPS_PRESSED:
			caps_pressed_cons = (caps_pressed_cons + 1) % 2;
			break;
		case BACKSPACE:
			handle_backspace();
			break;
		case ENTER:
			handle_enter();
			break;
		case ALT_PRESSED:
			alt_pressed_cons = 1;
			break;
		case ALT_RELEASED:
			alt_pressed_cons = 0;
			break;
		case CONTROL_PRESSED:
			control_pressed_cons = 1;
			break;
		case CONTROL_RELEASED:
			control_pressed_cons = 0;
			break;
		default:
			display_on_screen(scan_code);
			break;
	}
	send_eoi(KEYBOARD_IRQ);
	sti();
}

void display_on_screen(uint32_t scan_code){
	if(scan_code >= 60 || scan_code < 0)return;
	if(control_pressed_cons == 1){
		if(scan_code == 0x26){
			//if control l is pressed; 
			clear();
		//	set_screen_pos(0,0);
		}
		return;		
	}

	if(alt_pressed_cons == 1){
		return;
	}

	uint8_t keyword = keyboard_ch[shift_pressed_cons+2*caps_pressed_cons][scan_code];
	append_to_buffer(keyword);
	puts(keyword);
	return;
}

void append_to_buffer(uint8_t keyword){
	if(buffer_index < 128){ //buffer size; 
		key_buffer[buffer_index] = keyword;
		buffer_index ++;
	}
}

void handle_backspace(){
	if(buffer_index > 0){
		key_buffer[buffer_index] = '\0';
		buffer_index --;
	}
	backspace();
	//delete the char in buffer and update in memory, set the cursor to the new location. 
}

void handle_enter(){
	key_buffer[buffer_index] = '\n';
	enter();
	//need to set the memory

}

// void clear(){
// 	//set the memory. 
// 	int i;
// 	for(i = 0 ; i < buffer_index; i++){
// 		key_buffer[i] = '\0';
// 	}
// 	buffer_index = 0;
// }
