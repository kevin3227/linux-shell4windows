#ifndef _SHELL_H_
#define _SHELL_H_

#include<Windows.h>
#include<stdlib.h>
#include<fileapi.h>
#include<tchar.h>
//#include<stdio.h>
//#include<stdlib.h>

#define MAX_PAGE_NUM 1024
#define MAX_LENGTH 1024
#define MAX_ARG_NUM 8
#define COMMD_NUM 5
#define POS_NUM 3

//// command position
//#define FIRST 0
//#define MIDDLE 1
//#define LAST 2

#define PIPE "pipefile"
#define INPUT_TYPE argv[1]
#define OUTPUT_TYPE argv[2]
#define OVERWRITE argv[3]
#define OPT_ARG argv[4]

static char* commd_table[COMMD_NUM] = {
	"passwd", // 0
	"logout",	// 1
	"sort",		// 2
	"more",		// 3
	"print"		// 4
};

HANDLE handle_in;
HANDLE handle_out;
DWORD dw;
SECURITY_ATTRIBUTES sa;
STARTUPINFO si;
PROCESS_INFORMATION pi;

// typedef void(*commd)(char* argv[MAX_ARG_NUM], int* argc);
typedef struct {
	int offset;
	int fst_char;
}line;

typedef struct {
	char input[MAX_LENGTH];
	char output[MAX_LENGTH];
	char opt_arg[MAX_LENGTH];
	char overwrite[MAX_LENGTH];
}arg;

int commd_queue[MAX_ARG_NUM]; // command queue

int console();
void getArgv(char *command, char *argv[MAX_ARG_NUM], int *argc);
int parser(char *argv[MAX_ARG_NUM], int* argc, int commd_queue[MAX_ARG_NUM], arg arg[MAX_ARG_NUM]);
void process_queue(int commd_queue[MAX_ARG_NUM], arg arg[MAX_ARG_NUM]);
void cls(HANDLE hConsole);
// void more(char* filename);
// void sort(char* input, char* output);

#endif