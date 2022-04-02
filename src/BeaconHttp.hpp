#pragma once

#include "Beacon.hpp"

#include "../thirdParty/cpp-httplib/httplib.h"


class BeaconHttp : public Beacon
{

public:
	BeaconHttp(std::string& ip, int port);
	~BeaconHttp();

	int runHttp();

private:

};
