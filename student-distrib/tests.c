#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "paging.h"
#include "terminal.h"
#include "keyboard.h"
#include "file_system.h"

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
int test_terminal_read_full(){
	TEST_HEADER;
	uint8_t data[128]; // 128 because the buffer is only 128 byte long 
	memset(data, 0, sizeof(data));
	// int i;
	// printf("\n");
	
	// for(i = 0; i < 80; ++i){
    //     putc(key_buffer[i]);
    // }
	// 128 is the buffer size 
	terminal_read(0, data, 128);	// read the whole buffer
	terminal_write(0, data, 128);   // write the whole buffer
	// printf((int8_t*)key_buffer);
	// printf("\n");
	return PASS;
}

/* ls test
 * 
 * Test read_dentry_by_index
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: directory_read
 * Files: file_system.c
 */
int ls_test(){
	clear();
	TEST_HEADER;
	int i;
	uint8_t file_name[FILE_NAME_LENGTH+1];
	file_name[FILE_NAME_LENGTH] = '\0';

	for(i = 0; i<MAX_DENTRY_NUM; ++i){
		if(-1 != directory_read(0, file_name, FILE_NAME_LENGTH)) 
			printf("%s\n",file_name);
	}
	return PASS;
}

/* fish_frame_zero
 * 
 * Test read_dentry_by_name, read_data 
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: reading files and data in the files with small files 
 * Files: file_system.c
 */
int fish_frame_zero(){
	TEST_HEADER;
	char * file = "frame0.txt";
	char buf[10000]; // create a large buffer, default to 10000 so that the data can fit
	memset(buf,0,sizeof(buf));
	file_open((uint8_t*)file);
	file_read(0, buf, 0);
	printf("%s", buf);
	return PASS;
}

/* fish_frame_one
 * 
 * Test read_dentry_by_name, read_data 
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: reading files and data in the files with small files 
 * Files: file_system.c
 */
int fish_frame_one(){
	TEST_HEADER;
	char * file = "frame1.txt";
	char buf[10000]; // create a large buffer, default to 10000 so that the data can fit
	memset(buf,0,sizeof(buf));
	file_open((uint8_t*)file);
	file_read(0, buf, 0);
	printf("%s", buf);
	return PASS;
}

/* read_non_txt
 * 
 * Test read_dentry_by_name, read_data  
 * Inputs: char* file_name :	the name of the file
 *         int range:	number of bytes going to output
 * Outputs: PASS: for successful read
 * 	        FAIL: for invalid read range, invalid file
 * Side Effects: None
 * Coverage: file reading with various file names and file types
 * Files: file_system.c
 */
int read_non_txt(char* file_name, int range){
	TEST_HEADER;
	if(file_name == NULL){
		return FAIL;
	}
	if(range < 0){
		printf("Invalid range \n");
		return FAIL;
	}
	printf("\nfile name: %s \n", file_name);
	unsigned char buf[40000]; // create a large buffer, default to 40000 so that the data can fit
	memset(buf,0,sizeof(buf));
	file_open((uint8_t*)file_name);
	file_read(0, buf, 0);
	int i;
	for(i = 0; i < range; ++i){
		// if(i > 0 && !(i % 80)){	to print the stuff at the end
		// 	printf("\n");
		// }
		putc(buf[i]);
	}
	return PASS;
}

/* very_large_file_with_long_name_test
 * 
 * Test read_dentry_by_name, read_data  
 * Inputs: None
 * Outputs: PASS: file does not exist
 * 	        FAIL: file can be found and opened
 * Side Effects: None
 * Coverage: reading file with a name long then 32
 * Files: file_system.c
 */
int very_large_file_with_long_name_test(){
	TEST_HEADER;
	char * file = "verylargetextwithverylongname.txt";
	char buf[40000]; // create a large buffer, default to 40000 so that the data can fit
	memset(buf,0,sizeof(buf));
	if(-1 == file_open((uint8_t*)file)){
		printf("File does not exists\n");
		return PASS;
	}
	file_read(0, buf, 0);
	printf("%s", buf);
	return FAIL;
}

