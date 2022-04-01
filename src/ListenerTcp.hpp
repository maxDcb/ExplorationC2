#pragma once

#include "Listener.hpp"


class ListenerTcp : public Listener
{

public:
	ListenerTcp(int idxSession, int localport);
	~ListenerTcp();

	void connectSession();

private:
	void PingPong(C2Message &c2Message, C2Message &c2RetMessage);

	boost::asio::io_service m_ioService;

	int m_idxSession;

	std::vector<Server*> m_serversTcp;

};
