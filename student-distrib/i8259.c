/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* Save masks, and mask all IRQ */
    unsigned char a1, a2;
    a1 = inb(MASTER_8259_DATA);
    a2 = inb(SLAVE_8259_DATA);
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
    outb(MASTER_8259_DATA, a1);
    outb(SLAVE_8259_DATA, a2);

    enable_irq(SLAVE_IRQ_NUM);
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
    outb(port, value);
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
    outb(port, value);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8){
        outb(SLAVE_8259_PORT, EOI);
    }
    outb(MASTER_8259_PORT, EOI);
}
