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
int rtc_f;
int count = 0;

/* Initialize RTC */
void rtc_init(void) {
    enable_irq(RTC_IRQ_NUM);
    outb(REG_B | DIS_NMI, RTC_PORT);	// select register B, and disable NMI
    char prev=inb(RTC_DATA);	        // read the current value of register B
    outb(REG_B | DIS_NMI, RTC_PORT);	// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    rtc_change_rate(3);                 // set to maximum frequency: 1024Hz
}

/* Handle RTC interrupt */
void rtc_handler(void) {
    cli();
    send_eoi(RTC_IRQ_NUM);

    // use to test rtc refleshing screen
    // test_interrupts();
    if (count > 1024/rtc_f) {
        flag = 0;
        count = 0;
    } else {
        count ++;
    }

    outb(REG_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
    sti();
}

/* Initialize RTC frequency to 2Hz */
int rtc_open(void) {
    rtc_f = 2;  // set frequency
    return 1;
}

/* Block until the next interrupt */
int rtc_read(void) {
    flag = 1;
    while(flag==1);
    return 1;
}

/* Set the frequency */
int rtc_write(int* frequency) {
    int input_f;
    memcpy(&input_f, frequency, sizeof(int)); // set frequency
    if (input_f>0 && input_f<=8192) {
        rtc_f = input_f;
    } else {
        return -1;
    }
    return 0;
}

int rtc_close(void) {
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
