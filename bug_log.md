# Template
## Bug #: INT
### Date: 01/10/2000
### Symptom: 
### Difficulty to find: 
### Difficulty to fix:
---
# Bug Log

## Checkpoint 1
--- 
## Bug #: 1
### Date: 10/15/2022
### Symptom: 
    Page code seems to be correct however, info_mem Page Disabled.
### Difficulty to find: 
    Hard
### Difficulty to fix:
    Not easy, Henry helped us. The first syntax error is that all my bits in the struct are flipped since the least significant bit should be on top and the most significant bit should be at the bottom.
    
    Second main issue is that even though our struct is created, there is nothing in it. So the kernel do not know what to do with them. So we need to initialize the struct. The 4MB is for kernel. The video memory one is another bug in this. 
---
## Bug #: 2
### Date: 10/15/2022
### Symptom: 
    After init IDT, it will cause double fault
### Difficulty to find: 
    Hard
### Difficulty to fix:
    Need to initialize PIC before enable interrupt. PIC code is also buggy.
--- 

## Checkpoint 2
---
## Bug #: 3
### Date: 10/22/2022
### Symptom: 
    Keyboard can puts all characters and numbers, including ctrl and shift. But when trying to add the input to the buffer, it will output page fault exception. 
### Difficulty to find: 
    medium
### Difficulty to fix:
    easy. use puts instead of puc
---
## Bug #: 4
### Date: 10/22/2022
### Symptom: 
    Cursor position is not at thte correct location afte enter/backspace. 
### Difficulty to find: 
    medium
### Difficulty to fix:
    medium, write out own function of kbd_putc to handle kbd input. But the putc is fixed after that. 
---

## Bug #: 5
### Date: 10/22/2022
### Symptom: 
    read dentry by name and read dentry by index causing page fault
### Difficulty to find: 
    Hard
### Difficulty to fix:
    easy, there are a lots of pointer type casting and pointer manipulations going on. So there might be a situation where the program read an invalid address or reaching a null pointer, causing page fault exception. To fix the bug, we re-examine all the pointer type casting and pointer manipulation part of the function, and change all unmatching pointer type to the expected pointer type 
---
## Bug #: 6
### Date: 10/22/2022
### Symptom: 
    Page fault exception fired when dentry struct global variable is used
### Difficulty to find: 
    Medium
### Difficulty to fix:
    easy, when initializing global variables, compiler initializes all the global and static variable to 0 (for pointer type, the pointer variable is initialized to NULL). Since the global dentry struct we used at the first time is a struct pointer, the program fires page fault exception when we tries to dereferencing it.
---
## Bug #: 7
### Date: 10/22/2022
### Symptom: 
    file read only works for some specific files. not all valid files can be read.
    For example, frame0.txt can be read but frame1.txt cannot
### Difficulty to find: 
    Medium
### Difficulty to fix:
    easy, to fix the bug we rewrote the function that is used to find the data blocks.
---
