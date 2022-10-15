# interrupt wrapper code
# including system call

#define INTERRUPT_WRAPPER(func_name, handler) \
    iret


#define SYS_CALL(func_name, handler) \
    .global	func_name               ;\
    func_name:                      ;\ 
        call handler                ;\
        iret                       

SYS_CALL(system_call, system_call_helper)
