/* keyboard.h - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
#include "schedule.h"
#include "terminal.h"
#include "sys_call_helper.h"

uint8_t shift_pressed_cons;
uint8_t caps_pressed_cons;
uint8_t alt_pressed_cons;
uint8_t control_pressed_cons;
uint8_t buffer_index;
void reset_keybuf(){
	memset((uint8_t*)key_buffer, 0, sizeof(key_buffer));
	buffer_index = 0;
}

/* Mapping scancode to ascii */
// 4 conditions, 60 different inputs. includes char, number and special keys. 
const char keyboard_ch[4][60] = {{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm',',', '.', '/', '\0', '*', '\0', ' ', '\0'}, 
	 //normal keyboard

	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V', 
	 'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'},
	//keyboard when shift is pressed and capslock is off.

	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A', 'S',
	 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V', 
	 'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'},
	 //keyboard when capslock is on and shift is not pressed. 

	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a', 's',
	 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', '\0', '\\', 'z', 'x', 'c', 'v', 
	 'b', 'n', 'm', '<', '>', '?', '\0', '*', '\0', ' ', '\0'}};
	 //keyboard when capslock is on, shift is also pressed. 

/* Initialize keyboard */
/* void keyboard_init();
 * Description: initialize the keyboard and enable the cursor
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: update cursor position to the last line and draw it. 
*/
void keyboard_init(){
	enable_irq(KEYBOARD_IRQ);
	outb( 0x0A,0x3D4);
	outb((inb(0x3D5) & 0xC0) | 0,0x3D5);
 
	outb (0x0B,(0x3D4));
	outb( (inb(0x3D5) & 0xE0) | 24,0x3D5);
	update_cursor(2);
}

// control, shift, caps lock
// static uint32_t control_code;

/* Handle keyboard interrupt */
/* void keyboard_interrupt();
 * Description: handle keyboard interrupt, depends on the keyboard input, output different things to the screen. 
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: set the corresponding global flag variable, i.e. caps/shift/alt/ctrl 
*/
void keyboard_interrupt(){
	// cli(); this a interrupt handler
	uint32_t scan_code;
	
	scan_code = inb(KEYBOARD_PORT);
			
	//read the input to see if it's a special key. 
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
			terms[display_terminal].read = 1;
			handle_enter();
			break;
		case ENTER_RELEASE:
			terms[display_terminal].read = 0;
			memset((void*)key_buffer, 0, sizeof(key_buffer));
			break;
		case LEFT_ALT_PRESSED:
			alt_pressed_cons = 1;
			break;
		case LEFT_ALT_RELEASED:
			alt_pressed_cons = 0;
			break;
		case LEFT_CONTROL_PRESSED:
			control_pressed_cons = 1;
			break;
		case LEFT_CONTROL_RELEASED:
			control_pressed_cons = 0;
			break;
		case TAB_PRESSED:
			handle_tab();
		default:  //if not special character,it's either a char or num, display it. 
			display_on_screen(scan_code);
			break;
	}
	send_eoi(KEYBOARD_IRQ); //end of eoi. 
}

void display_on_screen(uint32_t scan_code){
	//check if the scan code is in range, aka a char or num. 
	if(control_pressed_cons == 1){
		//if control+l, clear the screen and update cursor to top left. 
		if(scan_code == 0x26){ 
			//if control l is pressed; 
			clear();
			memset((void*)key_buffer, 0, sizeof(key_buffer));
			buffer_index = 0;
			update_cursor(1);
		}
		return;		
	}

//if alt is pressed, do nothing. alt and keypress will execute the terminal and everything
	if(alt_pressed_cons == 1){
		if(scan_code == F1_pressed && display_terminal != 0){
			strncpy((int8_t*)terms[display_terminal].terminal_buf, (int8_t*)key_buffer, 128);	// copy the content to the terminal buffer
			terms[display_terminal].key_index = buffer_index;		// storing the current buffer index
			buffer_index = terms[0].key_index;
			strncpy((int8_t*)key_buffer, (int8_t*)terms[0].terminal_buf, 128);
			send_eoi(KEYBOARD_IRQ);
			set_display_term(0);
			set_display_cursor();
		}
		else if(scan_code == F2_pressed && display_terminal != 1){
			
			if(get_process_total() < 6 || active_terminal[1] != -1){
				strncpy((int8_t*)terms[display_terminal].terminal_buf, (int8_t*)key_buffer, 128);	// copy the content to the terminal buffer
				terms[display_terminal].key_index = buffer_index;		// storing the current buffer index
				buffer_index = terms[1].key_index;
				strncpy((int8_t*)key_buffer, (int8_t*)terms[1].terminal_buf, 128);
				send_eoi(KEYBOARD_IRQ);
				set_display_term(1);
				set_display_cursor();
			}
		}
		else if(scan_code == F3_pressed && display_terminal != 2){
			if(get_process_total() < 6 || active_terminal[2] != -1){
				strncpy((int8_t*)terms[display_terminal].terminal_buf, (int8_t*)key_buffer, 128);	// copy the content to the terminal buffer
				terms[display_terminal].key_index = buffer_index;		// storing the current buffer index
				buffer_index = terms[2].key_index;
				strncpy((int8_t*)key_buffer, (int8_t*)terms[2].terminal_buf, 128);
				send_eoi(KEYBOARD_IRQ);
				set_display_term(2);
				set_display_cursor();
			}
		}
		return;
	}
	
	if(scan_code >= 60 || scan_code < 0)return; //check if in char array range
	//output the keyword according to scan_code and shift/control flag status. 
	uint8_t keyword = keyboard_ch[shift_pressed_cons+2*caps_pressed_cons][scan_code];
	if(buffer_index >= 127)return; //buffer max is 127, letter after 127th input will not be recorded. 
	append_to_buffer(keyword);
	kbd_putc(keyword);
	//update the cursor to the new position. 
	update_cursor(0);
	return;
}

/* void append_to_buffer();
 * Description: add the input char/num to the kbd buffer. 
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: increment the buffer_index 
*/
void append_to_buffer(uint8_t keyword){
	if(buffer_index < 127){ //buffer size; 
		key_buffer[buffer_index] = keyword;
		buffer_index ++;
	}
	key_buffer[127] = '\0'; //last char is null. 
}


/* void handle_backspace();
 * Description: delete the last letter in keyboard buffer, and change the position of the cursor. 
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: decrement the buffer_index 
*/
void handle_backspace(){
	if(buffer_index > 0){
		key_buffer[--buffer_index] = '\0';
		backspace();
	}
	//delete the char in buffer and update in memory, set the cursor to the new location. 
}

/* void handle_enter();
 * Description: handle everything if user hit enter.  
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: none
*/
void handle_enter(){
	
	key_buffer[buffer_index] = '\n';
	enter();
	buffer_index = 0;

}

/* void handle_tab();
 * Description: handle tab if user hit tab.  
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: none
*/
void handle_tab(){
	int i;
	for(i = 0; i < 4; i++){
		key_buffer[buffer_index+i] = ' ';
		kbd_putc(' ');
	}
	buffer_index += 4; //4space = 1tab
	tab();
}
