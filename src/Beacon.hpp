#pragma once

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

protected:
	bool execInstruction(C2Message& c2Message, C2Message& c2RetMessage);

	std::vector<std::unique_ptr<std::thread>> m_threadsExec;
};
