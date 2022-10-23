.align 4096

.globl loadPageDirectory
loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    leave
    ret

# setting in the following order cr3, cr4, cr0
.globl enablePaging
enablePaging:
    push %ebp
    mov %esp, %ebp

    # start with cr4 
    mov %cr4, %eax
    or $0x010, %eax          # bit 4 of CR4
    mov %eax, %cr4

    # and then cr0
    mov %cr0, %eax
    or $0x80000001, %eax    # bit 31 and 0 of CR0
    mov %eax, %cr0

    leave
    ret
