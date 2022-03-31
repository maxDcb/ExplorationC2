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
	this->m_httpServ = std::make_unique<std::thread>(&ListenerHttp::lauchHttpServ, this);
}


ListenerHttp::~ListenerHttp()
{
}


void ListenerHttp::lauchHttpServ()
{

	std::cout << "lauchHttpServ" << std::endl;

	httplib::Server svr;
	httplib::Response res;

	svr.Get("/hi", [&](const httplib::Request& req, httplib::Response& res)
	{
		std::string testStr = this->test(req, res);
	});

	svr.listen("0.0.0.0", 8080);
}


std::string ListenerHttp::test(const Request& req, Response& res) 
{
	std::lock_guard<std::mutex> lock(m_mutex);

	res.body = m_cmd;

	std::string s;
	/*char buf[BUFSIZ];

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

	s += "\n";*/

	return s;
};


void ListenerHttp::PingPong(C2Message& c2Message, C2Message& c2RetMessage)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	c2Message.SerializeToString(&m_cmd);

	//c2RetMessage.ParseFromArray(ret.data(), (int)ret.size());

	return;
}







