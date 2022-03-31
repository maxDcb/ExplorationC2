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


class Beacon
{

public:
	Beacon();
	~Beacon();

	int runTcp();

private:
	bool execInstruction(C2Message& c2Message, C2Message& c2RetMessage);

	boost::asio::io_service m_ioService;

	int m_idxSession;

	std::vector<Client*> m_clientsTcp;

	void creatClientTcp(int port, std::string& ip);

	std::vector<std::unique_ptr<std::thread>> m_threadsExec;
};
