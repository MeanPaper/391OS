#ifndef _PAGING_H
#define _PAGING_H

typedef union page_directory_4MB_entry {
    uint32_t val;
    struct {
        uint16_t page_base_addr : 10;
        uint16_t Reserved       : 9;        // This is reserved and they are 0
        uint8_t page_table_attribute_index  : 1;        // PAT
        uint8_t avail           : 3;        // Avl
        uint8_t global_page     : 1;        // G 
        uint8_t page_size       : 1;        // PS
        uint8_t dirty           : 1;        // dirty ??
        uint8_t accessed        : 1;        // A
        uint8_t cache_disabled  : 1;        // PCD
        uint8_t write_through   : 1;        // PWT
        uint8_t user_super      : 1;        // User or Supervisor
        uint8_t rw              : 1;        // Read or write
        uint8_t present         : 1;        // P
    } __attribute__ ((packed));
} page_directory_4MB_entry_t;

typedef union page_directory_entry {
    uint32_t val;
    struct {
        uint32_t page_table_base_addr :20;
        uint8_t avail           : 3;        // Avl
        uint8_t global_page     : 1;        // G (ignored)
        uint8_t page_size       : 1;        // PS
        uint8_t reserve         : 1;        // reserve default to 0
        uint8_t accessed        : 1;        // A
        uint8_t cache_disabled  : 1;        // PCD
        uint8_t write_through   : 1;        // PWT
        uint8_t user_super      : 1;        // User or Supervisor
        uint8_t rw              : 1;        // Read or write
        uint8_t present         : 1;        // P
    } __attribute__ ((packed));
} page_directory_entry_t;

typedef union page_table_entry {
    uint32_t val;
    struct {
        uint32_t page_base_addr :20;
        uint8_t avail           : 3;        // Avl
        uint8_t global_page     : 1;        // G 
        uint8_t page_table_attribute_index  : 1;        // PAT
        uint8_t dirty           : 1;        // dirty ??
        uint8_t accessed        : 1;        // A
        uint8_t cache_disabled  : 1;        // PCD
        uint8_t write_through   : 1;        // PWT
        uint8_t user_super      : 1;        // User or Supervisor
        uint8_t rw              : 1;        // Read or write
        uint8_t present         : 1;        // P
    } __attribute__ ((packed));
} page_table_entry_t;

page_directory_entry_t page_directory[1024] __attribute__ ((aligned(4096)));
page_table_entry_t first_page_table[1024] __attribute__ ((aligned(4096)));

extern void page_init();

extern void loadPageDirectory(page_directory_entry_t* page_directory);
extern void enablePaging();

#endif
