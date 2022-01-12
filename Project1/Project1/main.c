/* skeleton of the simplified shell */

#include"commd.h"
#include"login.h"
#include<windows.h>

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

int console(char* uid) {
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
		char *argv[MAX_ARG_NUM];  //max MAX_ARG_NUM argv(MAX_ARG_NUM*64 = 512)
		int *argc = (int*)malloc(sizeof(int*));
		arg arg[MAX_ARG_NUM]; // argument queue

		int count_q;

		// get command from console input
		ReadConsole(handle_in, command, 512, &dw, NULL);
		getArgv(command, argv, argc);
		if ((*argc) == -1) continue;
		else if (!strcmp(argv[0], "passwd")) {
			//WriteConsole(handle_out, uid, strlen(uid), &dw, NULL);
			passwd(argv[1], uid);
			continue;
		}
		else if (!strcmp(argv[0], "logout")) {
			return;
		}
		count_q = parser(argv, argc, commd_queue, arg);
		if (!count_q) {
			WriteConsole(handle_out, "Command not found\n", strlen("Command not found\n"), &dw, NULL);
			continue;
		}
		else {
			process_queue(commd_queue, arg);
		}
	}

	CloseHandle(handle_out);
	CloseHandle(handle_in);

	return 0;
}

int main(int argc, char* argv[]) {
	//HANDLE hReadPipe = NULL;
	//HANDLE hWritePipe = NULL;
	//handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	//char *uid = GetCommandLineA();
	//WriteConsole(handle_out, uid, strlen(uid), &dw, NULL);

	char*uid = "1";
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	console(uid);

	CloseHandle(handle_in);
	CloseHandle(handle_out);

	return 0;
}
