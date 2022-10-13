
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "x86_desc.h"
#include "idt_desc.h"

#define INTEL_LAST_DEF  0x1F  // the last index of intel define info on idt
#define SYSTEM_CALL     0x80  // index for system calls



// void divide_error_exception(){
//     printf(" Divide Error Exception \n");
//     while(1);
// }

/**
 * 
 *  Notes from IA-32 documentation page 156, and mp3 documentation appendix D
 * 
 *  In the context of trap gate and interrupt gate
 * 
 *  D - size of gate: 1 means 32 bit, 0 means 16 bits
 *  reserved 3 = bit 8 = use to determine whether this is a trap or not 
 *  reserved 2 = bit 9 = 1, by design
 *  reserved 1 = bit 10 = 1, by design
 *  reserved 0 = bti 12 = 0, by design
 *  P = bit 15 = present bit = must be set to 1 for the discriptor to be valid
 * 
 *  interrupt gate             
 *  31-------------16--15-14-13-12-------8--7-6-5--4-----------0
 *  | Offset 31..16  | P | DPL | 0 D 1 1 0 |0 0 0 | reserved_4 |  4                            |
 *  +----------------------------------------------------------+
 *  31------------------------16-15----------------------------0
 *  |     Segment Selector      |      Offset 15 .. 0          |  0
 *  +----------------------------------------------------------+
 * 
 *  trap gate
 *  31-------------16--15-14-13-12-------8--7-6-5--4-----------0
 *  | Offset 31..16  | P | DPL | 0 D 1 1 1 |0 0 0 | reserved_4 |  4                            |
 *  +----------------------------------------------------------+
 *  31------------------------16-15----------------------------0
 *  |     Segment Selector      |      Offset 15 .. 0          |  0
 *  +----------------------------------------------------------+
 * 
 * the first 20 line of the code should work
 * 
 * 
 * 
*/

// initialize IDT
// 0x00 - 0x1F: exceptions, require trap gate settings
void init_idt_desc(){
    int i;
    for(i = 0; i <= INTEL_LAST_DEF; ++i){
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1; // set size of the gate to 32 bit 
        idt[i].reserved0 = 0;
        // trap or interrupt, set dpl to 0
        idt[i].dpl = 0;     
        idt[i].present = 1;   
        
    }

    for(i = INTEL_LAST_DEF + 1; i < NUM_VEC; ++i){
        // here are all the interrupt gates
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;        
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;    // setting interrupts dpl
        idt[i].present = 1;  
    }
    idt[SYSTEM_CALL].dpl = 3; // system call have dpl = 3

    // setting up other information
    // question on setting up idt, for the the first 32 entries as well as the rest of the entries
    // not sure what to do with them for now
    
    // SET_IDT_ENTRY(idt[0x00], some_handler);
    // SET_IDT_ENTRY(idt[0x01], some_handler);
    // SET_IDT_ENTRY(idt[0x02], some_handler);
    // SET_IDT_ENTRY(idt[0x03], some_handler);
    // SET_IDT_ENTRY(idt[0x04], some_handler);
    // SET_IDT_ENTRY(idt[0x05], some_handler);
    // SET_IDT_ENTRY(idt[0x06], some_handler);
    // SET_IDT_ENTRY(idt[0x07], some_handler);
    // SET_IDT_ENTRY(idt[0x08], some_handler);
    // SET_IDT_ENTRY(idt[0x09], some_handler);
    // SET_IDT_ENTRY(idt[0xA], some_handler);
    // SET_IDT_ENTRY(idt[0xB], some_handler);
    // SET_IDT_ENTRY(idt[0xC], some_handler);
    // SET_IDT_ENTRY(idt[0xD], some_handler);
    // SET_IDT_ENTRY(idt[0xE], some_handler);  
    // SET_IDT_ENTRY(idt[0xF], some_handler);
    // SET_IDT_ENTRY(idt[0x10], some_handler);
    // SET_IDT_ENTRY(idt[0x11], some_handler);
    // SET_IDT_ENTRY(idt[0x12], some_handler);
    // SET_IDT_ENTRY(idt[0x13], some_handler); // idt index 19
    // SET_IDT_ENTRY(idt[0x14], some_handler); // 20 to 31 do not use


    lidt(idt_desc_ptr);
}
