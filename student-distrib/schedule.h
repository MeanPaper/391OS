#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

#define PIT_DATA_0      0x40
#define PIT_DATA_1      0x41
#define PIT_DATA_2      0x42
#define PIT_CMD_PORT    0x43
#define PIT_IRQ_POS     0


// initialize pit
void pit_init();

// pit handler
void pit_handler();

// scheduler 
void scheduler();

#endif
