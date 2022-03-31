#pragma once

#include "Listener.hpp"

//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../thirdParty/cpp-httplib/httplib.h"

#include <mutex>


class ListenerHttp : public Listener
{

public:
	ListenerHttp(int idxSession, int localport);
	~ListenerHttp();

private:
	void lauchHttpServ();

	std::string test(const httplib::Request& req, httplib::Response& res);

	void PingPong(C2Message& c2Message, C2Message& c2RetMessage);

	std::unique_ptr<std::thread> m_httpServ;
	std::mutex m_mutex;

	std::string m_cmd;

	int m_idxSession;
};
