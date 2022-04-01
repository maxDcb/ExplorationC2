#include "ListenerTcp.hpp"


using namespace std;
using boost::asio::ip::tcp;


ListenerTcp::ListenerTcp(int idxSession, int localPort)
	: Listener(idxSession)
{
	Server* server_ = new Server(localPort);
	m_serversTcp.push_back(std::move(server_));
}


ListenerTcp::~ListenerTcp()
{
	for (int i = 0; i < m_serversTcp.size(); i++)
		if (m_serversTcp[i])
			delete m_serversTcp[i];
}


void ListenerTcp::connectSession()
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

	return;
}


void ListenerTcp::PingPong(C2Message &c2Message, C2Message &c2RetMessage)
{
	string out;
	c2Message.SerializeToString(&out);
	m_serversTcp[0]->send(out);

	string ret;
	m_serversTcp[0]->receive(ret);

	c2RetMessage.ParseFromArray(ret.data(), (int)ret.size()); 

	return;
}




