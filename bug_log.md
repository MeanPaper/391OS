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
## Checkpoint 3
---
## Bug #: 8
### Date: 11/4/2022
### Symptom: 
    Not able to print new line for "ls"
### Difficulty to find: 
    Medium
### Difficulty to fix:
    Not too difficult. In our terminal write, we will terminates the buffer with a null at index 32, then we will break. However, this implementation will not allow "ls" program to insert "/n". So we basically comment taht line our. 
---
## Bug #: 9
### Date: 11/5/2022
### Symptom: 
    After execute, there will be page fault. We find that when we try to return from halt, our parent esp and parent ebp is not working correctly.
### Difficulty to find: 
    Easy. Since it is after execute, so we have a pretty good idea that it is in the halt.
### Difficulty to fix:
    Difficult. We did not understand our execute with parent process and child process correctly. So Henry helped us to debug and clarify what's going on. So when halting child process, the child should return to child execute first before return to parent halt/process. 
---
## Bug #: 10
### Date: 11/6/2022
### Symptom: 
    In our shell terminal, when we do backspace, we could even delete "391OS> ". 
### Difficulty to find: 
    Easy. Since we know this is definitely from our keyboard function. 
### Difficulty to fix:
    Easy. In our terminal.c:handle_backspace, we need to make sure that when the buffer is empty, we cannot do backspace. 
---
## Bug #: 11
### Date: 11/7/2022
### Symptom: 
    Whenever we do something, our terminal will print multiple times.
### Difficulty to find: 
    Easy. Since we know this is definitely from our keyboard function. 
### Difficulty to fix:
    Medium. Henry pointed the error location. The fix took awhile since we need to implement a way to prevent multiple press due to human interaction time and CPU respond time. 
---
## Bug #: 12
### Date: 11/18/2022
### Symptom: 
    When switching terminals, the terminals will be able to open, but the keyboard is freezing and we cannot type
### Difficulty to find: 
    Hard. We initially fail to find where the problem is. We discuss with other students, and we figure out that we need to send eoi when switching terminals.
### Difficulty to fix:
    Easy. Just add a line of endeoi when switching terminals. 
---
## Checkpoint 5
---
## Bug #: 13
### Date: 11/23/2022
### Symptom: 
    Open terminal will page fault immediately
### Difficulty to find: 
    Hard. We asked for help from both Okan and Edward. Okan helped us greatly by guessing and checking. Then we were able to identify the bug. 
### Difficulty to fix:
---
## Bug #: 14
### Date: 11/23/2022
### Symptom: 
    Open second, and third terminal will page fault immediately
### Difficulty to find: 
    
### Difficulty to fix:
    Our teammate Dongming was able to fix the second page fault, but we still have the third terminal that has page fault. 

---
## Bug #: 15
### Date: 11/23/2022
### Symptom: 
    Terminal one runs pingpong. Open terminal two next. When switching back to terminal one, pingpong stops and outputs page fault. Then, terminal one runs pingpong again, and we switches to terminal three. When we switches back from terminal three to terminal one, pingpong in terminal one stops and outputs general protection fault
### Difficulty to find: 
    It is hard to step through the entire program to see which part causes the exception. This only happened when all the terminals are not opened
    
### Difficulty to fix:
    To fix this, we need to set up esp and ebp before executing new shells

---
## Bug #: 15
### Date: 11/23/2022
### Symptom: 
    Printing has a glitch when constantly switching between terminals
### Difficulty to find: 
    Hard to find. The display terminal index and scheduled terminal index are not change correctly
    
### Difficulty to fix:
    Re-map the scheduling video cursor after the display memory finishes changing


