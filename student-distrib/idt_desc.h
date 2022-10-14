#ifndef IDT_DESC_H
#define IDT_DESC_H


// initialize the idt table
extern void init_idt_desc();

// #define INTR_LINK(name, func)    \
//     .global name                ;\
//     name:                       ;\
//         pushal                  ;\
//         pushfl                  ;\
//         call func               ;\
//         popfl                   ;\
//         popal                   ;\
//         iret                    ;\

// INTR_LINK(rtc_handler_linkage, rtc_handler);

#endif
