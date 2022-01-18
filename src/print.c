#include "commd.h"
#include <windows.h>
void main(int argc, char *argv[])
{
    HANDLE handle_out;
    LARGE_INTEGER liCurrentPosition;
    liCurrentPosition.QuadPart = 0;
    if (!strcmp(OUTPUT_TYPE, PIPE)) {
		handle_out = CreateFile(PIPE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	} 
	else if (strcmp(OUTPUT_TYPE, "STD_OUTPUT")) {
		if(!strcmp(OVERWRITE, "FALSE")) {
			if (handle_out = CreateFile(OUTPUT_TYPE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0) == INVALID_HANDLE_VALUE) {
				handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
				WriteConsole(handle_out, "Failed to open file\n",
				strlen("Failed to open file\n"),
				&dw,
				NULL);
				return;
			}
			handle_out = CreateFile(OUTPUT_TYPE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			liCurrentPosition.QuadPart = 0;
			SetFilePointerEx(handle_out, liCurrentPosition, NULL, FILE_END);
		} 
		else {
			handle_out = CreateFile(OUTPUT_TYPE, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		}
	}
	else {
		handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	}
    DWORD dw;

    //argv : .\1.exe "%s%s%s" s1 s2 s3
    //argv[0] = ".\1.exe";
    //argv[1] = "%s %s %d";
    //argv[2] = "123";
    //argv[3] = "321";
    //argv[4] = "1";
    //argc = 5;

    char *p = argv[4];
    int value = 5; 
	int i; //position of s1

    char *tmp = NULL;
    char *outbuff = (char *)malloc(sizeof(char) * 256);
    int offset = 0;
    while (*p != '\0' && value <= argc)
    {
        if (*p != '%')
        {
            if (*p != '\\')
            {
                outbuff[offset++] = *p;
                p++;
                continue;
            }
            char code;
            p++;
            switch (*p++)
            {
            case 'r':
                code = '\r';
                break;
            case 'n':
                code = '\n';
                break;
            case 't':
                code = '\t';
                break;
            default:
                break;
            }
            outbuff[offset++] = code;
            p++;
            continue;
        }
        p++;
        switch (*p++)
        {
        case 'd':
            tmp = argv[value];
            while (*tmp && *tmp != '\0')
            {
                if (*tmp > '9' || *tmp < '0')
                {
                    exit(1);
                }
                tmp++;
            }
            break;
        case 's':
            break;
        case 'c':
            if (*(argv[value] + 1) != '\0')
            {
                exit(1);
            }
            break;
        case 'f':
            tmp = argv[value];
            while (*tmp && *tmp != '\0')
            {
                if ((*tmp > '9' || *tmp < '0') && *tmp != '.')
                {
                    exit(1);
                }
                tmp++;
            }
            break;
        default:
            break;
        }
        // WriteConsole(handle_out, argv[value], strlen(argv[value]), &dw, NULL);
        for (i = 0; argv[value][i] != '\0'; i++)
        {
            outbuff[offset++] = argv[value][i];
        }
        value++;
    }
    outbuff[offset] = '\0';
    WriteFile(handle_out, outbuff, strlen(outbuff), &dw, NULL);
    WriteFile(handle_out, '\n', strlen('\n'), &dw, NULL);
    free(outbuff);
}
