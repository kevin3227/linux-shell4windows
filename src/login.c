#include <windows.h>
#include <conio.h>
#include "login.h"
#include "commd.h"
#include <assert.h>

extern HANDLE handle_in;
extern HANDLE handle_out;
extern DWORD dw;

//command "login"
BOOL login(struct login_context *cxt)
{
    if (dLogin_initial(cxt))
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
            if (bcheck_psw(pswbuff, &pw))
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
BOOL passwd(char *argv[8], int *argc)
{
	char *id = argv[1];

	//argv[0] = passwd
	//argv[1] = id
	//argv[2] = user

    if (bpasswd_initial())
    {
        return 1;
    }
    char *name;
    name = (char *)malloc(sizeof(char) * 16);
    if (bgetnamebyid(name, id))
    {
        return 1;
    }
    if (argv[2])
    {
        if (bcmpstring(name, "root"))
        {
            return 1;
        }
    }
    else
    {
        argv[2] = name;
    }

    char *psw1 = (char *)malloc(sizeof(char) * 16);
    if (!psw1)
    {
        return 1;
    }
    if (bgetpswbyname(psw1, name))
    {
        return 1;
    }
    WriteConsole(handle_out, "\npassword: ", strlen("\npassword: "), &dw, NULL);
    // set console input disable echo mode

    // iMode = oldIMode;
    // iMode &= ~ENABLE_ECHO_INPUT;
    // if (!SetConsoleMode(handle_in, iMode))
    // {
    //     return GetLastError();
    // }

    //get passowrd
    ReadConsole(handle_in, pswbuff, 32, &dw, NULL);

    pswbuff[dget_inputlenth(pswbuff)] = '\0';

    char *pswhash;
    DWORD result = 0;
    pswhash = cHashMD5(pswbuff, &result);
    if (result)
    {
        return 1;
    }
    if (bcmpstring(pswhash, psw1))
    {
        return 1;
    }
    WriteConsole(handle_out, "(new)password: ", strlen("(new)password: "), &dw, NULL);
    ReadConsole(handle_in, pswbuff, 32, &dw, NULL);
    pswbuff[dget_inputlenth(pswbuff)] = '\0';

    //reset input_mode
    // if (!SetConsoleMode(handle_in, oldIMode))
    // {
    //     return GetLastError();
    // }

    if (bwritepsw(argv[2], pswbuff))
        return 1;
    WriteConsole(handle_out, "SUCCEED", strlen("SUCCEED"), &dw, NULL);
    return 0;
}

BOOL bpasswd_initial()
{
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
    oldIMode = 0;
    if (!GetConsoleMode(handle_in, &oldIMode))
    {
        return GetLastError();
    }
    pswbuff = (char *)malloc(sizeof(char) * 32);
    if (!pswbuff)
    {
        return GetLastError();
    }
    return 0;
}
//command "logout"
BOOL logout()
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

DWORD dLogin_initial(struct login_context *cxt)
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
BOOL bcheck_psw(char *pswbuff, struct passwd *pw)
{

    DWORD lenth = 0, result;
    int count = 0;
    char *pswhash;

    pswhash = cHashMD5(pswbuff, &result);
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
    if (brfile_init("usr.txt"))
    {
        return 1;
    }
    DWORD dwBytesRead = 0;                             //actual lenth of readbytes
    BOOL bContinue = TRUE;                             //read file control sign
    OVERLAPPED stOverlapped = {0};                     //read file info(offset)
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
char *cHashMD5(char *data, DWORD *result)
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

BOOL brfile_init(char *path)
{
    //creat handle event
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL)
    {
        return 1;
    }

    //creat handle file
    hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        WriteConsole(handle_out, "\n OPPEN FILE FAILED ! \n\tEXIT\n", strlen("\n OPPEN FILE FAILED ! \n\tEXIT\n"), &dw, NULL);
        return GetLastError();
    }
    return 0;
}

BOOL bwfile_init(char *path)
{
    //creat handle event
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL)
    {
        return 1;
    }

    //creat handle file
    // hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    hFile = CreateFile(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        WriteConsole(handle_out, "\n OPPEN FILE FAILED ! \n\tEXIT\n", strlen("\n OPPEN FILE FAILED ! \n\tEXIT\n"), &dw, NULL);
        return GetLastError();
    }
    return 0;
}

BOOL bgetnamebyid(char *name, char *uid)
{
    if (brfile_init("usr.txt"))
    {
        return 1;
    }
    DWORD dwBytesRead = 0;                             //actual lenth of readbytes
    BOOL bContinue = TRUE;                             //read file control sign
    OVERLAPPED stOverlapped = {0};                     //read file info(offset)
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
                BOOL result = bcpstring(tmpbuff, name, count);
                if (result)
                {
                    WriteConsole(handle_out, "COPY NAME ERROR", strlen(" COPY NAME ERROR "), &dw, NULL);
                    return 1;
                }
            }
            tmpcount++;
            count = 0;
        }
        //save the last parameter
        else if (*rbuff == '\r')
        {
            count--;

            // WriteConsole(handle_out, cxt->uid, strlen(cxt->uid), &dw, NULL);
        }
        //check input_user is in this line.
        else if (*rbuff == '\n')
        {

            tmpbuff[--count] = '\0';
            if (!bcmpstring(tmpbuff, uid))
            {
                // WriteConsole(handle_out, tmpbuff, strlen(tmpbuff), &dw, NULL);
                CloseHandle(hFile);
                CloseHandle(hEvent);
                return 0;
            }
            // WriteConsole(handle_out, tmpbuff, strlen(tmpbuff), &dw, NULL);
            // WriteConsole(handle_out, uid, strlen(uid), &dw, NULL);
            // WriteConsole(handle_out, "\nCMP ERROR\n", strlen("\nCMP ERROR\n"), &dw, NULL);
            tmpcount = 0;
            count = 0;
        }

        stOverlapped.Offset = stOverlapped.Offset + dwBytesRead;
        if (stOverlapped.Offset < dwFileSize)
            bContinue = TRUE;
    }

    free(tmpbuff);
    free(name);
    CloseHandle(hFile);
    CloseHandle(hEvent);

    return 1;
}

