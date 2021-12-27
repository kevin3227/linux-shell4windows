#include <windows.h>
#include "commd.h"

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

    DWORD pid;
};

//command "login"
BOOL login();
//command "passwd"
void password();
//command "logout"
void logout();

DWORD LoginInitial(struct login_context *cxt);
BOOL dget_inputlenth(char *inputbuff);
BOOL bget_usr(char *usrbuff, struct passwd *pw);
BOOL icheck_psw(char *pswbuff, struct passwd *pw);

