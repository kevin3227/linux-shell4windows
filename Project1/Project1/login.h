#ifndef _LOGIN_H_
#define _LOGIN_H_

#include <windows.h>
#include "commd.h"

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

struct passwd
{
	char *pw_name; /* Username. */
	DWORD lenth_name;
	char *pw_passwd; /* Password. */
	DWORD lenth_passwd;
};

struct login_context
{
	struct passwd *pwd; /* user info */
	char *uid;
	DWORD pid;
};

//command "login"
BOOL login(struct login_context *cxt);
//command "passwd"
void password();
//command "logout"
void logout();

DWORD Login_initial(struct login_context *cxt);
DWORD dget_inputlenth(char *inputbuff);
BOOL bget_usr(char *usrbuff, struct login_context *cxt);
BOOL icheck_psw(char *pswbuff, struct passwd *pw);
char *HashMD5(char *data, DWORD *result);
BOOL bcpstring(char *source_str, char *taget_str, int lenth);
BOOL bfile_init();

#endif