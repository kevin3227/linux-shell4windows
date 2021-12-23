/* implementation of the commands */

#include<windows.h>
#include<stdio.h>
#include<string.h>
#include<conio.h>
#include<math.h>
#define MAX_PAGENUM 128

HANDLE	handle_in;
HANDLE	handle_out;
DWORD dw;

// typedef void(*commd)(char* argv[8], int* argc);

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
		while ((*command_index) == ' ') {
			command_index++;
		}
		// remove space enter alt
		while ((*command_index) != ' ' && (*command_index) != '\n' && (*command_index) != '\r' && (*command_index) != '\0') {
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
	FILE* fp;
	errno_t err = fopen_s(&fp, argv[1], "r");
	if (err) {
		WriteConsole(handle_out, "Failed to open file\n", strlen("Failed to open file\n"), &dw, NULL);
		fclose(fp);
	}
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle_out, &csbi);

	int conf, flag,
		count = 0, count_c = 1,
		size = 1, size_r, size_c, pctg,
		page_num = 0, offset = 0, index[MAX_PAGENUM];
	char tmp, fst_char;

	index[page_num] = 0;
	fst_char = fgetc(fp);
	while (tmp = fgetc(fp) != EOF) {
		size++;
	}
	fseek(fp, 0, SEEK_SET);

	while (fp) {
		// get window size
		size_r = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		size_c = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		count++;
		offset++;

		tmp = fgetc(fp);
		// EOF keyboard response
		if (tmp == EOF) {
 			flag = 1;
			while (flag) {
				conf = -1;
				if (_kbhit()) {
					conf = _getch();
				}
				switch (conf) {
				case ('b'):
					if (page_num == 1) break;
					else if (page_num == 1) {
						cls(handle_out);
						count = 0;
						count_c = 1;
						flag = 0;
						fseek(fp, 0, SEEK_SET);
						offset = index[page_num - 1];
						page_num--;
						break;
					}
					else {
						cls(handle_out);
						count = 0;
						count_c = 1;
						flag = 0;
						fseek(fp, index[page_num - 1], SEEK_SET);
						offset = index[page_num - 1];
						page_num--;
						break;
					}

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
		if (tmp == '\n' || count >= size_r) {
			count_c++;
			count = 0;
		}
		// another page
		else if (count_c >= size_c) {
			page_num++;
			index[page_num] = offset;
			pctg = ((double)offset / (double)size) * 100.0;
			printf(" --MORE %d%%--", pctg);
			// WriteConsole(handle_out, " --MORE-- ", strlen(" --MORE-- "), &dw, NULL);
			flag = 1;
			while (flag) {
				conf = -1;
				if (_kbhit()) {
					conf = _getch();
				}
				// keyboard response
				switch (conf) {
				case (' '):
				case ('\r'):
				cls(handle_out);
				count = 0;
				count_c = 1;
				flag = 0;
				break;

				case ('b'):
				if (page_num == 1) break;
				else if (page_num == 2) {
					cls(handle_out);
					count = 0;
					count_c = 1;
					flag = 0;
					fseek(fp, 0, SEEK_SET);
					offset = index[page_num - 2];
					page_num -= 2;
					break;
				}
				else {
					cls(handle_out);
					count = 0;
					count_c = 1;
					flag = 0;
					fseek(fp, index[page_num - 2], SEEK_SET);
					offset = index[page_num - 2];
					page_num -= 2;
					break;
				}

				case ('q'):
				cls(handle_out);
				return;
				default:
				break;
				}
			}
		}
		if (!offset) {
			putchar(fst_char);
		}
		else {
			putchar(tmp);
		}
		// WriteConsole(handle_out, tmp, strlen(tmp), &dw, NULL);
	}
	// cls(handle_out);
	fclose(fp);
	WriteConsole(handle_out, "\n", strlen("\n"), &dw, NULL);
}
/* TODO */
