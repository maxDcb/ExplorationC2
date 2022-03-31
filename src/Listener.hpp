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


class Listener
{

public:
	Listener(int idxSession);
	~Listener();

	int connectSession();

protected:
	virtual void PingPong(C2Message& c2Message, C2Message& c2RetMessage) {};

	bool execInstruction(std::vector<std::string>& splitedCmd, C2Message& c2Message);

	int m_idxSession;

private:
	
	
};
