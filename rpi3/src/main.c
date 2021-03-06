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
char working_directory[500] = "\\";
//char working_directory[500] = "\\*.*";
char buffer[500];
uint8_t directory_index = 0;

void DisplayDirectory(const char*);
int ProcessCommand(char word [], uint8_t len);
char * GetCurrentDirectory(char subdir []);
void GetBinary(const char * fileName);
void executeSimpleApp(const char * fileName);
void catFile(const char* fileName);
void returnCharToExecutable(const char c);
void processCommandLoop(void);


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
			returnCharToExecutable(c);
			
			break;
	}
	
}


int main (void) {
	PiConsole_Init(0, 0, 0, &printf);								// Auto resolution console, show resolution to screen

	displaySmartStart(&printf);
	printf("\n");															// Display smart start details
	ARM_setmaxspeed(&printf);										// ARM CPU to max speed and confirm to screen

	/* Display the SD CARD directory */
	sdInitCard (&printf, &printf, true);

	printf("\n");
  
	/* Display root directory */
	// printf("Directory (/): \n");
	// DisplayDirectory("\\*.*");


	hal_io_video_init();

	hal_io_video_puts( "HELLO THERE ", 3, VIDEO_COLOR_WHITE );

	//Typewriter
	hal_io_serial_init();
	hal_io_serial_puts( SerialA, "Typewriter:" );
	
	printf("#");

	processCommandLoop();
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

void processCommandLoop(void) {
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
}



int ProcessCommand(char word [], uint8_t len) {
	uint8_t i = 0;
	char *command = strtok(word, " ");
	char *command_arg = strtok(NULL, " ");
	// printf("Full command is %s\n", command);

	if (command[0] == '$') {
		char * bin = strtok(command, "$");
			executeSimpleApp(bin);
	} else if (strcmp(command, "sysinfo") == 0) {
		printf("\nSystem information\n OS Name: TestOS\n OS Version: 2.0\n OS Manufacturer: Sad BCIT Students");
	} else if (strcmp(command, "ls") == 0 || strcmp(command, "LS") == 0) {
		printf("\n%s", working_directory);
		char * argument = strtok(NULL, " ");
		char * str = GetCurrentDirectory(argument);

		char str_dir[500];
		strcpy(str_dir, working_directory);
		char *p = str_dir;
		strcpy(&str_dir[strlen(p)], "*.*");
		DisplayDirectory(str_dir);
	
	} else if (strcmp(command, "cat") == 0) {

		catFile(command_arg);

	} else if (strcmp(command, "cd") == 0) {
		// removes *.*
		char *p = working_directory;
		//p[strlen(p)-3] = 0;

		if (strcmp(command_arg, "..") == 0) {
			// if root
			if (strlen(working_directory) < 2) {
				strcpy(&working_directory, "\\");
				//strcpy(&working_directory, "\\*.*");
			} else {
				// if not root
				do {
					p[strlen(p)-1] = 0;
				} while (p[strlen(p)-1] != '\\');
				//strcpy(&working_directory[strlen(p)], "*.*");
			}
		} else {
			strcpy(&working_directory[strlen(p)], command_arg);
			strcpy(&working_directory[strlen(p)], "\\");
		}
		printf("\nworking directory: %s\n", working_directory);

	} else if (strcmp(command, "dump") == 0) {
		// char * argument = strtok(NULL, " ");
		if (command_arg == NULL) {
			printf ("\nPlease enter the name of a binary file");
		} else {
			GetBinary(command_arg);
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
	//printf("\nExecuting %s\n", fileName);
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
	printf("\n");
	do {
		if (find.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			printf("%s <DIR>\n\0", find.cFileName);
		else printf("%s \n\0", find.cFileName);									// Display each entry
	} while (sdFindNextFile(fh, &find) != 0);						// Loop finding next file
	sdFindClose(fh);											// Close the serach handle
}

void catFile(const char* fileName) {
	char file_dir[500];
	strcpy(file_dir, working_directory);
	char *p = file_dir;
	strcpy(&file_dir[strlen(p)], fileName);

	HANDLE fHandle = sdCreateFile(file_dir, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (fHandle != 0) {
		uint32_t bytesRead;

		if ((sdReadFile(fHandle, &buffer[0], 500, &bytesRead, 0) == true))  {
			buffer[bytesRead-1] = '\0';  ///insert null char
			printf("\nFile Contents: %s", &buffer[0]);
		} else {
			printf("\nFailed to read");
		}
		// Close the file
		sdCloseHandle(fHandle);
	} else {
		printf("\nInvalid file to read");
	}
}

