/* skeleton of the simplified shell */

#include"shell.h"
#include"login.h"
#include<windows.h>

HANDLE	handle_in;
HANDLE	handle_out;
DWORD dw;
/* TODO */

int console() {
	// I/O handle Windows API
	handle_in = GetStdHandle(STD_INPUT_HANDLE);
	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);

	int control = 1;
	// main loop
	while (control) {
		// get current directory
		char *current_directory = (char*)malloc(sizeof(char) * MAX_PATH);
		GetCurrentDirectory(MAX_PATH, current_directory);
		// display current directory
		WriteConsole(handle_out, current_directory, strlen(current_directory), &dw, NULL);
		WriteConsole(handle_out, "$ ", strlen("$ "), &dw, NULL);

		// define a buffer to receive command and argv
		char *command = (char*)malloc(sizeof(char) * 512);  //1KB
		char *argv[8];  //max 8 argv(8*64 = 512)
		int *argc = (int*)malloc(sizeof(int*));

		// get command from console input
		ReadConsole(handle_in, command, 512, &dw, NULL);
		getArgv(command, argv, argc);
		if ((*argc) == -1) continue;
		else if (!strcmp(argv[0], "more")) {
			more(argv, argc);
		}
		else if (!strcmp(argv[0], "sort")) {
			sort(argv, argc);
		}
		// else if (!strcmp(argv[0], "?")) {
		// TODO
		// }
		else {
			WriteConsole(handle_out, "Command not found\n", strlen("Command not found\n"), &dw, NULL);
			continue;
		}
	}

	CloseHandle(handle_out);
	CloseHandle(handle_in);

	return 0;
}

int main() {
	if (!login()) {
		WriteConsole(handle_out, "\nLOGIN SUCCEED\n", strlen("\nLOGIN SUCCEED\n"), &dw, NULL);
	}
	system("pause");

	//CloseHandle(handle_in);
	//CloseHandle(handle_out);

	//console();
	return 0;
}
