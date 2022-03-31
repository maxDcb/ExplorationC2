#include "BeaconTcp.hpp"
#include "BeaconHttp.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "BeaconLaucher tcp / BeaconLaucher http";
		return -1;
	}

	std::string arg = argv[1];

	if (arg == "tcp")
	{
		BeaconTcp beaconTcp;
		beaconTcp.runTcp();
	}
	else if (arg == "http")
	{
		BeaconHttp Beaconhttp;
		Beaconhttp.runHttp();
	}
	else
	{
		std::cout << "type " << arg << " unknonwn" << std::endl;
	}
}