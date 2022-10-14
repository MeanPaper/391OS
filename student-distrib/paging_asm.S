#include "paging.h"
.align 4096

.globl loadPageDirectory
loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

.globl enablePaging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax    # bit 31 of CR0
mov %eax, %cr0
mov %cr4, %eax
or $0x90, %eax          # bit 7 and bit 4 of CR4
mov %eax, %cr4
mov %ebp, %esp
pop %ebp
ret


