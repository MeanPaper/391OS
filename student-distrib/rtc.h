/* rtc.h - Defines used in interactions with the RTC
 * vim:ts=4 noexpandtab
 * reference: https://wiki.osdev.org/RTC
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "i8259.h"

#define RTC_IRQ_NUM 8

/* Ports */
#define RTC_PORT    0x70    // used to specify an index or "register number", and to disable NMI
#define RTC_DATA    (RTC_PORT+1)    // used to read or write from/to that byte of CMOS configuration space

#define DIS_NMI 0x80
/* RTC Status Register, with NMI disabled */
#define REG_A   0xA
#define REG_B   0xB
#define REG_C   0xC
/* some constants */
#define MIN_RATE    3
#define MAX_FREQ    1024
#define MIN_FREQ    2


/* Externally-visible functions */

/* rtc_init(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: Initialize RTC */
extern void rtc_init(void);

/* rtc_open(void)
 *   Inputs: none
 *   Return Value: 0
 *   Function: Initialize RTC frequency to 2Hz */
extern int rtc_open(const uint8_t*);

/* rtc_read(void)
 *   Inputs: none
 *   Return Value: 0
 *   Function: Block until the next interrupt */
extern int rtc_read(int32_t fd, void* buf, int32_t nbytes);

/* rtc_write(int frequency)
 *   Inputs: int frequency - how fast the RTC generates interrupts
 *   Return Value: 0 if success; -1 if fail
 *   Function: Set the frequency of the RTC interrupts generations */
extern int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

/* rtc_close(void)
 *   Inputs: none
 *   Return Value: 0
 *   Function: Nothing */
extern int rtc_close(int32_t);

/* rtc_handler(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: RTC interrupts handler */
extern void rtc_handler(void);

#endif /* _RTC_H */
