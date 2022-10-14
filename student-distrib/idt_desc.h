#ifndef _IDT_DESC_H
#define _IDT_DESC_H

#include "exception.h"
// initialize the idt table
extern void init_idt_desc();

extern void exception_handler(int idt_num);

#endif
