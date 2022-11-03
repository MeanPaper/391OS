.align 4
.globl system_call, flush_cr3, context_switch

system_call:
    # eax will have sys_call number
    cmpl $LAST_SYS_CALL, %eax       # sys call num > 10 ?
    jg invalid_sys_call
    cmpl $FIRST_SYS_CALL, %eax      # sys call num < 1 ?
    jl invalid_sys_call
    
    pushal
    pushfl
    # pus h 3 special regs (inputs), first argument in ebx, then ecx, then edx
    pushl %edx
    pushl %ecx
    pushl %ebx 
    call *syscall_jump_table(,%eax,4) # call functions based on the given jump table
    addl $12, %esp
    # set up kernel stack again
    jmp return_to_user
    
invalid_sys_call:
    andl $0, %eax
    addl $-1, %eax
return_to_user:
    popfl
    popal
    iret

# num range is 1 to 9
syscall_jump_table:
.long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn


flush_TLB:
    movl %cr3, %eax
    movl %eax, %cr3
    ret

context_switch:
    ret
