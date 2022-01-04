//--------------------------------------------------------------------
//  Declare variables.
#include "windows.h"
#include <stdio.h>
//--------------------------------------------------------------------
// Get a handle to a cryptography provider context.
int main(int argc, char *argv[])
{
    // argv[0] = ".\1.exe";
    // argv[1] = "%s%d\\n\\t%f";
    // argv[3] = "hello";
    // argv[2] = "1.0";
    // argv[4] = "20";
    // argc = 5;
    char *p = argv[1];
    int value = 2;
    while (*p != '\0' && value < argc)
    {
        if (*p != '%')
        {
            if (*p != '\\')
            {
                printf("%c", *p++);
                continue;
            }
            int code;
            p++;
            switch (*p++)
            {
            case 'r':
                code = 13;
                break;
            case 'n':
                code = 10;
                break;
            case 't':
                code = 9;
                break;
            default:
                break;
            }
            printf("%c", code);
            continue;
        }
        p++;
        switch (*p++)
        {
        case 'd':
            char *tmp = argv[value];
            while (*tmp != '\0')
            {
                if (*tmp > '9' || *tmp < '0')
                {
                    printf("error");
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
                printf("error");
                exit(1);
            }
            break;

        case 'f':
            char *tmp2 = argv[value];
            while (*tmp2 != '\0')
            {
                if ((*tmp2 > '9' || *tmp2 < '0') && *tmp2 != '.')
                {
                    printf("error");
                    exit(1);
                }
                tmp2++;
            }
            break;

        default:
            printf("error");
            break;
        }
        printf("%s", argv[value++]);
    }

    return 0;
}
