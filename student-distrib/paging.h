#ifndef PAGING_H
#define PAGING_H

#include "types.h"

uint32_t page_directory[1024];
uint32_t first_page_table[1024];

extern void page_init();

// need data allignment with 4096
// This should go outside any function..
extern void loadPageDirectory(unsigned int*) {
    asm volatile ("push %ebp            \n\
                mov %esp, %ebp          \n\
                mov 8(%esp), %eax       \n\
                mov %eax, %cr3          \n\
                mov %ebp, %esp          \n\
                pop %ebp                \n\
                ret                     \n\
                "                       \
    );

    return;
}
extern void enablePaging() {
    asm volatile ("push %ebp            \n\
                mov %esp, %ebp          \n\
                mov %cr0, %eax          \n\
                or $0x80000000, %eax    \n\
                mov %eax, %cr0          \n\
                mov %ebp, %esp          \n\
                pop %ebp                \n\
                ret                     \n\
                "                       \
    );

    return;
}

#endif
