#ifndef PAGING_H
#define PAGING_H


uint32_t page_directory[1024];
uint32_t first_page_table[1024];

extern void page_init();

//other functions
//loadPageDirectory(ASM)
//enablePaging(ASM)

// need data allignment with 4096

#endif
