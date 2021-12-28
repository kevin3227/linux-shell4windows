 #include "login.h"
 #include <windows.h>

 HANDLE handle_in;
 HANDLE handle_out;
 DWORD dw;

 // /* TODO */

 int main(int argc, TCHAR *argv[])
 {
     struct login_context cxt;
     if (login(&cxt))
     {
         return 0;
     }
     // WriteConsole(handle_out, "\nLOGIN SUCCEED\n", strlen("\nLOGIN SUCCEED\n"), &dw, NULL);
     //CloseHandle(handle_in);
     //CloseHandle(handle_out);

     // console();
     STARTUPINFO si;
     PROCESS_INFORMATION pi;
     ZeroMemory(&si, sizeof(si));
     si.cb = sizeof(si);
     ZeroMemory(&pi, sizeof(pi));

     // Start the child process.
     if (!CreateProcess(NULL,          // No module name (use command line)
                        ".\\main.exe", // Command line
                        NULL,          // Process handle not inheritable
                        NULL,          // Thread handle not inheritable
                        FALSE,         // Set handle inheritance to FALSE
                        0,             // No creation flags
                        NULL,          // Use parent's environment block
                        NULL,          // Use parent's starting directory
                        &si,           // Pointer to STARTUPINFO structure
                        &pi)           // Pointer to PROCESS_INFORMATION structure
     )
     {
         WriteConsole(handle_out, "\nCREATE PROCESS FAILED\n", strlen("\nCREATE PROCESS FAILED\n"), &dw, NULL);
         return GetLastError();
     }
     // WriteConsole(handle_out, "\nCREATE PROCESS SUCCEED\n", strlen("\nCREATE PROCESS SUCCEED\n"), &dw, NULL);

     // Wait until child process exits.
     WaitForSingleObject(pi.hProcess, INFINITE);

     WriteConsole(handle_out, "\nPROCESS FINISHED\n", strlen("\nPROCESS FINISHED\n"), &dw, NULL);
     // Close process and thread handles.
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);

     system("pause");

     return 0;
 }
