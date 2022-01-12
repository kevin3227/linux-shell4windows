BOOL passwd(char *argv, char *id)
{
    if (bpasswd_initial())
    {
        return 1;
    }
    char *name = (char *)malloc(sizeof(char) * 16);
    if (!name)
    {
        return 1;
    }

    if (bgetnamebyid(name, id))
    {
        return 1;
    }

    if (argv)
    {
        if (bcmpstring(name, "root"))
        {
            return 1;
        }
    }
    else
    {
        argv = name;
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

    if (bwritepsw(argv, pswbuff))
        return 1;
    free(name);
    free(psw1);
    WriteConsole(handle_out, "SUCCEED", strlen("SUCCEED"), &dw, NULL);
    return 0;
}
