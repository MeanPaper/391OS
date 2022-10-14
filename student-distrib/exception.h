#ifndef _EXCEPTION_H
#define _EXCEPTION_H 


// exception functions
extern void divide_by_zero();          // 0
extern void debug_except();            // 1
extern void nmi_intr_except();         // 2
extern void breakpoint();              // 3
extern void overflow();                // 4
extern void BOUND_range_exceeded();    // 5
extern void invalid_opcode();          // 6
extern void device_not_avail();        // 7
extern void double_fault();            // 8
extern void coprocessor_seg_overrun(); // 9
extern void invalid_tts();             // 10
extern void segment_not_present();     // 11
extern void stack_segment_fault();     // 12
extern void general_protection();      // 13
extern void page_fault();              // 14
extern void fpu_float_error();         // 16
extern void alignment_check();         // 17   
extern void machine_check();           // 18
extern void simd_float_exception();    // 19

#endif
