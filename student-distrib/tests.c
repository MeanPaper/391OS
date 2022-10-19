#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "paging.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
/* Divide by zero test
 * 
 * Test divide by zero exception for exception handler. 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT Exception handler
 * Files: idt.c
 */

// add more tests here
/* Checkpoint 1 tests*/
int div_zero_test(){
	TEST_HEADER;
	int j = 0;
	int i = 10 / j; // raise divide by 0 exception
	return i ? PASS: FAIL;
}

// /* RTC test
//  * 
//  * Test to set and enable RTC. 
//  * Inputs: None
//  * Outputs: 1
//  * Side Effects: None
//  * Coverage: RTC
//  * Files: rtc.c
//  */
// int rtc_test() {
// 	TEST_HEADER;
// 	rtc_change_rate(8);
// 	return 1;
// }

/* Derefrence NULL test
 * 
 * Test derefrence a null pointer for exception handler. 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: IDT Exception handler
 * Files: idt.c
 */
int derefence_null(){  // needs to have page set up before testing, should trigger page fault
	TEST_HEADER;
	int * j = NULL;
	printf("%d", *j);
	return (*j) ? PASS: FAIL;
}

/* System call  test
 * 
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: System Call
 */
int system_call_test(){
	TEST_HEADER;
	asm volatile("int $0x80");
	return PASS;
}


/* paging test
 * 
 * Check if memory address can be derefrenced. 
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: memory from paging. 
 * Files: paging.c
 */

int paging_test_no_fault(){
	TEST_HEADER;
	uint32_t * i = &page_directory[1];
	uint32_t * j = &first_page_table[0xB8000 >> 12];
	uint32_t x = *i;
	uint32_t y = *j;
	printf("ignore values (for eliminating warnings): \n %d \n %d", x, y);
	return PASS;
}


/* Overflow test
 * 
 * Test Overflow for exception handler. 
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: IDT Exception handler
 * Files: idt.c
 */
int overflow_exception_test(){
	TEST_HEADER;
	asm volatile("int $4");

	return PASS;
}
/* Checkpoint 2 tests */

/* RTC test
 * 
 * Test to set and enable RTC. 
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.c
 */
int rtc_test() {
	TEST_HEADER;
	rtc_open();
	int idx, loop, f;
	loop = 0;
	f = 2;
	while(loop < 10) {
		idx = 0;
		while(idx < 4) {
			rtc_read();
			putc('1');
			idx++;
		}
		f = 2 * f;
		rtc_write(&f);
		loop ++;
	}
	rtc_close();
	putc('\n');
	return 1;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	clear();
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here

	// checkpoint 1 test
	/* expect screen to be constantly changing */
	TEST_OUTPUT("rtc_test", rtc_test());
	
	/* expect to have div by 0 exception */
	// TEST_OUTPUT("divide by 0 test", div_zero_test());

	/* expect to have system call and program stay in a loop */
	// TEST_OUTPUT("System call test", system_call_test());

	/* expect to have a page fault */
	// TEST_OUTPUT("deference null pointer", derefence_null()); 

	/* expect to have no page fault */
	// TEST_OUTPUT("deference correct page pointer", paging_test_no_fault()); 

	/* expect to have overflow exception */
	// TEST_OUTPUT("overflow exception rise", overflow_exception_test()); 
}
