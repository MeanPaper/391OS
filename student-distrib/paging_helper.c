#include "paging.h"
#include "lib.h"
#include "terminal.h"
#include "system_call.h"

#define FIRST_PROG_VIRTUAL 0x08000000 // 128 MB
#define FOUR_MB_PAGE       0x400000
#define PROG_FIRST_PAGE    0x800000 // 8 MB

uint32_t vram_addrs[3] = {TERM1_VIDEO, TERM2_VIDEO, TERM3_VIDEO};

void init_terminal_video(){
    int i;
    page_table_entry_t video_page;
    uint32_t video_phys = VIDEO_PHYS_ALTER;

    for(i = 0; i < 3; ++i){
        video_page.val = 0;
        video_page.page_base_addr = vram_addrs[i] >> FOUR_KB_SHIFT; 
        video_page.rw = 1;
        video_page.present = 1;
        // video_page.user_super = 1;
        first_page_table[vram_addrs[i] >> FOUR_KB_SHIFT] = video_page.val;
    }
    
    // this set up the backup page for the physical memory
    video_page.val = 0;
    video_page.page_base_addr = (uint32_t)((VIDEO_PHYS) >> FOUR_KB_SHIFT);
    video_page.rw = 1;
    video_page.present = 1;
    video_page.user_super = 1;
    first_page_table[(int32_t)(video_phys >> FOUR_KB_SHIFT)] = video_page.val;
} 

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

    // |3 enables the low two bit which is present and r/w
    page_directory[0] = ((unsigned int)first_page_table) | 7;

    page_directory_4MB_entry_t kernel_page;

    page_table_entry_t vid_page;
    vid_page.val = 0;

    /* Set up video mem page */
    // this need 4kB page
    // physcial addr / 4096 to determine the index on page table
    // because we don't have that many space in the page_base_addr
    // also the bottom 12 bits are zero anyway
    vid_page.page_base_addr = VIDEO_PHYS >> 12; 
    vid_page.rw = 1;
    vid_page.present = 1;
    first_page_table[VIDEO_PHYS >> 12] = vid_page.val;
    init_terminal_video();

    /* set up the kernel page */
    kernel_page.val = 0;
    kernel_page.page_base_addr = 1;
    kernel_page.present = 1;
    kernel_page.rw = 1;
    kernel_page.page_size = 1;

    // kernel_page.global_page = 1;
    // this is because kernel page only use 4MB page
    // so there is no page table indirection
    page_directory[1] = kernel_page.val;

    // load page directory to cr3
    loadPageDirectory(page_directory);

    //enable paging
    enablePaging();

}


// change user level physcal virtual address mapping
void map_sched_video_page(int term_idx){
    page_table_entry_t temp;
    temp.val = first_page_table[(VIDEO_PHYS_ALTER) >> 12];   // getting the user pointer
    if(term_idx == display_terminal){                      // force the user point to the physical memory
        temp.page_base_addr = (VIDEO_PHYS) >> 12;
        first_page_table[(VIDEO_PHYS_ALTER) >> 12] = temp.val;
        flush_TLB();
        return;
    }
    
    temp.page_base_addr = vram_addrs[term_idx] >> 12;
    first_page_table[(VIDEO_PHYS_ALTER) >> 12] = temp.val;
    flush_TLB();
}

int32_t map_program_page(int pid_num){
    page_directory_4MB_entry_t prog_page; // global page directory entry?
    prog_page.val = 0; // clean the page
    prog_page.page_base_addr = (PROG_FIRST_PAGE + (pid_num-1) * FOUR_MB_PAGE) >> FOUR_MB_SHIFT;    // 
    prog_page.present = 1;
    prog_page.rw = 1;
    prog_page.page_size = 1;
    prog_page.user_super = 1;                                           
    page_directory[(FIRST_PROG_VIRTUAL) >> FOUR_MB_SHIFT] = prog_page.val;       // loading the 4MB page directory entries 
    flush_TLB();
    return 0;
}

// this one has to change to allow multiple terminal addresses
int32_t map_vidmap_page(int32_t video_addr, int32_t cur_term){
    page_table_entry_t vid_page;
    vid_page.val = 0;

    if(cur_term == display_terminal){
        vid_page.page_base_addr = VIDEO_PHYS >> 12;
    }
    else{
        vid_page.page_base_addr = vram_addrs[cur_term] >> 12;
    }
    vid_page.rw = 1;                          // enable read and write
    vid_page.present = 1;                     // present the page
    vid_page.user_super = 1;                  // allowing dpl 3
    video_page_table[cur_term] = vid_page.val;       // this is at page table entry 0 because the virtual address is 256MB the body are 0 for the most part
    page_directory[video_addr >> FOUR_MB_SHIFT] = (uint32_t)(video_page_table) | 7;  // or with 111 allowing r/w, p, as well as user dpl  
    flush_TLB();
    return 0;   
}
// void addpage
// pm addr >> 22
// void removepage

int32_t remove_program_page(int pid_num){
    page_directory[(FIRST_PROG_VIRTUAL ) >> FOUR_MB_SHIFT] &= 0x0;
    return 0;
}

