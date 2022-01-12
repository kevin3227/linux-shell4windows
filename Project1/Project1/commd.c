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
			// iudge command character and save to the split argv
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

//
void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);
}


// string copy
static inline void strmcpy(char* dst, const char* src) {
	wsprintf(dst, TEXT("%s"), src);
}

// search the command table
static inline int search_commd(char* commd) {
	for (int i = 0; i <= COMMD_NUM - 1; i++) {
		if (!strcmp(commd, commd_table[i])) return i;
	}
	return -1;
}

// set progress
void set_process(int count_c ,arg arg[MAX_ARG_NUM]) {
	//int pos;
 //	if (!count_c && count_c != count_q - 1) pos = FIRST;
	//else if (count_c == count_q - 1) pos = LAST;
	//else pos = MIDDLE;
	char* commd = (char*)malloc(MAX_LENGTH * sizeof(char));
	if (!strcmp(arg[count_c].input, "") && !strcmp(arg[count_c].output, "")) {
		return;
	}
	else if (!strcmp(arg[count_c].input, "") && strcmp(arg[count_c].output, "") && !strcmp(arg[count_c].overwrite, "")) {
		wsprintf(commd, TEXT("%s.exe %s %s %s"), commd_table[commd_queue[count_c]], "STD_INPUT", arg[count_c].output, "TRUE");
	}
	else if (!strcmp(arg[count_c].output, "") && strcmp(arg[count_c].input, "") && !strcmp(arg[count_c].overwrite, "")) {
		wsprintf(commd, TEXT("%s.exe %s %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, "STD_OUTPUT", "TRUE");
	}
	else if (!strcmp(arg[count_c].input, "") && strcmp(arg[count_c].output, "") && strcmp(arg[count_c].overwrite, "")) {
		wsprintf(commd, TEXT("%s.exe %s %s %s"), commd_table[commd_queue[count_c]], "STD_INPUT", arg[count_c].output, arg[count_c].overwrite);
	}
	else if (!strcmp(arg[count_c].output, "") && strcmp(arg[count_c].input, "") && strcmp(arg[count_c].overwrite, "")){
		wsprintf(commd, TEXT("%s.exe %s %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, "STD_OUTPUT", arg[count_c].overwrite);
	}
	else if (!strcmp(arg[count_c].overwrite, "")) {
		wsprintf(commd, TEXT("%s.exe %s %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, arg[count_c].output, "TRUE");
	}
	else {
		wsprintf(commd, TEXT("%s.exe %s %s %s"), commd_table[commd_queue[count_c]], arg[count_c].input, arg[count_c].output, arg[count_c].overwrite);
	}
	// optional arguments
	wsprintf(commd, TEXT("%s %s"), commd, arg[count_c].opt_arg);

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
	// size_t i = 0;
	int i, fi, fo, fow ,fop;
	count_a = 0;
	count_q = 0;
	while (argv[count_a] != "" && count_a < (*argc) + 1 && count_q < (*argc) + 1) { // generate a command queue
		i = 0; 
		fi = 0, fo = 0, fow = 0, fop = 0;
		int index = search_commd(argv[count_a]);
		if (index == -1) {
			count_a++;
			continue;
		}
		commd_queue[count_q] = index;
		count_a++;
		count_q++;
		if (count_a < (*argc) + 1 && !strcmp(argv[count_a - 1], "|")) {
			continue;
		}
		else if (count_a + 1 <= (*argc) && !strcmp(argv[count_a], "<")) {
			strmcpy(arg[count_q - 1].input, argv[count_a + 1]);
			fi = 1;
		}
		else if (count_a - 3 >= 0 && !strcmp(argv[count_a - 2], ">")) {
			strmcpy(arg[count_q - 1].input, argv[count_a - 3]);
			fi = 1;
		}

		i = count_a - 1;
		while (!fi && i-- > 0) {
			if (i - 1 < 0 || strcmp(argv[i], "|")) continue;
			if (count_q - 1 == 0) {
				strmcpy(arg[0].input, argv[0]);
				fi = 1;
				break;
			}
			strmcpy(arg[count_q - 1].input, PIPE);
			fi = 1;
		}

		i = count_a - 1;
		while (!fi && i++ < (*argc)) {
			if (!strcmp(argv[i], "|") || !strcmp(argv[i], ">") || !strcmp(argv[i], ">>") || !strcmp(argv[i], "<")) break;
			if (i == count_a) {
				strmcpy(arg[count_q - 1].opt_arg, argv[i]);
				wsprintf(arg[count_q - 1].opt_arg, TEXT("%s%s"), arg[count_q - 1].opt_arg, " ");
				fop = 1;
			}
			else {
				wsprintf(arg[count_q - 1].opt_arg, TEXT("%s %s"), arg[count_q - 1].opt_arg, argv[i]);
				fop = 1;
			}
		}

		if (!fop) strmcpy(arg[count_q - 1].opt_arg, "");

		if(fi == 0) strmcpy(arg[count_q - 1].input, "STD_INPUT");

		i = count_a - 1;
		while (i-- > 0) {
			if (i - 1 < 0 || strcmp(argv[i], "<")) continue;
			strmcpy(arg[count_q - 1].output, argv[i - 1]);
			fo = 1;
		}
		i = count_a - 1;

		while (!fo && i++ < (*argc)) {
			if (i + 1 > (*argc) || strcmp(argv[i], ">")) continue;
			strmcpy(arg[count_q - 1].output, argv[i + 1]);
			fo = 1;
		}

		i = count_a - 1;
		while (!fo && i++ < (*argc)) {
			if (i + 1 > (*argc) || strcmp(argv[i], ">>")) continue;
			strmcpy(arg[count_q - 1].overwrite, "FALSE");
			fow = 1;
			strmcpy(arg[count_q - 1].output, argv[i + 1]);
			fo = 1;
		}
		i = count_a - 1;
		while (!fo && i++ < (*argc)) {
			if (i + 1 > (*argc) || strcmp(argv[i], "|")) continue;
			strmcpy(arg[count_q - 1].output, PIPE);
			fo = 1;
		}

		if (!fo) strmcpy(arg[count_q - 1].output, "STD_OUTPUT");
		if (!fow) strmcpy(arg[count_q - 1].overwrite, "TRUE");
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
