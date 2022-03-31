#pragma once

#include "Beacon.hpp"

#include "../thirdParty/cpp-httplib/httplib.h"


class BeaconHttp : public Beacon
{

public:
	BeaconHttp();
	~BeaconHttp();

	int runHttp();

private:

};
