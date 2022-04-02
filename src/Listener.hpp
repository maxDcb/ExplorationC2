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

#include "Tools.hpp"


class Listener
{

public:
	Listener(int idxSession, int port);
	~Listener();

	virtual void connectSession() {};

	int getIdxSession()
	{
		return m_idxSession;
	}

protected:
	virtual void PingPong(C2Message& c2Message, C2Message& c2RetMessage) {};

	bool execInstruction(std::vector<std::string>& splitedCmd, C2Message& c2Message);

	int m_idxSession;

	int m_port;

private:
	
	
};
