#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define ARRAY_SIZE          1024
#define ALIGNMENT           4096
#define FOUR_KB             ALIGNMENT
#define FOUR_MB_SHIFT       22
#define FOUR_KB_SHIFT       12
#define VIDEO_PHYS          0xB8000
#define TERM1_VIDEO         VIDEO_PHYS + ALIGNMENT
#define TERM2_VIDEO         VIDEO_PHYS + ALIGNMENT * 2
#define TERM3_VIDEO         VIDEO_PHYS + ALIGNMENT * 3
#define VIDEO_PHYS_ALTER    VIDEO_PHYS + ALIGNMENT * 4 // this is the back up address for the physical memory

/* Initialize struct for 4MB page directory */
// The number of bits below is from intel manual pg.91
typedef union page_directory_4MB_entry {
    uint32_t val;
    struct {
        uint8_t present         : 1;        // P
        uint8_t rw              : 1;        // Read or write
        uint8_t user_super      : 1;        // User or Supervisor
        uint8_t write_through   : 1;        // PWT
        uint8_t cache_disabled  : 1;        // PCD
        uint8_t accessed        : 1;        // A
        uint8_t reserve         : 1;        // reserve default to 0
        uint8_t page_size       : 1;        // PS
        uint8_t global_page     : 1;        // G (ignored)
        uint8_t avail           : 3;        // Avl 
        uint8_t attribute       : 1;
        uint16_t Reserved       : 9;        // This is reserved and they are 0
        uint16_t page_base_addr : 10;
    } __attribute__ ((packed));
} page_directory_4MB_entry_t;

/* Initialize struct for  page directory */
// The number of bits below is from intel manual pg.90
typedef union page_directory_entry {
    uint32_t val;
    struct {
        uint8_t present         : 1;        // P
        uint8_t rw              : 1;        // Read or write
        uint8_t user_super      : 1;        // User or Supervisor
        uint8_t write_through   : 1;        // PWT
        uint8_t cache_disabled  : 1;        // PCD
        uint8_t accessed        : 1;        // A
        uint8_t reserve         : 1;        // reserve default to 0
        uint8_t page_size       : 1;        // PS
        uint8_t global_page     : 1;        // G (ignored)
        uint8_t avail           : 3;        // Avl       
        uint32_t page_table_base_addr :20;
    
    } __attribute__ ((packed));
} page_directory_entry_t;

/* Initialize struct for page table */
// The number of bits below is from intel manual pg.90
typedef union page_table_entry {
    uint32_t val;
    struct {
        uint8_t present         : 1;        // P
        uint8_t rw              : 1;        // Read or write
        uint8_t user_super      : 1;        // User or Supervisor
        uint8_t write_through   : 1;        // PWT
        uint8_t cache_disabled  : 1;        // PCD
        uint8_t accessed        : 1;        // A
        uint8_t dirty         : 1;
        uint8_t page_table_attribute_index  : 1;        // PAT
        uint8_t global_page     : 1;        // G 
        uint8_t avail           : 3;        // Avl
        uint32_t page_base_addr :20;      
    } __attribute__ ((packed));
} page_table_entry_t;



/* Initialize page directory and table array */
uint32_t page_directory[ARRAY_SIZE] __attribute__ ((aligned(ALIGNMENT)));
uint32_t first_page_table[ARRAY_SIZE] __attribute__ ((aligned(ALIGNMENT)));
uint32_t video_page_table[ARRAY_SIZE] __attribute__ ((aligned(ALIGNMENT)));

extern uint32_t vram_addrs[3];

/* void page_init();
 * Inputs: none
 * Return Value: none
 * Function: initialize page with video mem page and kernel page */
extern void page_init();

/* void loadPageDirectory(uint32_t* page_directory);
 * Inputs: uint32_t* page_directory = the pointer to our page directory
 * Return Value: none
 * Function: In ASM, set CR3(PDBR) to the base addr of our page directory */
extern void loadPageDirectory(uint32_t* page_directory);


/* void enablePaging();
 * Inputs: none
 * Return Value: none
 * Function: In ASM, we need to enable several bits in CR0 and CR4 
 *           - PG: Paging (bit 31 of CR0)
 *           - PE: Protection Enable (bit 0 of CR0).
 *           - PSE: Page Size Extensions (bit 4 of CR4) */
extern void enablePaging();

extern int32_t map_program_page(int pid_num);
extern int32_t map_vidmap_page(int32_t video_addr, int32_t cur_term);  // need to test this

extern int32_t remove_program_page(int pid_num);
extern int32_t video_mem_swap(uint8_t current, uint8_t next);
extern void map_sched_video_page(int term_idx);

#endif
