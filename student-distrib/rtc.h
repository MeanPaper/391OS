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

/* Externally-visible functions */

/* rtc_init(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: Initialize RTC */
extern void rtc_init(void);

/* rtc_handler(void)
 *   Inputs: none
 *   Return Value: none
 *   Function: RTC interrupts handler */
extern void rtc_handler(void);

/* rtc_change_rate(int frequency)
 *   Inputs: none
 *   Return Value: int frequency - how fast the RTC generates interrupts
 *   Function: Set the frequency of the RTC interrupts generations */
extern void rtc_change_rate(int frequency);

#endif /* _RTC_H */
