# Template
## Bug #: INT
### Date: 01/10/2000
### Symptom: 
### Difficulty to find: 
### Difficulty to fix:
---
# Bug Log
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