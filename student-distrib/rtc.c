/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 * reference: https://wiki.osdev.org/RTC
 */

#include "rtc.h"
#include "lib.h"

/* Local function */
static int log2_helper(int input);
static void rtc_change_rate(int frequency);

/* Local variable */
int flag;

/* Initialize RTC */
void rtc_init(void) {
    enable_irq(RTC_IRQ_NUM);
    outb(REG_B | DIS_NMI, RTC_PORT);	// select register B, and disable NMI
    char prev=inb(RTC_DATA);	        // read the current value of register B
    outb(REG_B | DIS_NMI, RTC_PORT);	// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
}

/* Handle RTC interrupt */
void rtc_handler(void) {
    cli();
    send_eoi(RTC_IRQ_NUM);

    // use to test rtc refleshing screen
    // test_interrupts();
    flag = 0;   // clear flag
    
    outb(REG_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
    sti();
}

/* Initialize RTC frequency to 2Hz */
int rtc_open(const uint8_t* file) {
    // rate = 16-log2(f) = 16-1 = 15
    rtc_change_rate(15);
    return 1;
}

/* Block until the next interrupt */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    sti();
    flag = 1;   // set flag
    while(flag==1);
    return 1;
}

/* Set the frequency */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    int rtc_f;
    memcpy(&rtc_f, (int*)buf, sizeof(int));
    int rate = 16 - log2_helper(rtc_f);     // convert frequency to rate
    if (rate <=2 || rate >=16) return -1;   // input is not power of 2 or rate is not in the valid range
    rtc_change_rate(rate);
    return 0;
}

int rtc_close(int32_t fd) {
    return 0;
}

/* rtc_change_rate(int rate)
 *   Inputs: none
 *   Return Value: int rate - how fast the RTC generates interrupts, frequency =  32768 >> (rate-1)
 *   Function: Set the frequency of the RTC interrupts generations */
void rtc_change_rate(int rate) {
    cli();
    outb(REG_A | DIS_NMI, RTC_PORT);
    char prev = inb(RTC_DATA);
    outb(REG_A | DIS_NMI, RTC_PORT);
    outb((prev & 0xF0) | rate, RTC_DATA); // rate is the buttom 4 bits
    sti();
}

/* log2_helper(int input)
 *   Inputs: int input
 *   Return Value: int count
 *   Function: Compute count = log2(input) */
int log2_helper(int input) {
    int count = 0;
    int curr_num = 1;
    while (curr_num != input) {
        curr_num = curr_num * 2;
        count++;
        if (curr_num>input) return -1;
    }
    return count;
}
