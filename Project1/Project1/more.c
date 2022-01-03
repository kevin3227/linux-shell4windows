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

void more(char* filename) {
	HANDLE fp;
	cls(handle_out);
	
	fp = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

	int flag, sign = 1,
		count = 0, count_c = 1,
		size_r, size_c, pctg,
		page_num = 0;
	DWORD index[MAX_PAGE_NUM];
	char *fst_char = (char*)malloc(sizeof(char)),
		*buffer = (char*)malloc(MAX_LENGTH * sizeof(char));
	LARGE_INTEGER liCurrentPosition;

	index[page_num] = FILE_BEGIN;
	ReadFile(fp, fst_char, 1, &dwBytesRead, &stOverlapped);
	liCurrentPosition.QuadPart = 0;
	SetFilePointerEx(fp, liCurrentPosition, NULL, FILE_BEGIN);

	DWORD cNumRead, fdwMode, fdwSaveOldMode;
	INPUT_RECORD irInBuf;
	// Save the current input mode, to be restored on exit.
	GetConsoleMode(handle_in, &fdwSaveOldMode);
	// Enable the window and mouse input events.
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(handle_in, fdwMode);

	/* TODO */

	while (fp && sign) {
		sign = 0;
		flag = 0;
		FlushConsoleInputBuffer(handle_in);
		/*irInBuf.Event.KeyEvent.uChar.AsciiChar = '\0';*/
		// get window size
		GetConsoleScreenBufferInfo(handle_out, &csbi);
		size_r = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		size_c = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		count++;

		ReadFile(fp, tmp, 1, NULL, NULL);
		// EOF keyboard response
		if (stOverlapped.Offset++ < dwFileSize - 1) sign = 1;
		else {
			WriteConsole(handle_out, tmp, 1, &dw, NULL);
			flag = 1;
		}
		while (flag == 1 && ReadConsoleInput(handle_in, &irInBuf, 1, &cNumRead)) {
			if (irInBuf.EventType != KEY_EVENT) continue;
			switch (irInBuf.Event.KeyEvent.uChar.AsciiChar) {
			case 'b':
				if (page_num == 1) break;
				cls(handle_out);
				count = 0;
				count_c = 1;
				flag = -1;
				sign = 1;
				liCurrentPosition.LowPart = index[page_num - 1];
				SetFilePointer(fp, liCurrentPosition.LowPart, NULL, FILE_BEGIN);
				page_num--;
				stOverlapped.Offset = index[page_num];
				break;

			case 'q':
				cls(handle_out);
				SetConsoleMode(handle_in, fdwSaveOldMode);
				CloseHandle(fp);
				return;

			default:
				break;
			}
		}
		if(flag == -1) continue;
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
		while (flag == 2 && ReadConsoleInput(handle_in, &irInBuf, 1, &cNumRead)) {
			if (irInBuf.EventType != KEY_EVENT) continue;
			switch (irInBuf.Event.KeyEvent.uChar.AsciiChar) {
			case ' ':
				cls(handle_out);
				count = 0;
				count_c = 1;
				flag = -2;
				break;

			case 'b':
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

			case 'q':
				cls(handle_out);
				SetConsoleMode(handle_in, fdwSaveOldMode);
				CloseHandle(fp);
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
	free(tmp);
	free(fst_char);
	free(buffer);
}

int main (int argc, TCHAR *argv[]) {
	handle_in = GetStdHandle(STD_INPUT_HANDLE);
	handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	more(argv[1]);
	CloseHandle(handle_in);
	CloseHandle(handle_out);
	return 0;
}
