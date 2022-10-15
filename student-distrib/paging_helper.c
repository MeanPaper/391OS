#include "paging.h"
#include "lib.h"

uint32_t page_directory[1024] __attribute__ ((aligned(4096)));
uint32_t first_page_table[1024] __attribute__ ((aligned(4096)));

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
        page_directory[i] = 0;
    }

    //we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < 1024; i++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        first_page_table[i] = 0; // attributes: supervisor level, read/write, present.
    }

    page_directory[0] = ((unsigned int)first_page_table) | 3;

    page_directory_4MB_entry_t kernel_page;

    page_table_entry_t vid_page;
    vid_page.val = 0;

    // Set up video mem page
    vid_page.page_base_addr = 0xB8000 >> 12;
    vid_page.rw = 1;
    vid_page.present = 1;
    first_page_table[0xB8000 >> 12] = vid_page.val;

    // Make the kernel page
    kernel_page.val = 0;
    kernel_page.page_base_addr = 1;
    kernel_page.present = 1;
    kernel_page.rw = 1;
    kernel_page.page_size = 1;
    // kernel_page.global_page = 1;
    page_directory[1] = kernel_page.val;

    // load page directory to cr3
    loadPageDirectory(page_directory);

    //enable paging
    enablePaging();

}
