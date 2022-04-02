#pragma once

#include "Beacon.hpp"


class BeaconTcp : public Beacon
{

public:
	BeaconTcp(std::string& ip, int port);
	~BeaconTcp();

	int runTcp();

private:

	boost::asio::io_service m_ioService;

	std::vector<Client*> m_clientsTcp;

	void creatClientTcp(int port, std::string& ip);
};
