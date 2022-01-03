#ifndef _SHELL_H_
#define _SHELL_H_

#include<Windows.h>
#include<fileapi.h>
#include<tchar.h>

#define MAX_PAGE_NUM 1024
#define MAX_LENGTH 1024
#define MAX_ARG_NUM 8
#define COMMD_NUM 5
#define POS_NUM 3

// command position
#define FIRST 0
#define MIDDLE 1
#define LAST 2

#define PIPE "pipefile"

static char* commd_table[COMMD_NUM] = {
	"password", // 0
	"logout",	// 1
	"sort",		// 2
	"more",		// 3
	"print"		// 4
};

static char* pos_table[POS_NUM] = {
	"first", // 0
	"middle",	// 1
	"last"		// 2
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
	// char* opt_arg[MAX_ARG_NUM];
	// char overwrite;
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