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

	void connectSession();

private:
	void lauchHttpServ();

	std::string setCmd(const httplib::Request& req, httplib::Response& res);
	std::string getResponse(const httplib::Request& req, httplib::Response& res);

	std::unique_ptr<std::thread> m_httpServ;
	std::mutex m_mutex;

	std::string m_cmd;
};
