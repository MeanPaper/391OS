#include "paging.h"
#include "lib.h"

// initialize page
// reference: https://wiki.osdev.org/Setting_Up_Paging
void page_init() {
    //set each entry to not present
    unsigned int i;
    for(i = 0; i < 1024; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i].val = 0x00000002;
    }

    //we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        first_page_table[i].val = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
    }

    page_directory[0].val = ((unsigned int)first_page_table) | 3;

    // load page directory to cr3
    loadPageDirectory(page_directory);

    //enable paging
    enablePaging();

}


