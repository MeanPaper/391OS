/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 * reference: https://wiki.osdev.org/RTC
 */

#include "rtc.h"
#include "lib.h"
#include "terminal.h"

/* Local function */
// static int log2_helper(int input);
static void rtc_change_rate(int frequency);

/* Local variable */
volatile int flag;
int rtc_f;  // frequency, stored in terminal struct
int count = 0;

/* Initialize RTC */
void rtc_init(void) {
    outb(REG_B | DIS_NMI, RTC_PORT);	// select register B, and disable NMI
    char prev=inb(RTC_DATA);	        // read the current value of register B
    outb(REG_B | DIS_NMI, RTC_PORT);	// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    rtc_change_rate(MIN_RATE);                 // set to maximum frequency: 1024Hz (the corresponding rate is 3)
    enable_irq(RTC_IRQ_NUM);

    rtc_f = MIN_FREQ;
}

/* Handle RTC interrupt */
void rtc_handler(void) {
    send_eoi(RTC_IRQ_NUM);
    int i;
    // use to test rtc refleshing screen
    // test_interrupts();
    for(i = 0; i < 3; ++i){
        // handle in-used rtc in all three terminals
        if (terms[i].rtc_frequency != -1) {
            if (terms[i].rtc_counter >= MAX_FREQ/terms[i].rtc_frequency) {
                terms[i].rtc_flag = 0;      // set flag to 0, could return from read
                terms[i].rtc_counter = 0;   // reset counter
            } else {
                terms[i].rtc_counter ++;
            }
        }
    }
    
    outb(REG_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
}

/* Initialize RTC frequency to 2Hz */
int rtc_open(const uint8_t* file) {
    terms[current_term_id].rtc_frequency = MIN_FREQ;
    terms[current_term_id].rtc_counter = 0;
    // rtc_f = MIN_FREQ;  // set frequency
    return 1;
}

void return_from_read() {
    return;
}

/* Block until the next interrupt */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
    sti();
    terms[current_term_id].rtc_flag = 1;
    while(terms[current_term_id].rtc_flag == 1);
    return_from_read();
    return 0;
}

/* Set the frequency */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    int input_f;
    if (buf == NULL) return -1;
    if (nbytes != sizeof(int)) return -1;
    memcpy(&input_f, (int*)buf, sizeof(int));
    if (input_f<MIN_FREQ || input_f>MAX_FREQ) return -1;
    terms[current_term_id].rtc_frequency = input_f;
    return 0;
}

int rtc_close(int32_t fd) {
    terms[current_term_id].rtc_frequency = MIN_FREQ;
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
