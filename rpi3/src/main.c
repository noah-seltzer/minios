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

#define SVC_00 0x00
#define SVC_01 0x01

// void __svc(0x00) svc_zero(const char *string);
// void __svc(0x01) svc_one(const char *string);

// int call_system_func(void) {    
// 	svc_zero("String to pass to SVC handler zero");    
// 	svc_one("String to pass to a different OS function");
// 	}


char command_buffer[500];
char working_directory[500];
uint8_t directory_index = 0;

void DisplayDirectory(const char*);
int ProcessCommand(char word [], uint8_t len);
char * GetCurrentDirectory(char subdir []);
void GetBinary(const char * fileName);
void executeSimpleApp(const char * fileName);
void tryToPassAChar(const char c);

int handleInterrupt(void){
	
  //printf("Please Clap\n");
	char int_code, arg1, c;
	//char arg2;
	asm volatile ("mov %[int_code], x0\n" : [int_code] "=r" (int_code));
	//printf("interrupt recieved, status code %d\n", int_code);
	switch (int_code) {
		case 0:
			asm volatile ("mov %[arg1], x1\n" : [arg1] "=r" (arg1));
			printf("%c", arg1);
			break;
		case 1:
			c = hal_io_serial_getc( SerialA );
			hal_io_serial_putc( SerialA, c );
			printf("%c", c);
			tryToPassAChar(c);
			
			break;
	}
	
	//asm volatile ("mov %[arg2], x0\n" : [arg2] "=r" (arg2));
	// asm volatile ("mov %[arg2], x1\n" : [arg2] "=r" (arg2)); 
	// printf("%c", arg2);
	// // printf("\n");
	// // printf("%s", *arg2);
	//asm volatile ("eret");
	//return arg1;
}

void letsTryForScience(char a) {
	//asm volatile ("mov %[arg1], x0\n" : [arg1] "=r" (arg1)); 
	asm volatile ("mov x0, 'c'");
	asm volatile ("svc #0x0A");
}

// __attribute__((naked)) void svc_handler(void);
// __attribute__((naked)) void svc_handler(void) {
// 	// printf("Please Clap\n");
// 	// return 0;
// 	uint32_t arg0, arg1, arg2, arg3, ret_address;
// 	asm volatile ( "mov %[arg0], r0 \n"
// 												"mov %[arg1], r1 \n"
// 												"mov %[arg2], r2 \n"
// 												"mov %[arg3], r3 \n"
// 												"mov %[ret_address], r14 \n"
// 												: [arg0] "=r" (arg0), // output
// 													[arg1] "=r" (arg1),
// 													[arg2] "=r" (arg2),
// 													[arg3] "=r" (arg3),
// 													[ret_address] "=r" (ret_address)
// 												: // no input
// 												: // Clobber list
// 	);
// }


int main (void) {
	PiConsole_Init(0, 0, 0, &printf);								// Auto resolution console, show resolution to screen

	displaySmartStart(&printf);
	printf("\n");															// Display smart start details
	ARM_setmaxspeed(&printf);										// ARM CPU to max speed and confirm to screen

	/* Display the SD CARD directory */
	sdInitCard (&printf, &printf, true);

	//letsTryForScience('c');
	//asm volatile ("svc #0x0A");

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
	printf("#");
	uint8_t c;
	uint8_t command_buffer_index = 0;

	//printf("%s\n", "Attempting to dump Sensors.bin");
	//GetBinary("app.bin");
	//executeSimpleApp("app.bin");
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
				word[command_buffer_index] = '\0';

				int result = ProcessCommand(word, command_buffer_index);

				command_buffer_index = 0;
				printf("\r\n#");
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
	//printf("Full command is %s\n", command);
	if (command[0] == '$') {
		char * bin = strtok(command, "$");
			executeSimpleApp(bin);
		
	} else if (strcmp(command, "ls") == 0 || strcmp(command, "LS") == 0) {
		char * argument = strtok(NULL, " ");
		char * str = GetCurrentDirectory(argument);
		printf(str);
		//DisplayDirectory(GetCurrentDirectory(argument));
		//TODO make this process ls arguments (IE ls /home)
	
	} else if (strcmp(command, "dump") == 0) {
		char * argument = strtok(NULL, " ");
		if (argument == NULL) {
			printf ("Please enter the name of a binary file");
		} else {
			GetBinary(argument);
		}
	} else {
		printf("\n%s", "Command not recognized");
	}
	return 0;
}

char * GetCurrentDirectory(char subdir []) {
	
	char * str = (char *) malloc(45);
	strcpy(str, "this is a directory");
	return str;
}


typedef int (*printhandler) (const char *fmt, ...);

void executeSimpleApp(const char * fileName) {
	
	HANDLE fHandle = sdCreateFile(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	printf("\nExecuting %s\n", fileName);
	if (fHandle != 0) {
		uint32_t bytesRead;
		uint32_t useless;
		int fileSize = sdGetFileSize(fHandle, &useless);
		char buffer[fileSize];
		//printf("File is %d bytes long\n", fileSize);
		
		if ((sdReadFile(fHandle, &buffer[0], fileSize, &bytesRead, 0) == true))  {
				
				
				int (*execution) (void) = &buffer;

				int pls = execution();
				printf("process executed with status code %d", pls);
				
				// uint32_t useless;
				// int fileSize = sdGetFileSize(fHandle, &useless);
				// printf("size of file: %d", fileSize);
				// for(size_t i = 0; i < bytesRead; i += 2)
				// {

				// 	printf("%x", (unsigned char) buffer[i]);
				// }
				
		}
		else{
			printf("Failed to read" );
		}

		// Close the file
		sdCloseHandle(fHandle);
	}
}


void GetBinary(const char * fileName) {
	
	HANDLE fHandle = sdCreateFile(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	printf("Getting binary for %s\n", fileName);
	if (fHandle != 0) {
		uint32_t bytesRead;
		uint32_t useless;
		int fileSize = sdGetFileSize(fHandle, &useless);
		char buffer[fileSize];
		//printf("File is %d bytes long\n", fileSize);
		
		if ((sdReadFile(fHandle, &buffer[0], fileSize, &bytesRead, 0) == true))  {
			uint32_t row = 0;
			uint32_t col = 0;
			//printf("          1    2    3    4    5    6    7    8\n");
			for(size_t i = 0; i < bytesRead; i++)
			{
				if (col == 0) {
					printf("%07x0  ", row++);
				}
				
				printf("%02x ", buffer[i] & 0xff);
				if(++col >= 16) {
					printf("\n");
					col = 0;
				}
			}
			
			
				//buffer[bytesRead-1] = '\0';  ///insert null char
				//printf("File Contents: %s", &buffer[0]);
				// printf("%x\n", buffer[0] & 0xff);
				// printf("%x\n", buffer[1] & 0xff);
				// printf("%x\n", buffer[2] & 0xff);
				// printf("%x\n", buffer[3] & 0xff);
				// printf("%x\n", buffer[4] & 0xff);
				// printf("%x\n", buffer[5] & 0xff);
				// printf("%x\n", buffer[6] & 0xff);

				// int (*execution) (void) = &buffer;

				// int pls = execution();
				// printf("process executed with status code %d \n", pls);
				
				// uint32_t useless;
				// int fileSize = sdGetFileSize(fHandle, &useless);
				// printf("size of file: %d", fileSize);
				// for(size_t i = 0; i < bytesRead; i += 2)
				// {

				// 	printf("%x", (unsigned char) buffer[i]);
				// }
				
		}
		else{
			printf("Failed to read" );
		}

		// Close the file
		sdCloseHandle(fHandle);
	}
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
