/* implementation of the commands */

#include<windows.h>

void getArgv(char *command, char *argv[8], int *argc) {
	// index for scaning command
	char *command_index = command;

	// scan command string and split to argv
	for ((*argc) = 0; (*command_index) != '\n' && (*command_index) != '\r' && (*argc) < 8; (*argc)++) {
		// get argv
		int index = 0;
		// save the split argv
		argv[(*argc)] = (char*)malloc(sizeof(char) * 64);
		// if character is space or enter or alt than move
		while ((*command_index) == ' ')	{
			command_index++;
		}
		// remove space enter alt
		while ((*command_index) != ' ' && (*command_index) != '\n' && (*command_index) != '\r' && (*command_index) != '\0')	{
			// judge command character and save to the split argv
			*(argv[(*argc)] + index) = (*command_index);
			command_index++;
			index++;
		}
		// end of each argv
		*(argv[(*argc)] + index) = '\0';
	}
	argv[*(argc)] = '\0';
	(*argc)--;

}

/* TODO */
