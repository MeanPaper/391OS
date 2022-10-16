/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 * reference: https://wiki.osdev.org/PIC
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    uint32_t flags;
    cli_and_save(flags);

    /* Mask all IRQ */
    outb(0xff, MASTER_8259_DATA);
    outb(0xff, SLAVE_8259_DATA);

    /* Initialize the primary PIC*/
    outb(ICW1, MASTER_8259_PORT);         // Start PIC in cascade mode
    outb(ICW2_MASTER, MASTER_8259_DATA);  // Master offset
    outb(ICW3_MASTER, MASTER_8259_DATA);  // Tell master about the slave
    outb(ICW4, MASTER_8259_DATA);         // Set mode for pics: 8086/88 (MCS-80/85) mode

    /* Initialize the secondary PIC*/
    outb(ICW1, SLAVE_8259_PORT);          // Start PIC in cascade mode
    outb(ICW2_SLAVE, SLAVE_8259_DATA);    // Slave offset
    outb(ICW3_SLAVE, SLAVE_8259_DATA);    // Tell slave its identitiy
    outb(ICW4, SLAVE_8259_DATA);          // Set mode for pics: 8086/88 (MCS-80/85) mode

    /* Restored saved masks (enable IRQ) */
    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);

    /* Enable interrupts from the secondary PIC*/
    enable_irq(SLAVE_IRQ_NUM);
    
    restore_flags(flags);
    sti();
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    if (irq_num > 15 || irq_num < 0) return;    // invalid irq_num
    if (irq_num < 8) {
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= 8;
    }
    value = inb(port) & ~(1 << irq_num);    // clear the corresponding mask bit
    if(MASTER_8259_DATA == port) master_mask = value;
    else slave_mask = value;
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    if (irq_num > 15 || irq_num < 0) return;    // invalid irq_num
    if (irq_num < 8) {
        port = MASTER_8259_DATA;
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);     // set the corresponding mask bit
    if(MASTER_8259_DATA == port) master_mask = value;
    else slave_mask = value;
    outb(value, port);
}

// reference: i8259 data sheet page 15, END OF INTERRUPT
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8){
        // eoi to slave
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
    }
    // eoi to master
    outb(EOI | ((irq_num >= 8) ? SLAVE_IRQ_NUM : irq_num), MASTER_8259_PORT);
}
