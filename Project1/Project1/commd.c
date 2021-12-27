/* implementation of the commands */

#include"commd.h"

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

void getArgv(char *command, char *argv[8], int *argc) {
	// index for scaning command
	char *command_index = command;

	// scan command string and split to argv
	for ((*argc) = 0; 
	(*command_index) != '\n' 
	&& (*command_index) != '\r' 
	&& (*argc) < 8; (*argc)++) {
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
//clear the screenbuffer
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

//command "more"
void more(char* argv[8], int* argc) {
	cls(handle_out);
	// no arguments 
	if ((*argc) == 0) {
		WriteConsole(
			handle_out,
			"Please enter arguments. For further info, try 'man more'\n",
			strlen("Please enter arguments. For further info, try 'man more'\n"),
			&dw,
			NULL);
		return;
	}
	HANDLE fp = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fp == INVALID_HANDLE_VALUE) {
		WriteConsole(handle_out, "Failed to open file\n",
			strlen("Failed to open file\n"),
			&dw,
			NULL);
		return;
	}
	DWORD dwBytesRead = 0;         //actual lenth of readbytes
	BOOL bContinue = TRUE;         //read file control sign
	OVERLAPPED stOverlapped = { 0 }; //read file info(offset)
	DWORD dwFileSize = GetFileSize(fp, NULL);   //get file size
	char *tmp = (char *)malloc(sizeof(char)); //read buffer
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	int conf, flag, sign = 1,
		count = 0, count_c = 1,
		size_r, size_c, pctg,
		page_num = 0;
	DWORD index[MAX_PAGENUM];
	char *fst_char = (char*)malloc(sizeof(char)),
		*buffer = (char*)malloc(MAX_LENGTH * sizeof(char));
	LARGE_INTEGER liCurrentPosition;

	index[page_num] = FILE_BEGIN;
	ReadFile(fp, fst_char, 1, &dwBytesRead, &stOverlapped);
	liCurrentPosition.QuadPart = 0;
	SetFilePointerEx(fp, liCurrentPosition, NULL, FILE_BEGIN);

	/* TODO */

	while (fp && sign) {
		sign = 0;
		flag = 0;
		// get window size
		GetConsoleScreenBufferInfo(handle_out, &csbi);
		size_r = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		size_c = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		count++;

		ReadFile(fp, tmp, 1, NULL, NULL);
		// EOF keyboard response
		stOverlapped.Offset++;
		if (stOverlapped.Offset < dwFileSize) sign = 1;
		else {
			WriteConsole(handle_out, tmp, 1, &dw, NULL);
			flag = 1;
			while (flag == 1) {
				conf = -1;
				if (_kbhit()) conf = _getch();
				switch (conf) {
				case ('b'):
					if (page_num == 1) break;
					cls(handle_out);
					count = 0;
					count_c = 1;
					flag = -2;
					liCurrentPosition.LowPart = index[page_num - 1];
					SetFilePointer(fp, liCurrentPosition.LowPart, NULL, FILE_BEGIN);
					page_num--;
					stOverlapped.Offset = index[page_num];
					sign = 1;
					break;

				case ('q'):
					cls(handle_out);
					return;

				default:
					break;
				}
			}
			continue;
		}
		// another line
		if (tmp[0] == '\n' || count > size_r) {
			count_c++;
			count = 0;
		}
		// another page
		if (count_c >= size_c) {
			page_num++;
			/*liCurrentPosition.QuadPart = 0;
			SetFilePointerEx(fp, liCurrentPosition, &liCurrentPosition, FILE_CURRENT);*/
			index[page_num] = stOverlapped.Offset;
			pctg = ((double)index[page_num] / (double)dwFileSize) * 100.0;
			WriteConsole(handle_out, tmp, 1, &dw, NULL);
			wsprintf(buffer, TEXT(" --MORE %d%%-- "), pctg);
			WriteConsole(handle_out, buffer, strlen(" --MORE %d%%-- "), NULL, NULL);
			flag = 2;
		}
		while (flag == 2) {
			conf = -1;
			if (_kbhit()) {
				conf = _getch();
			}
			// keyboard response
			switch (conf) {
			case (' '):
				cls(handle_out);
				count = 0;
				count_c = 1;
				flag = -2;
				break;

			case ('\r'):
				/* TODO */

			case ('b'):
				if (page_num == 1) break;
				cls(handle_out);
				count = 0;
				count_c = 1;
				flag = -2;
				liCurrentPosition.LowPart = index[page_num - 2];
				SetFilePointer(fp, liCurrentPosition.LowPart, NULL, FILE_BEGIN);
				page_num -= 2;
				stOverlapped.Offset = index[page_num];
				break;

			case ('q'):
				cls(handle_out);
				return;

			default:
				break;
			}
		}
		if ((flag == -2)) {
			continue;
		}

		WriteConsole(handle_out, tmp, 1, &dw, NULL);
	}
		CloseHandle(fp);
		WriteConsole(handle_out, "\n", strlen("\n"), &dw, NULL);
		return;
}

// command "sort"
//int cmpfunc(const void * a, const void * b) {
//	line* line1 = a;
//	line* line2 = b;
//	return (*line1).fst_char - (*line2).fst_char;
//}
//
//void sort(char *argv[8], int *argc) {
//	cls(handle_out);
//	// no arguments 
//	if ((*argc) == 0) {
//		WriteConsole(
//			handle_out,
//			"Please enter arguments. For further info, try 'man sort'\n",
//			strlen("Please enter arguments. For further info, try 'man sort'\n"),
//			&dw,
//			NULL);
//		return;
//	}
//	FILE* fp;
//	errno_t err = fopen_s(&fp, argv[1], "r");
//	if (err) {
//		WriteConsole(handle_out, 
//			"Failed to open file\n", 
//			strlen("Failed to open file\n"), 
//			&dw, 
//			NULL);
//			return;
//	}
//
//	line lineset[MAX_LENGTH];
//	char tmp[MAX_LENGTH];
//	int seq = 0, i;
//
//	// read in line by line
//	while (fp && seq < MAX_LENGTH) {
//		i = 0;
//		lineset[seq].offset = ftell(fp);
//		if(fgets(tmp, MAX_LENGTH, fp));
//		lineset[seq].fst_char = tmp[0];
//		while (tmp[i] != '\0') {
//			i++;
//		}
//		if (tmp[i - 1] != '\n') break;
//		seq++;
//	}
//	// sort by first char
//	qsort(lineset, seq + 1, sizeof(line), cmpfunc);
//	// print sorted text
//	for (i = 0; i <= seq; ++i) {
//		fseek(fp, lineset[i].offset, SEEK_SET);
//		fgets(tmp, MAX_LENGTH, fp);
//		printf("%s", tmp);
//	}
//	fclose(fp);
//}

// command ""

/* TODO */