#include "schedule.h"

#define SQR_WAVE_MODE 0x33      // square wave mode, use by most of the OS
#define PIT_APPR_MAX  1193180   // this is approximately the highest requency for the pit


void pit_init(){
    // cli();
    
    // outb(SQR_WAVE_MODE, PIT_CMD_PORT);
    // outb( freq & 0xFF, PIT_DATA_0); // sending low bytes
    // outb( freq     >> 8, PIT_DATA_0); // sending high bytes
    // enable_irq(PIT_IRQ_POS);

    // sti();
}
