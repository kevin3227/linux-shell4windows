/* process commands */

#include"commd.h"

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

extern HANDLE hReadPipe = NULL;
extern HANDLE hWritePipe = NULL;

extern SECURITY_ATTRIBUTES sa;
extern STARTUPINFO si;
extern PROCESS_INFORMATION pi;

int redir_flag = 0, count_a, count_q;

void getArgv(char *command, char *argv[MAX_ARG_NUM], int *argc) {
	// index for scaning command
	char *command_index = command;

	// scan command string and split to argv
	for ((*argc) = 0; 
	(*command_index) != '\n' 
	&& (*command_index) != '\r' 
	&& (*argc) < MAX_ARG_NUM; (*argc)++) {
		// get argv
		int index = 0;
		// save the split argv
		argv[(*argc)] = (char*)malloc(sizeof(char) * 64);
		// if character is space or enter or alt than move
		while ((*command_index) == ' ') {
			command_index++;
		}
		// remove space enter alt
		while (
		(*command_index) != ' ' 
		&& (*command_index) != '\n' 
		&& (*command_index) != '\r' 
		&& (*command_index) != '\0') {
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

// search the command table
static inline int search_commd(char* commd) {
	for (int i = 0; i <= COMMD_NUM - 1; i++) {
		if (!strcmp(commd, commd_table[i])) return i;
	}
	return -1;
}

//static inline void str_cpy(char* argv[MAX_ARG_NUM], arg arg[MAX_ARG_NUM], int count_a, int count_q) {
//	int i = 0;
//	while (argv[count_a][i] != '\0' && i < strlen(argv[count_a])) {
//		arg[count_q].output[i] = argv[count_a][i];
//		i++;
//	}
//}

// set progress
void set_process(int count_c ,arg arg[MAX_ARG_NUM]) {
	int pos;
 	if (!count_c && count_c != count_q - 1) pos = FIRST;
	else if (count_c == count_q - 1) pos = LAST;
	else pos = MIDDLE;
	char* commd = (char*)malloc(MAX_LENGTH * sizeof(char));
	if (count_q != 1 && (!strcmp(arg[count_c].input, "") || !strcmp(arg[count_c].output, ""))) {
		switch (pos) {
		case FIRST:
			wsprintf(commd, TEXT("%s.exe %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, PIPE);
			break;

		case MIDDLE:
			wsprintf(commd, TEXT("%s.exe %s %s"), commd_table[commd_queue[count_c]], PIPE, PIPE);
			break;

		case LAST:
			wsprintf(commd, TEXT("%s.exe %s %s"), commd_table[commd_queue[count_c]], PIPE, "STD_OUTPUT");
			break;
		}
	}
	else if (!strcmp(arg[count_c].output, "")){
		wsprintf(commd, TEXT("%s.exe %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, "STD_OUTPUT");
	}
	else {
		wsprintf(commd, TEXT("%s.exe %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, arg[count_c].output);
	}

	if (!CreateProcess(NULL,   // No module name (use command line)
		commd,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		WriteConsole(handle_out, "CreateProcess failed\n", strlen("CreateProcess failed\n"), NULL, NULL);
		return;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

// command parser
int parser(char *argv[MAX_ARG_NUM], int* argc, int commd_queue[MAX_ARG_NUM], arg arg[MAX_ARG_NUM]) {
	size_t i = 0;
	if (search_commd(argv[0]) == -1) { // first argument is not a command
		while (argv[0][i] != '\0' && i < strlen(argv[0])) {
			arg[0].input[i] = argv[0][i];
			i++;
		}
	}
	count_a = 0;
	count_q = 0;
	while (argv[count_a] != "" && count_a < (*argc) + 1 && count_q < (*argc) + 1) { // generate a command queue
		int index = search_commd(argv[count_a]);
		if (index == -1) {
			count_a++;
			continue;
		}
		commd_queue[count_q] = index;
		count_a++;
		count_q++;
		if (count_a == (*argc) + 1 || !strcmp(argv[count_a], "|")) {
			continue;
		}
		else if (count_a + 1 <= (*argc) && !strcmp(argv[count_a], "<")) {
			i = 0;
			while (argv[count_a + 1][i] != '\0' && i < strlen(argv[count_a + 1])) {
				arg[count_q - 1].input[i] = argv[count_a + 1][i];
				i++;
			}
		}
		if (count_a + 1 <= (*argc) && !strcmp(argv[count_a], ">")) {
			i = 0;
			while (argv[count_a + 1][i] != '\0' && i < strlen(argv[count_a + 1])) {
				arg[count_q - 1].output[i] = argv[count_a + 1][i];
				i++;
			}
		}
		if (count_a - 3 >= 0 && !strcmp(argv[count_a - 2], ">")) {
			i = 0;
			while (argv[count_a - 3][i] != '\0' && i < strlen(argv[count_a - 3])) {
				arg[count_q - 1].input[i] = argv[count_a - 3][i];
				i++;
			}
		}
		if (count_a - 3 >= 0 && !strcmp(argv[count_a - 2], "<")) {
			i = 0;
			while (argv[count_a - 3][i] != '\0' && i < strlen(argv[count_a - 3])) {
				arg[count_q - 1].output[i] = argv[count_a - 3][i];
				i++;
			}
		}
		if (count_a <= (*argc) && !strcmp(arg[count_q - 1].input, "") && strcmp(argv[count_a], "|")) {
			i = 0;
			while (argv[count_a][i] != '\0' && i < strlen(argv[count_a])) {
				arg[count_q - 1].input[i] = argv[count_a][i];
				i++;
			}
		}
		if (count_a + 2 <= (*argc) && !strcmp(arg[count_q - 1].output, "") && !strcmp(argv[count_a + 1], ">")) {
			i = 0;
			while (argv[count_a + 2][i] != '\0' && i < strlen(argv[count_a + 2])) {
				arg[count_q - 1].output[i] = argv[count_a + 2][i];
				i++;
			}
		}
	}

	return count_q; // number of commands
}

// process the queue
void process_queue(int commd_queue[MAX_ARG_NUM], arg arg[MAX_ARG_NUM]) {
	int count_c = 0;
	while (count_c < count_q) {
		set_process(count_c, arg);
		count_c++;
	}
}
