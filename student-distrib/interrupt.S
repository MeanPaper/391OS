# interrupt wrapper code
# including system call
#define ASM 1
#define INTERRUPT_WRAPPER(func_name, handler, irq_num) \
    .global	func_name                       ;\
    func_name:                              ;\
        push $0                             ;\
        push $irq_num                       ;\
        pushal                              ;\
        call handler                        ;\
        popal                               ;\
        addl $8, %esp                       ;\
        iret


# system call only use register

#define SYS_CALL(func_name, handler) \
    .global	func_name               ;\
    func_name:                      ;\
        push $0                     ;\
        push $0x80                  ;\
        call handler                ;\
        addl $8, %esp               ;\
        iret                       

SYS_CALL(system_call, system_call_helper)

INTERRUPT_WRAPPER(rtc_interrupt_call, rtc_handler, 0x28)
INTERRUPT_WRAPPER(keyboard_intr_call, keyboard_interrupt, 0x21)
