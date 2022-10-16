/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define MASTER_8259_DATA    (MASTER_8259_PORT + 1)
#define SLAVE_8259_PORT     0xA0
#define SLAVE_8259_DATA     (SLAVE_8259_PORT + 1)

#define SLAVE_IRQ_NUM   0x02

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1                0x11
#define ICW2_MASTER         0x20
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04
#define ICW3_SLAVE          0x02
#define ICW4                0x01


// i8259 data sheet page 53 
// non specific EOI signal used 0x20
/* declare specific EOI signal */
/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */

#define EOI                 0x60

/* Externally-visible functions */

/* i8259_init(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: Initialize both PICs */
void i8259_init(void);

/* enable_irq(uint32_t irq_num)
 *   Inputs: uint32_t irq_num
 *   Return Value: none
 *   Function: Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);

/* disable_irq(uint32_t irq_num)
 *   Inputs: uint32_t irq_num
 *   Return Value: none
 *   Function: Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);

/* send_eoi(uint32_t irq_num)
 *   Inputs: uint32_t irq_num
 *   Return Value: none
 *   Function: Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
