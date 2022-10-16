/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"

int log2_helper(unsigned int input);

/* Initialize RTC */
void rtc_init(void) {
    enable_irq(RTC_IRQ_NUM);
    outb(REG_B | DIS_NMI, RTC_PORT);		// select register B, and disable NMI
    char prev=inb(RTC_DATA);	// read the current value of register B
    outb(REG_B | DIS_NMI, RTC_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
}

/* Change frequency */
// frequency =  32768 >> (rate-1);
void rtc_change_rate(int frequency) {
    int rate = 16 - log2_helper(frequency); // the higher the rate, the smaller the frequency
    if (rate > 2 && rate < 16) {
        cli();
        outb(REG_A | DIS_NMI, RTC_PORT);
        char prev = inb(RTC_DATA);
        outb(REG_A | DIS_NMI, RTC_PORT);
        outb((prev & 0xF0) | rate, RTC_DATA); // rate is the buttom 4 bits
        sti();
    }
}

/* Handle RTC interrupt */
void rtc_handler(void) {
    cli();
    send_eoi(RTC_IRQ_NUM);

    // use to test rtc refleshing screen
    // test_interrupts();
    
    outb(REG_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
    sti();
}

int log2_helper(unsigned int input) {
    int count = 0;
    int curr_num = 1;
    while (curr_num <= input) {
        curr_num = curr_num * 2;
        count++;
    }
    count--;
    return count;
}