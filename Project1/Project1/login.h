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
BOOL passwd();
//command "logout"
BOOL logout();

DWORD dLogin_initial(struct passwd *pw);
BOOL bpasswd_initial();
BOOL brfile_init(char *path);
BOOL bwfile_init(char *path);

BOOL bget_usr(char *usrbuff);
BOOL bcheck_psw(char *pswbuff, struct passwd *pw);
char *cHashMD5(char *data, DWORD *result);

BOOL bgetnamebyid(char *name, char *id);
BOOL bgetpswbyname(char *psw, char *name);
BOOL bwritepsw(char *name, char *psw);
BOOL bcmpstring(char *str1, char *str2);
BOOL bcpstring(char *source_str, char *taget_str, int lenth);
DWORD dget_inputlenth(char *inputbuff);
#endif