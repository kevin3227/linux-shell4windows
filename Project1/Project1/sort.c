#include"commd.h"

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

int cmpfunc(const void * a, const void * b) {
	line* line1 = a;
	line* line2 = b;
	return (*line1).fst_char - (*line2).fst_char;
}

void sort(char* input, char* output, char* overwrite, char* opt) {
	HANDLE fp, handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	
	cls(handle_out);
	if (!strcmp(input, "STD_INPUT")) {
		fp = CreateFile(opt, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}
	else if(!strcmp(input, PIPE)) {
		fp = CreateFile(PIPE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}
	else {
		fp = CreateFile(input, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}
	
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
	char *buffer = (char*)malloc(sizeof(char));

	line lineset[MAX_LENGTH];
	int seq = 0, i = 0, flag = 1;
	LARGE_INTEGER liCurrentPosition;
	// read in line by line

	ReadFile(fp, tmp, 1, &dwBytesRead, &stOverlapped);
	lineset[seq].fst_char = tmp[0];
	lineset[seq].offset = FILE_BEGIN;
	liCurrentPosition.QuadPart = 0;
	
		// if(fgets(tmp, MAX_LENGTH, fp));
	while(ReadFile(fp, tmp, 1, &dwBytesRead, NULL)) {
		// SetFilePointerEx(fp, liCurrentPosition, NULL, FILE_CURRENT);
		if (stOverlapped.Offset++ == dwFileSize) break;
		if (tmp[0] == '\n') {
			// i = 0;
			seq++;
			stOverlapped.Offset++;
			ReadFile(fp, tmp, 1, &dwBytesRead, NULL);
			// SetFilePointerEx(fp, liCurrentPosition, NULL, FILE_CURRENT);
			lineset[seq].fst_char = tmp[0];
			lineset[seq].offset = stOverlapped.Offset;
		}
	}
	// sort by first char
	qsort(lineset, seq + 1, sizeof(line), cmpfunc);
	stOverlapped.Offset = 0;
	// print sorted text
	if (!strcmp(output, PIPE)) {
		handle_out = CreateFile(PIPE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	} 
	else if (strcmp(output, "STD_OUTPUT")) {
		if(!strcmp(overwrite, "FALSE")) {
			if (handle_out = CreateFile(output, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0) == INVALID_HANDLE_VALUE) {
				handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
				WriteConsole(handle_out, "Failed to open file\n",
				strlen("Failed to open file\n"),
				&dw,
				NULL);
				return;
			}
			handle_out = CreateFile(output, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			liCurrentPosition.QuadPart = 0;
			SetFilePointerEx(handle_out, liCurrentPosition, NULL, FILE_END);
		} 
		else {
			handle_out = CreateFile(output, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		}
	}
	
	i = 0;
	while(i < seq && flag) {
		liCurrentPosition.QuadPart = lineset[i].offset;
		SetFilePointerEx(fp, liCurrentPosition, NULL, FILE_BEGIN);
		stOverlapped.Offset = liCurrentPosition.LowPart;
		do {
			ReadFile(fp, tmp, 1, &dwBytesRead, NULL);
			stOverlapped.Offset++;
			wsprintf(buffer, TEXT("%c"), tmp[0]);
			WriteFile(handle_out, buffer, 1, NULL, NULL);
		} while (tmp[0] != '\n' && stOverlapped.Offset != dwFileSize);
		i++;
	}
	CloseHandle(fp);
	CloseHandle(handle_out);
}

int main(int argc, TCHAR *argv[]) {
	sort(INPUT_TYPE, OUTPUT_TYPE, OVERWRITE, OPT_ARG);
	return 0;
}
