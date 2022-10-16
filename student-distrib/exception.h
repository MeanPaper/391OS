#ifndef _EXCEPTION_H
#define _EXCEPTION_H 


/* 
 * All the functions below are just a wrapper for 
 * different exceptions. They all connect to exception_handler
 * but with different exception number and error codes
 * 
 * All functions have no input, no output, no return value
 * All functions will print exception message because they connect
 * to exception_handler
 * 
*/
extern void divide_by_zero();          // idt 0
extern void debug_except();            // idt 1
extern void nmi_intr_except();         // idt 2
extern void breakpoint();              // idt 3
extern void overflow();                // idt 4
extern void BOUND_range_exceeded();    // idt 5
extern void invalid_opcode();          // idt 6
extern void device_not_avail();        // idt 7
extern void double_fault();            // idt 8
extern void coprocessor_seg_overrun(); // idt 9
extern void invalid_tts();             // idt 10
extern void segment_not_present();     // idt 11
extern void stack_segment_fault();     // idt 12
extern void general_protection();      // idt 13
extern void page_fault();              // idt 14
extern void fpu_float_error();         // idt 16
extern void alignment_check();         // idt 17   
extern void machine_check();           // idt 18
extern void simd_float_exception();    // idt 19

#endif
