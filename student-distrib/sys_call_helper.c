#include "interrupt.h"
#include "lib.h"

void system_call_helper(){
    clear();
    printf(" System call, reach IDT 0x80 \n");  
}
