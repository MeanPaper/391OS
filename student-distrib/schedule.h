#ifndef _SCHEDULE_
#define _SCHEDULE_

#include "types.h"
#include "i8259.h"

#define PIT_DATA_0      0x40
#define PIT_DATA_1      0x41
#define PIT_DATA_2      0x42
#define PIT_MODE_PORT   0x43
#define PIT_IRQ_POS     0


// initialize pit
void pit_init();

// pit handler


// scheduler 

#endif
