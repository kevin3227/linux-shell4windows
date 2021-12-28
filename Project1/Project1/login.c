#include <windows.h>
#include <conio.h>
#include "login.h"
#include "commd.h"
#include <assert.h>

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

HANDLE hEvent;
HANDLE hFile;

DWORD oldIMode;
DWORD iMode;

char *usrbuff;
char *pswbuff;

struct passwd pw;

//command "login"
BOOL login(struct login_context *cxt)
{
	if (Login_initial(cxt))
	{
		return GetLastError();
	}
	cls(handle_out);
	WriteConsole(handle_out, "login: ", strlen("login: "), &dw, NULL);
	//get username
	ReadConsole(handle_in, usrbuff, 32, &dw, NULL);
	//WriteConsole(handle_out, usrbuff, dget_inputlenth(usrbuff), &dw, NULL);
	usrbuff[dget_inputlenth(usrbuff)] = '\0';
	//check input username whether in saved user_file
	if (!bget_usr(usrbuff, cxt))
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
			pswbuff[dget_inputlenth(pswbuff)] = '\0';

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
	example: string/r/n***  to 6.
*/
DWORD dget_inputlenth(char *inputbuff)
{

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

DWORD Login_initial(struct login_context *cxt)
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
	pw.pw_name = NULL;
	pw.pw_passwd = NULL;

	//get struct to save login infomation

	cxt->pid = 0; /* PID */
	cxt->pwd = &pw;

	//get old console input mode
	oldIMode = 0;
	if (!GetConsoleMode(handle_in, &oldIMode))
	{
		return GetLastError();
	}
	usrbuff = (char *)malloc(sizeof(char) * 32);
	if (!usrbuff)
	{
		return GetLastError();
	}

	pswbuff = (char *)malloc(sizeof(char) * 32);
	if (!pswbuff)
	{
		return GetLastError();
	}
	cxt->pwd->pw_name = (char *)malloc(sizeof(char) * 32);
	if (!cxt->pwd->pw_name)
	{
		return GetLastError();
	}
	cxt->pwd->pw_passwd = (char *)malloc(sizeof(char) * 32);
	if (!cxt->pwd->pw_passwd)
	{
		return GetLastError();
	}
	cxt->uid = (char *)malloc(sizeof(char) * 4);
	if (!cxt->uid)
	{
		return GetLastError();
	}
	return 0;
}

/*
	check password is right.
	usrbuff: password to be check
	pw: saved user infomation
*/

BOOL icheck_psw(char *pswbuff, struct passwd *pw)
{

	DWORD lenth = 0, result;
	int count = 0;
	char *pswhash;

	pswhash = HashMD5(pswbuff, &result);
	if (result && pw->lenth_passwd != 32)
	{
		WriteConsole(handle_out, "\n PASSWORD WRONG! \n", strlen("\n PASSWORD WRONG! \n"), &dw, NULL);
		return result;
	}
	else
	{
		while (pswhash[count] == pw->pw_passwd[count] && count < pw->lenth_passwd)
		{
			count++;
		}
		if (count == 32)
		{
			// WriteConsole(handle_out, "\nCHECK PASSWORD SUCCEED!\n", strlen("\nCHECK PASSWORD SUCCEED!\n"), &dw, NULL);
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
BOOL bget_usr(char *usrbuff, struct login_context *cxt)
{
	if (bfile_init())
	{
		return 1;
	}
	DWORD dwBytesRead = 0;                             //actual lenth of readbytes
	BOOL bContinue = TRUE;                             //read file control sign
	OVERLAPPED stOverlapped = { 0 };                     //read file info(offset)
	stOverlapped.hEvent = hEvent;                      //read event
	char *rbuff = (char *)malloc(1);                   //a char buffer read from file
	DWORD dwFileSize = GetFileSize(hFile, NULL);       //get file size
	char *tmpbuff = (char *)malloc(sizeof(char) * 48); //a string buffer from file
	int count = 0, tmpcount = 0;                       // read buffer offset

	while (bContinue)
	{
		bContinue = FALSE;
		ReadFile(hFile, rbuff, 1, &dwBytesRead, &stOverlapped);
		// WriteConsole(handle_out, rbuff, 1, &dw, NULL);
		tmpbuff[count] = *rbuff;
		count++;
		//save a group of parameters from a line
		if (*rbuff == ':')
		{
			if (tmpcount == 0)
			{
				tmpbuff[--count] = '\0';
				tmpcount = !tmpcount;
				cxt->pwd->lenth_name = count;
				if (cxt->pwd->lenth_name == dget_inputlenth(usrbuff))
				{
					BOOL result = bcpstring(tmpbuff, cxt->pwd->pw_name, count);
					if (result)
					{
						WriteConsole(handle_out, "COPY NAME ERROR", strlen(" COPY NAME ERROR "), &dw, NULL);
						return 1;
					}
				}
				count = 0;
			}
			else if (tmpcount == 1)
			{
				tmpbuff[--count] = '\0';
				tmpcount = !tmpcount;
				cxt->pwd->lenth_passwd = count;
				if (bcpstring(tmpbuff, cxt->pwd->pw_passwd, count))
				{
					WriteConsole(handle_out, "COPY PASSWORD ERROR", strlen(" COPY PASSWORD ERROR "), &dw, NULL);
					return 1;
				}
				count = 0;
			}
		}
		//save the last parameter
		else if (*rbuff == '\r')
		{
			tmpbuff[--count] = '\0';
			int i = count;
			while (i && i < 4)
			{
				i--;
				cxt->uid[i] = tmpbuff[i];
			}
			cxt->uid[count] = '\0';
			// WriteConsole(handle_out, cxt->uid, strlen(cxt->uid), &dw, NULL);
		}
		//check input_user is in this line.
		else if (*rbuff == '\n')
		{
			int i = 0;
			while (usrbuff[i] == cxt->pwd->pw_name[i] && i < 32)
			{
				i++;
			}
			if (cxt->pwd->lenth_name == (i - 1))
			{
				CloseHandle(hFile);
				CloseHandle(hEvent);
				return 0;
			}
			else
			{
				count = 0;
			}
		}

		stOverlapped.Offset = stOverlapped.Offset + dwBytesRead;
		if (stOverlapped.Offset < dwFileSize)
			bContinue = TRUE;
	}

	CloseHandle(hFile);
	CloseHandle(hEvent);

	return 1;
}

/*
	a hash function to hash a string
	return a hashstring
*/
char *HashMD5(char *data, DWORD *result)
{
	DWORD dwStatus = 0;
	DWORD cbHash = 16;
	int i = 0;
	HCRYPTPROV cryptProv;
	HCRYPTHASH cryptHash;
	BYTE hash[16];
	char *hex = "0123456789abcdef";
	char *strHash;
	strHash = (char *)malloc(500);
	memset(strHash, '\0', 500);
	if (!CryptAcquireContext(&cryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		*result = dwStatus;
		return NULL;
	}
	if (!CryptCreateHash(cryptProv, CALG_MD5, 0, 0, &cryptHash))
	{
		dwStatus = GetLastError();
		CryptReleaseContext(cryptProv, 0);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptHashData(cryptHash, (BYTE *)data, strlen(data), 0))
	{
		dwStatus = GetLastError();
		CryptReleaseContext(cryptProv, 0);
		CryptDestroyHash(cryptHash);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptGetHashParam(cryptHash, HP_HASHVAL, hash, &cbHash, 0))
	{
		dwStatus = GetLastError();
		CryptReleaseContext(cryptProv, 0);
		CryptDestroyHash(cryptHash);
		*result = dwStatus;
		return NULL;
	}
	for (i = 0; i < cbHash; i++)
	{
		strHash[i * 2] = hex[hash[i] >> 4];
		strHash[(i * 2) + 1] = hex[hash[i] & 0xF];
	}
	CryptDestroyHash(cryptHash);
	CryptReleaseContext(cryptProv, 0);
	return strHash;
}

BOOL bcpstring(char *source_str, char *taget_str, int lenth)
{
	int stroffset = lenth;
	if (lenth < 0 || lenth > 32)
	{
		return 1;
	}

	while (stroffset)
	{
		stroffset--;
		taget_str[stroffset] = source_str[stroffset];
	}
	taget_str[lenth] = '\0';
	return 0;
}

BOOL bfile_init()
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
	return 0;
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