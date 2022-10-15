# interrupt wrapper code
# including system call

#define INTERRUPT_WRAPPER(func_name, handler) \
    .global	func_name                       ;\
    func_name:                              ;\
        pushal                              ;\
        call handler                        ;\
        popal                               ;\
        iret


# system call only use register

#define SYS_CALL(func_name, handler) \
    .global	func_name               ;\
    func_name:                      ;\ 
        call handler                ;\
        iret                       

SYS_CALL(system_call, system_call_helper)

INTERRUPT_WRAPPER(rtc_interrupt_call, rtc_handler)
INTERRUPT_WRAPPER(keyboard_intr_call, keyboard_interrupt)