BOOL bgetpswbyname(char *psw, char *name)
{
    if (brfile_init("usr.txt"))
    {
        return 1;
    }
    DWORD dwBytesRead = 0;                             //actual lenth of readbytes
    BOOL bContinue = TRUE;                             //read file control sign
    OVERLAPPED stOverlapped = {0};                     //read file info(offset)
    stOverlapped.hEvent = hEvent;                      //read event
    char *rbuff = (char *)malloc(1);                   //a char buffer read from file
    DWORD dwFileSize = GetFileSize(hFile, NULL);       //get file size
    char *tmpbuff = (char *)malloc(sizeof(char) * 48); //a string buffer from file
    int count = 0, tmpcount = 0;                       // read buffer offset
    BOOL flag = 0;
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
                if (!bcmpstring(tmpbuff, name))
                {
                    flag = 1;
                }
            }
            if (tmpcount == 1 && flag)
            {
                tmpbuff[--count] = '\0';
                BOOL result = bcpstring(tmpbuff, psw, count);
                if (!result)
                {
                    // WriteConsole(handle_out, tmpbuff, strlen(tmpbuff), &dw, NULL);
                    CloseHandle(hFile);
                    CloseHandle(hEvent);
                    return 0;
                }
            }
            tmpcount++;
            count = 0;
        }
        //next line.
        else if (*rbuff == '\n')
        {
            count = 0;
            tmpcount = 0;
        }

        stOverlapped.Offset = stOverlapped.Offset + dwBytesRead;
        if (stOverlapped.Offset < dwFileSize)
            bContinue = TRUE;
    }

    free(tmpbuff);
    free(name);
    CloseHandle(hFile);
    CloseHandle(hEvent);

    return 1;
}

BOOL bwritepsw(char *name, char *psw)
{
    if (bwfile_init("usr.txt"))
    {
        return 1;
    }
    DWORD dwBytesRead = 0, pswoffset = 0;              //actual lenth of readbytes
    BOOL bContinue = TRUE;                             //read file control sign
    OVERLAPPED stOverlapped = {0};                     //read file info(offset)
    stOverlapped.hEvent = hEvent;                      //read event
    char *rbuff = (char *)malloc(1);                   //a char buffer read from file
    DWORD dwFileSize = GetFileSize(hFile, NULL);       //get file size
    char *tmpbuff = (char *)malloc(sizeof(char) * 48); //a string buffer from file
    int count = 0, tmpcount = 0;                       // read buffer offset
    BOOL flag = 0;
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
                if (!bcmpstring(tmpbuff, name))
                {
                    char *pswhash;
                    DWORD result = 0, dwbyteswritten;
                    pswoffset = stOverlapped.Offset + 1;
                    pswhash = cHashMD5(psw, &result);
                    if (result)
                    {
                        WriteConsole(handle_out, "\n PASSWORD WRONG! \n", strlen("\n PASSWORD WRONG! \n"), &dw, NULL);
                        return result;
                    }
                    result = WriteFile(
                        hFile,           // open file handle
                        pswhash,         // start of data to write
                        32,              // number of bytes to write
                        &dwbyteswritten, // number of bytes that were written
                        NULL);           // no overlapped structure
                    if (result && dwbyteswritten == 32)
                    {
                        WriteConsole(handle_out, tmpbuff, strlen(tmpbuff), &dw, NULL);
                        WriteConsole(handle_out, pswhash, strlen(pswhash), &dw, NULL);
                        WriteConsole(handle_out, psw, strlen(psw), &dw, NULL);
                        CloseHandle(hFile);
                        CloseHandle(hEvent);
                        return 0;
                    }
                }
            }
            tmpcount++;
            count = 0;
        }
        //next line.
        else if (*rbuff == '\n')
        {
            count = 0;
            tmpcount = 0;
        }

        stOverlapped.Offset = stOverlapped.Offset + dwBytesRead;
        if (stOverlapped.Offset < dwFileSize)
            bContinue = TRUE;
    }

    free(tmpbuff);
    free(name);
    CloseHandle(hFile);
    CloseHandle(hEvent);

    return 1;
}

BOOL bcmpstring(char *str1, char *str2)
{
    int i = 0;
    while (str1[i] == str2[i] && i < 32 && str1[i] != '\0')
    {
        i++;
    }
    if (i == 0)
    {
        return 1;
    }

    return 0;
}