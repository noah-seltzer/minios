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

typedef int (*printhandler) (const char *fmt, ...);





MINIOS_APP int main (printhandler print_handler) {
	char arr[] = "Please clap\n";
	int i = 0;
	while(arr[i] != '\0') {
		print_handler("%c", arr[i++]);
	}
	return(3);
}

