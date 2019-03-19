/**
*     	main.c
*
*				Kernel's Entry Point
*
*/
#include <stdlib.h>
#include <stdbool.h>			// Neede for bool
#include <stdint.h>				// Needed for uint32_t, uint16_t etc
#include <string.h>				// Needed for memcpy
#include <stdarg.h>				// Needed for variadic arguments
#include "drivers/stdio/emb-stdio.h"			// Needed for printf
#include "boot/rpi-smartstart.h"		// Needed for smart start API
#include "drivers/sdcard/SDCard.h"
#include "hal/hal.h"

char command_buffer[500];
char working_directory[500];
uint8_t directory_index = 0;

void DisplayDirectory(const char*);
int ProcessCommand(char word [], uint8_t len);
char * GetCurrentDirectory(char subdir []);

int main (void) {
	PiConsole_Init(0, 0, 0, &printf);								// Auto resolution console, show resolution to screen

	displaySmartStart(&printf);
	printf("\n");															// Display smart start details
	ARM_setmaxspeed(&printf);										// ARM CPU to max speed and confirm to screen

	/* Display the SD CARD directory */
	sdInitCard (&printf, &printf, true);
	printf("\n");
	DisplayDirectory("\\*.*");
	/* Display root directory */
	/*printf("Directory (/): \n");
	DisplayDirectory("\\*.*");

	printf("\n");
  printf("Opening Alice.txt \n");
  */

	/*
	HANDLE fHandle = sdCreateFile("Alice.txt", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fHandle != 0) {
		uint32_t bytesRead;

		if ((sdReadFile(fHandle, &bcommand_uffer[0], 500, &bytesRead, 0) == true))  {
				bcommand_uffer[bytesRead-1] = '\0';  ///insert null char
				printf("File Contents: %s", &bcommand_uffer[0]);
		}
		else{
			printf("Failed to read" );
		}

		// Close the file
		sdCloseHandle(fHandle);

	}*/

	hal_io_video_init();

	hal_io_video_puts( "HELLO THERE ", 3, VIDEO_COLOR_WHITE );

	//Typewriter
	hal_io_serial_init();
	hal_io_serial_puts( SerialA, "Typewriter:" );

	uint8_t c;
	uint8_t command_buffer_index = 0;

	while(command_buffer_index < 75){
		c = hal_io_serial_getc( SerialA );
		hal_io_serial_putc( SerialA, c );

		//if the enter key is encountered
		if(c == '\n' || c == '\r') {
			if (command_buffer_index > 0) {

				char word[command_buffer_index];
				for(size_t i = 0; i < command_buffer_index; i++)
				{
					word[i] = command_buffer[i];
				}

				printf("\n\r");

				int result = ProcessCommand(word, command_buffer_index);

				command_buffer_index = 0;
			}
		} else {
			printf( "%c", c );
			command_buffer[command_buffer_index++] = c;
		}
	}
	/* display bitmap on screen */
	//DisplayBitmap(743, 624, "./MINIOS.BMP");   //<<<<-- Doesn't seem to work

	while (1){
		set_Activity_LED(1);			// Turn LED on
		timer_wait(500000);				// 0.5 sec delay
		set_Activity_LED(0);			// Turn Led off
		timer_wait(500000);				// 0.5 sec delay
    }
	return(0);
}



int ProcessCommand(char word [], uint8_t len) {
	uint8_t i = 0;
	char *command = strtok(word, " ");
	if (strcmp(command, "ls") == 0 || strcmp(command, "LS") == 0) {
		char * argument = strtok(NULL, " ");
		char * str = GetCurrentDirectory(argument);
		printf(str);
		//DisplayDirectory(GetCurrentDirectory(argument));
		//TODO make this process ls arguments (IE ls /home)
	} else {
		printf("%s\n", "Command not recognized");
	}
	return 0;
}

char * GetCurrentDirectory(char subdir []) {
	
	char * str = (char *) malloc(45);
	strcpy(str, "this is a directory");
	return str;
}

void DisplayDirectory(const char* dirName) {
	HANDLE fh;
	FIND_DATA find;
	char* month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	fh = sdFindFirstFile(dirName, &find);							// Find first file
	do {
		if (find.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			printf("%s <DIR>\n", find.cFileName);
		else printf("%c%c%c%c%c%c%c%c.%c%c%c Size: %9lu bytes, %2d/%s/%4d, LFN: %s\n",
			find.cAlternateFileName[0], find.cAlternateFileName[1],
			find.cAlternateFileName[2], find.cAlternateFileName[3],
			find.cAlternateFileName[4], find.cAlternateFileName[5],
			find.cAlternateFileName[6], find.cAlternateFileName[7],
			find.cAlternateFileName[8], find.cAlternateFileName[9],
			find.cAlternateFileName[10],
			(unsigned long)find.nFileSizeLow,
			find.CreateDT.tm_mday, month[find.CreateDT.tm_mon],
			find.CreateDT.tm_year + 1900,
			find.cFileName);										// Display each entry
	} while (sdFindNextFile(fh, &find) != 0);						// Loop finding next file
	sdFindClose(fh);												// Close the serach handle
}

//TODO Loader

int loader() {

}
