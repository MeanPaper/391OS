
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "x86_desc.h"
#include "idt_desc.h"

#define INTRRUPT_START  32    // start of user defined interrupt in IDT
#define SYSTEM_CALL     0x80  // index for system calls
#define INTEL_LAST_DEF  19    // the last valid intel defined entry
#define INTEL_RESERVED  15    // on the IDT, 15 is reserved by intel
#define NMI_INTR        2


void divide_by_zero(){ // 0
    clear();
    printf(" Divide by 0 error exception \n");
    while(1);
}

void debug_except(){ // 1
    clear();
    printf(" Debug exception \n");
    while(1);
}

void nmi_intr_except(){ // 2
    clear();
    printf(" NMI interrupt \n");
    while(1);
}

void breakpoint(){ // 3
    clear();
    printf(" Breakpoint exception \n");
    while(1);
}

void overflow(){ // 4
    clear();
    printf(" Overflow exception \n");
    while(1);
}

void BOUND_range_exceeded(){ // 5
    clear();
    printf(" BOUND range exceeded exception \n");
    while(1);
}

void invalid_opcode(){ // 6
    clear();
    printf(" Invalid opcode exception \n");
    while(1);
}

void device_not_avail(){ // 7
    clear();
    printf(" Device not available exception \n");
    while(1);
}

void double_fault(){ // 8
    clear();
    printf(" Double fault exception \n");
    while(1);
}

void coprocessor_seg_overrun(){ // 9
    clear();
    printf(" Coprocessor segment overrun exception \n");
    while(1);
}

void invalid_tts(){ // 10
    clear();
    printf(" Invalid TSS exception \n");
    while(1);
}

void segment_not_present(){ // 11
    clear();
    printf(" Segment not present exception \n");
    while(1);
}

void stack_segment_fault(){ // 12
    clear();
    printf(" Stack segment fault exception \n");
    while(1);
}

void general_protection(){ // 13
    clear();
    printf(" General protection exception \n");
    while(1);
}

void page_fault(){ // 14
    clear();
    printf(" Page fault exception \n");
    while(1);
}

void fpu_float_error(){ // 16
    clear();
    printf(" x87 FPU floating-point error exception \n");
    while(1);
}

void alignment_check(){ // 17   
    clear();
    printf(" Alignment check exception \n");
    while(1);
}

void machine_check(){ // 18
    clear();
    printf(" Machine check exception \n");
    while(1);
}

void simd_float_exception(){ // 19
    clear();
    printf(" SIMD floating-point exception \n");
    while(1);
}


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
 *  | Offset 31..16  | P | DPL | 0 D 1 1 0 |0 0 0 | reserved_4 |  4                            
 *  +----------------------------------------------------------+
 *  31------------------------16-15----------------------------0
 *  |     Segment Selector      |      Offset 15 .. 0          |  0
 *  +----------------------------------------------------------+
 * 
 *  trap gate
 *  31-------------16--15-14-13-12-------8--7-6-5--4-----------0
 *  | Offset 31..16  | P | DPL | 0 D 1 1 1 |0 0 0 | reserved_4 |  4                            
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
    for(i = 0; i <=INTEL_LAST_DEF; ++i){
        if(INTEL_RESERVED == i) { // entry 15 intel reserved, not used
            continue;
        }
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = (NMI_INTR == i) ? 0 : 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1; // set size of the gate to 32 bit 
        idt[i].reserved0 = 0;
        // trap or interrupt, set dpl to 0
        idt[i].dpl = 0;     
        idt[i].present = 1;   
        
    }

    for(i = INTRRUPT_START + 1; i < NUM_VEC; ++i){
        // here are all the interrupt gates, dl35 included a long comment on the top
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
    
    // intel defined part, exception and interrupt
    SET_IDT_ENTRY(idt[0], divide_by_zero);
    SET_IDT_ENTRY(idt[1], debug_except);
    SET_IDT_ENTRY(idt[2], nmi_intr_except);
    SET_IDT_ENTRY(idt[3], breakpoint);
    SET_IDT_ENTRY(idt[4], overflow);
    SET_IDT_ENTRY(idt[5], BOUND_range_exceeded);
    SET_IDT_ENTRY(idt[6], invalid_opcode);
    SET_IDT_ENTRY(idt[7], device_not_avail);
    SET_IDT_ENTRY(idt[8], double_fault);
    SET_IDT_ENTRY(idt[9], coprocessor_seg_overrun);
    SET_IDT_ENTRY(idt[10], invalid_tts);
    SET_IDT_ENTRY(idt[11], segment_not_present);
    SET_IDT_ENTRY(idt[12], stack_segment_fault);
    SET_IDT_ENTRY(idt[13], general_protection);
    SET_IDT_ENTRY(idt[14], page_fault);  
    SET_IDT_ENTRY(idt[16], fpu_float_error);
    SET_IDT_ENTRY(idt[17], alignment_check);
    SET_IDT_ENTRY(idt[18], machine_check);
    SET_IDT_ENTRY(idt[19], simd_float_exception); // idt index 19

    // user defined part, exception and interrupt

    lidt(idt_desc_ptr);
}
