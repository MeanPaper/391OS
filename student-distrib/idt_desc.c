
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "tests.h"
#include "x86_desc.h"
#include "idt_desc.h"
#include "interrupt.h"
#include "exception.h"
#include "system_call.h"
#include "schedule.h"

#define INTRRUPT_START          32    // start of user defined interrupt in IDT
#define SYSTEM_CALL_INDEX     0x80  // index for system calls
#define INTEL_LAST_DEF          19    // the last valid intel defined entry
#define INTEL_RESERVED          15    // on the IDT, 15 is reserved by intel
#define NMI_INTR                2
#define EXCEPTION_MSG_NUM       20


// this function is used to be used by idt index 15
// for testing purpose
// void print_intel(){
//     printf(" intel reserved \n");
//     while(1);
// }


/*
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
 *  the first 20 line of the code should work
 * 
*/

// 0x00 - 0x1F: exceptions, require trap gate settings

/* void init_idt_desc();
 * Description: initialize idt entry (exceptions, interrupt, system call)
 *              and connect them to the corresponding handler
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: idt table changes
*/
void init_idt_desc(){
    int i;
    for(i = 0; i < INTEL_LAST_DEF; ++i){
        if(INTEL_RESERVED == i){continue;}
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

    for(i = INTRRUPT_START; i < NUM_VEC; ++i){
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
    idt[SYSTEM_CALL_INDEX].dpl = 3; // system call have dpl = 3
    
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

    // user defined interrupts: rtc and keyboard
    SET_IDT_ENTRY(idt[0x20], pit_handle_call);
    SET_IDT_ENTRY(idt[0x21], keyboard_intr_call);
    SET_IDT_ENTRY(idt[0x28], rtc_interrupt_call);

    // system call
    SET_IDT_ENTRY(idt[0x80], system_call);
}

// there are
const char* exception_message[EXCEPTION_MSG_NUM] = {
    " Divide by 0 error exception \n ",
    " Debug exception \n",
    " NMI interrupt \n",
    " Breakpoint exception \n",
    " Overflow exception \n",
    " BOUND range exceeded exception \n",
    " Invalid opcode exception \n",
    " Device not available exception \n",
    " Double fault exception \n",
    " Coprocessor segment overrun exception \n",
    " Invalid TSS exception \n",
    " Segment not present exception \n",
    " Stack segment fault exception \n",
    " General protection exception \n",
    " Page fault exception \n",
    " OOPS, intel reserved",
    " x87 FPU floating-point error exception \n",
    " Alignment check exception \n",
    " Machine check exception \n",
    " SIMD floating-point exception \n"
};

/*
 * void exception_handler(int idt_num)
 * Description: print exception message base on the corresponding exception idt
 *              number
 * 
 * Inputs:      int idt_num  :  the idt index of the exception
 * Output: none
 * Return Value: none
 * Side Effects: clear screen and output the exception message and force 
 *               system to stay in loop (for checkpoint 1)
 * 
*/
void exception_handler(int idt_num){
    // clear();
    if(idt_num < 0 || idt_num > INTEL_LAST_DEF){    
        printf("oops! reaching the wrong code.");
        set_exception_flag(1);
        halt(0);
    }
    set_exception_flag(1);
    printf("%s", exception_message[idt_num]);
    halt(0);
}
