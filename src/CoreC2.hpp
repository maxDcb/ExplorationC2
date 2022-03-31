#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <random>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <SocketHandler.hpp>

#include "C2Message.pb.h"


class CoreC2
{

public:
	CoreC2();
	~CoreC2();

	int run();

private:
	bool execInstruction(std::vector<std::string>& splitedCmd, C2Message& c2Message);

	int PingPong(C2Message &c2Message, C2Message &c2RetMessage);

	boost::asio::io_service m_ioService;

	int m_idxSession;

	std::vector<Server*> m_serversTcp;

	int createListner();
	void createServerTcp(int port);

	int connectSession(int idSession);

};
