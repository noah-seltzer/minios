/**
*     	main.c
*
*				Kernel's Entry Point
*
*/

// #include <stdbool.h>			// Neede for bool
// #include <stdint.h>				// Needed for uint32_t, uint16_t etc
// #include <string.h>				// Needed for memcpy
// #include <stdarg.h>				// Needed for variadic arguments
// #include "drivers/stdio/emb-stdio.h"			// Needed for printf
// #include "boot/rpi-smartstart.h"		// Needed for smart start API
// #include "drivers/sdcard/SDCard.h"
// #include "hal/hal.h"
#define MINIOS_APP __attribute__((section(".app_entry")))
#define WRITE_CHAR_CODE 0
#define READ_CHAR_CODE 1

// void justPrintSomething(int i) {
// 	asm volatile ("mov x6, 'h'");
// 	asm volatile ("svc #0x0A");
// }

void sendInterrupt(int code, char c);

void printChar(char c) {
	sendInterrupt(WRITE_CHAR_CODE, c);
}

void printCharArray(char * c) {
	int i = 0;
	while (c[i] != '\0') {
		printChar(c[i]);
		i++;
	}
}


MINIOS_APP int main (void) {
	//char arr[] = "Please clap\n";
	printCharArray("howdy\n");
	//print(arr);
	// justPrintSomething(3);

	// justPrintSomething(6);
	// asm volatile ("svc #0x0A");

	// asm volatile ("svc #0x0A");

	// asm volatile ("mov x0, 'o'");
	
	// asm volatile ("svc #0x0A");

	// asm volatile ("mov x0, 'w'");
	
	// asm volatile ("svc #0x0A");

	// asm volatile ("mov x0, 'd'");
	
	// asm volatile ("svc #0x0A");

	// asm volatile ("mov x0, 'y'");
	
	// asm volatile ("svc #0x0A");

	// asm volatile ("mov x0, '\n'");
	
	// asm volatile ("svc #0x0A");
	return(5);
}

