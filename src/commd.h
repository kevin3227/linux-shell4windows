#ifndef _COMMD_H_
#define _COMMD_H_

#include <Windows.h>
#include <stdio.h>
#define MAX_PAGENUM 1024
#define MAX_LENGTH 1024

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

// typedef void(*commd)(char* argv[8], int* argc);
typedef struct
{
	int offset;
	int fst_char;
} line;

int console();
void getArgv(char *command, char *argv[8], int *argc);
void cls(HANDLE hConsole);
void more(char *argv[8], int *argc);
void sort(char *argv[8], int *argc);

#endif