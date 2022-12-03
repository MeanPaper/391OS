# interrupt wrapper code
# including system call
#define ASM 1




# INTERRUPT_WRAPPER
# Description: interrupt handler wrapper
#
# Input:    func_name : label for function name
#           irq_num   : label for interrupt number
# Output:       none
# Reture Value: none
# 
# 

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

INTERRUPT_WRAPPER(rtc_interrupt_call, rtc_handler, 0x28)
INTERRUPT_WRAPPER(keyboard_intr_call, keyboard_interrupt, 0x21)
INTERRUPT_WRAPPER(pit_handle_call, pit_handler , 0x20)
