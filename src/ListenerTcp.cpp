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




