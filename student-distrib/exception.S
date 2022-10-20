#  some function have the error and some does no
#  stack
#  +--------------------+
#  |     irq/except     |
#  +--------------------+
#  | error code / dummy |
#  +--------------------+
#  |     return addr    |
#  +--------------------+
#  |  something else    |

# pushfl and popfl are done by the processors so I don't have to save it
# according to the descriptor file


# EXCEPTION
# Description: exception handler wrapper without error code
#
# Input:    func_name : label for function name
#           idt_num   : label for idt number
# Output:       none
# Reture Value: none
# 
# 

#define EXCEPTION(func_name, idt_num)        \
    .global func_name                       ;\
    func_name:                              ;\
        pushl $0                            ;\
        pushal                              ;\
        pushl $idt_num                      ;\
        call exception_handler              ;\
        addl $4, %esp                       ;\
        popal                               ;\
        addl $4, %esp                       ;\
        iret               


# EXCEPTION_WITH_CODE
# Description: exception handler wrapper with error code
#
# Input:    func_name : label for function name
#           idt_num   : label for idt number
# Output:       none
# Reture Value: none
# 
# 

#define EXCEPTION_WITH_CODE(func_name, idt_num) \
    .global func_name                       ;\
    func_name:                              ;\
        pushal                              ;\
        pushl $idt_num                      ;\
        call exception_handler              ;\
        addl $4, %esp                       ;\
        popal                               ;\
        addl $4, %esp                       ;\
        iret        
                
# connect exception helper function to exception handler 
# with specific exception number
EXCEPTION(divide_by_zero, 0)
EXCEPTION(debug_except, 1)
EXCEPTION(nmi_intr_except, 2)
EXCEPTION(breakpoint, 3)
EXCEPTION(overflow, 4)
EXCEPTION(BOUND_range_exceeded, 5)
EXCEPTION(invalid_opcode, 6)
EXCEPTION(device_not_avail, 7)

EXCEPTION_WITH_CODE(double_fault, 8)

EXCEPTION(coprocessor_seg_overrun, 9)

EXCEPTION_WITH_CODE(invalid_tts, 10)
EXCEPTION_WITH_CODE(segment_not_present, 11)
EXCEPTION_WITH_CODE(stack_segment_fault, 12)
EXCEPTION_WITH_CODE(general_protection, 13)
EXCEPTION_WITH_CODE(page_fault, 14)

EXCEPTION(fpu_float_error, 16)
EXCEPTION_WITH_CODE(alignment_check, 17)
EXCEPTION(machine_check, 18)
EXCEPTION(simd_float_exception, 19)
