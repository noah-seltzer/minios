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

char readChar(void) {
	char c;
	sendInterrupt(READ_CHAR_CODE, c);
	// c = _read_char_from_register();
	asm volatile ("mov %[c], x13\n" : [c] "=r" (c));
	return c;
}


MINIOS_APP int main (void) {
	
	//char c;
	// printChar('h');
	// printChar('e');
	// printChar('l');
	// printChar('l');
	// printChar('o');
	// printChar(' ');
	// printChar('w');
	// printChar('o');
	// printChar('r');
	// printChar('l');
	// printChar('d');
	// printChar('\n');


	// c = readChar();

	// printChar('\n');
	// printChar('y');
	// printChar('o');
	// printChar('u');
	// printChar(' ');
	// printChar('t');
	// printChar('y');
	// printChar('p');
	// printChar('e');
	// printChar('d');
	// printChar(':');
	// printChar(' ');
	// printChar(c);
	// printChar('\n');

	return(2);
}

