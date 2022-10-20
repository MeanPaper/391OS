#include "interrupt.h"
#include "lib.h"

/*
 * system_call_helper 
 * Description: output system call message and put system in loop for now
 * Input: none
 * Output: none
 * Return value: none
 * 
*/
void system_call_helper(){
    clear();
    printf(" System call, reach IDT 0x80 \n");
    while(1);  
}
