/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"

/* Initialize RTC */
void rtc_init(void) {
    outb(REG_B | DIS_NMI, RTC_PORT);		// select register B, and disable NMI
    char prev=inb(RTC_DATA);	// read the current value of register B
    outb(REG_B | DIS_NMI, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
}

/* Handle RTC interrupt */
void rtc_handler(void) {
    send_eoi(RTC_IRQ_NUM);
    test_interrupts();
    outb(REG_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
}
