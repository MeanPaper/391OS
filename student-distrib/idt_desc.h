#ifndef _IDT_DESC_H
#define _IDT_DESC_H

/* initialize IDT */
extern void init_idt_desc();

/* handling exception message, for checkpoint 1 it only print */
extern void exception_handler(int idt_num);

#endif