/* very_large_file_with_long_name_ok
 * 
 * Test read_dentry_by_name, read_data  
 * Inputs: None
 * Outputs: FAIL: file does not exist
 * 	        PASS: file can be found and opened
 * Side Effects: None
 * Coverage: reading file with a name long but with in the file name range
 * Files: file_system.c
 */
int very_large_file_with_long_name_ok(){
	TEST_HEADER;
	char * file = "verylargetextwithverylongname.tx";
	char buf[40000]; // create a large buffer, default to 40000 so that the data can fit
	memset(buf,0,sizeof(buf));
	if(-1 == file_open((uint8_t*)file)){
		printf("File does not exists\n");
		return FAIL;
	}
	file_read(0, buf, 0);
	printf("%s", buf);
	return PASS;
}

/* RTC test
 * 
 * Test RTC driver
 * Inputs: None
 * Outputs: 1
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.c
 */
// int rtc_test_2() {
// 	TEST_HEADER;
// 	rtc_open(NULL);
// 	int idx, loop, loop2, f;
// 	loop = 0;
// 	loop2 = 0;
// 	f = 2;	// base frequency is 2 
// 	while(loop2<5) { // loop 5 times
// 		while(loop < 10) { // loop 10 times
// 			idx = 0;
// 			while(idx < 8) {	// loop for outputing 8 char at a time 
// 				rtc_read();
// 				putc('1');
// 				idx++;
// 			}
// 			f = 2 * f;	// increase frequency
// 			rtc_write(&f);
// 			loop ++;
// 		}
// 		loop2++;
// 		loop = 0;
// 		// putc('\n');
// 	}
// 	rtc_close();
// 	return 1;
// }



/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	clear();

	//TEST_OUTPUT("idt_test", idt_test());

	printf("\n\n");
	// TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here

	// checkpoint 1 test
	/* expect screen to be constantly changing */
	// TEST_OUTPUT("rtc_test", rtc_test());
	/* expect to have div by 0 exception */
	// TEST_OUTPUT("divide by 0 test", div_zero_test());
	/* expect to have system call and program stay in a loop */
	// TEST_OUTPUT("System call test", system_call_test());
		
	/* Checkpoint 2 test start here */

	/* expect screen to print "1" with increasing speed */
	// TEST_OUTPUT("rtc_test_2", rtc_test_2());

	/* output all the file within the directory including "." */
	// TEST_OUTPUT("ls_test", ls_test());
	
	// /* output frame0.txt file */
	// TEST_OUTPUT("fish frame zero", fish_frame_zero());
	
	// /* ouptut frame1.txt file */
	// TEST_OUTPUT("fish frame one", fish_frame_one());
	
	// /* fish file test */
	// this test is strange, 36000 is an approximation of fish file size
	// TEST_OUTPUT("fish file test", read_non_txt("fish", 1000));
 	// printf(" =============================================================== \n");
		
	// /* grep exe read test */
	// TEST_OUTPUT("grep file test", read_non_txt("grep", 2000));
 	// printf(" =============================================================== \n");
	
	/* ls exe read test */
	// default reading range to 2000 Bytes
	// TEST_OUTPUT("ls file test", read_non_txt("ls", 2000));
 	// printf(" =============================================================== \n");
	
	/* output verylargetextwithverylongname.txt, only pass when the file cannot be opened */
	// TEST_OUTPUT("long name file test", very_large_file_with_long_name_test());
	
	/* successfully read the file and output the content */
	// TEST_OUTPUT("read the long name file", very_large_file_with_long_name_ok());
	// printf(" =============================================================== \n");
	// TEST_OUTPUT("test terminal read full", test_terminal_read_full());
}
