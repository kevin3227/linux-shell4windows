#include <windows.h>
#include <conio.h>
#include "login.h"
#include <assert.h>

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;
HANDLE hEvent;
HANDLE hFile;

DWORD oldIMode;
DWORD iMode;

DWORD oldOMode;
DWORD oMode;
char *usrbuff;
char *pswbuff;

//command "login"
BOOL login()
{
    //get handle in
    handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle_out == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

    //get handle out
    handle_in = GetStdHandle(STD_INPUT_HANDLE);
    if (handle_in == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

	//initial  name and password of the current user
	struct passwd pw = {
		.pw_name = NULL,
		.pw_passwd = NULL
	};

	//get struct to save login infomation
	struct login_context cxt = {
		.pid = NULL, /* PID */
		.pwd = &pw 
	};

	LoginInitial(&cxt);
	//if (!LoginInitial())
	//{
	//	return GetLastError();
	//}

    WriteConsole(handle_out, "login: ", strlen("login: "), &dw, NULL);
    //get username
    ReadConsole(handle_in, usrbuff, 32, &dw, NULL);
	//WriteConsole(handle_out, usrbuff, dget_inputlenth(usrbuff), &dw, NULL);

    //check input username whether in saved user_file
    if (!bget_usr(usrbuff, &pw))
    {

        int logincount = 0; // to count input times
        while (logincount < 5)
        {
            WriteConsole(handle_out, "password: ", strlen("password: "), &dw, NULL);

            // set console input disable echo mode
            iMode = oldIMode;
            iMode &= ~ENABLE_ECHO_INPUT;
            if (!SetConsoleMode(handle_in, iMode))
            {
                return GetLastError();
            }

            //get passowrd
            ReadConsole(handle_in, pswbuff, 32, &dw, NULL);

            //reset input_mode
            if (!SetConsoleMode(handle_in, oldIMode))
            {
                return GetLastError();
            }

            //check passowrd
            if (icheck_psw(pswbuff, &pw))
            {
                free(usrbuff);
                free(pswbuff);
                return 0;
            }
            else
            {
                logincount++;
                WriteConsole(handle_out, "\nWRONG PASSWORD! PLEASE RETRY\n", strlen("\nWRONG PASSWORD! PLEASE RETRY\n"), &dw, NULL);
            }
        }
    }
    WriteConsole(handle_out, "\nWRONG USER NAME!\n exit \n", strlen("\nWRONG USER NAME!\n exit \n"), &dw, NULL);
    free(pswbuff);
    free(usrbuff);
    return 1;
}

//command "password"
void password()
{
	/* TODO */
}

//command "logout"
void logout()
{
	/* TODO */
}


/*
	get string lenth from input buffer.
	example: string/r/n***  to 6 .
*/
DWORD dget_inputlenth(char *inputbuff) {

	char *buffer = inputbuff;
	DWORD lenth = 0;
	while (*buffer != '\r' && *buffer != ' ' && lenth < 32 && *buffer != '\0' && inputbuff)
	{
		lenth++;
		buffer++;
	}
	if (lenth == 32)
	{
		return 0;
	}
	return lenth;
}

DWORD LoginInitial(struct login_context *cxt)
{
    //get old console input mode
    oldIMode = 0;
    if (!GetConsoleMode(handle_in, &oldIMode))
    {
        return GetLastError();
    }
	usrbuff = (char *)malloc(sizeof(char) * 32);
	if (!usrbuff) {
		return GetLastError();
	}

	pswbuff = (char *)malloc(sizeof(char) * 32);
	if (!pswbuff) {
		return GetLastError();
	}
	cxt->pwd->pw_name = (char *)malloc(sizeof(char) * 32);
	if (!cxt->pwd->pw_name) {
		return GetLastError();
	}
	cxt->pwd->pw_passwd = (char *)malloc(sizeof(char) * 32);
	if (!cxt->pwd->pw_passwd) {
		return GetLastError();
	}
	return 0;
}

/*
    check password is right.
    usrbuff: password to be check
    pw: saved user infomation

    there's a bug to be fixed !!!!!
*/

BOOL icheck_psw(char *pswbuff, struct passwd *pw)
{

	DWORD lenth = 0;
	int count = 0;
	lenth = dget_inputlenth(pswbuff);
	if (lenth == pw->lenth_passwd)
	{
		while (pswbuff[count] == pw->pw_passwd[count] && count != lenth)
		{
			count++;
		}
		if (pw->lenth_passwd == count) {
			WriteConsole(handle_out, "\nCHECK PASSWORD SUCCEED!\n", strlen("\nCHECK PASSWORD SUCCEED!\n"), &dw, NULL);
			return 1;
		}
	}
	return 0;
}
/*
    get user_name and password by username
    usrbuff: username to be found.
    pw: struct to be returned, including user infomation 
*/
BOOL bget_usr(char *usrbuff, struct passwd *pw)
{
    //creat handle event
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (hEvent == NULL)
    {
        return 1;
    }

    //creat handle file
    hFile = CreateFile("usr.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
		WriteConsole(handle_out, "\n OPPEN FILE FAILED ! \n\tEXIT\n", strlen("\n OPPEN FILE FAILED ! \n\tEXIT\n"), &dw, NULL);
        return GetLastError();
    }

    DWORD dwBytesRead = 0;         //actual lenth of readbytes
    BOOL bContinue = TRUE;         //read file control sign
    OVERLAPPED stOverlapped = {0}; //read file info(offset)
    stOverlapped.hEvent = hEvent;  //read event
    char *rbuff = (char *)malloc(1);
    DWORD dwFileSize = GetFileSize(hFile, NULL);   //get file size
    char *tmp = (char *)malloc(sizeof(char) * 16); //read buffer
    int count = 0;                                 // read buffer offset

    while (bContinue)
    {
        bContinue = FALSE;
        ReadFile(hFile, rbuff, 1, &dwBytesRead, &stOverlapped);

        // WriteConsole(handle_out, rbuff, 1, &dw, NULL);

        tmp[count] = *rbuff;
        count++;

        if (*rbuff == ' ')
        {
            tmp[--count] = '\0';
            pw->lenth_name = count;
            while (count&&count < 32)
            {
                count--;
                pw->pw_name[count] = tmp[count];
            }
            pw->pw_name[pw->lenth_name] = '\0';
            // WriteConsole(handle_out, pw->pw_name, strlen(pw->pw_name), &dw, NULL);
        }
        else if (*rbuff == '\r')
        {
            tmp[--count] = '\0';
            pw->lenth_passwd = count;
            while (count&&count<32)
            {
                count--;
                pw->pw_passwd[count] = tmp[count];
            }
            pw->pw_passwd[pw->lenth_passwd] = '\0';
            // WriteConsole(handle_out, pw->pw_passwd, strlen(pw->pw_passwd), &dw, NULL);
        }
        else if (*rbuff == '\n')
        {
            //check usrbuff ?= pw->pw_name
			DWORD input_lenth = 0;
            int tmpcount = 0;
			input_lenth = dget_inputlenth(usrbuff);
			if (input_lenth == pw->lenth_name)
			{
				while (usrbuff[tmpcount] == pw->pw_name[tmpcount] && tmpcount != input_lenth)
				{
					tmpcount++;
				}
				if (pw->lenth_name == tmpcount)
				{
					CloseHandle(hFile);
					CloseHandle(hEvent);
					return 0;
				}
				
			}
			count = 0;
        }

        stOverlapped.Offset = stOverlapped.Offset + dwBytesRead;
        if (stOverlapped.Offset < dwFileSize)
            bContinue = TRUE;
    }

    CloseHandle(hFile);
    CloseHandle(hEvent);

    return 1;
}

void ErrorExit(char *lpszMessage)
{
    // Restore input mode on exit.
	CloseHandle(hFile);
	CloseHandle(hEvent);
	free(usrbuff);
	free(pswbuff);
    WriteConsole(handle_out, lpszMessage, strlen(lpszMessage), &dw, NULL);
    ExitProcess(0);
}