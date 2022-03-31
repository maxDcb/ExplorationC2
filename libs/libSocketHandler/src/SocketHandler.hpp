#pragma once

#include <fstream>
#include <memory>
#include <chrono>
#include <random>
#include <vector>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>


class Server
{
public:
	Server(int port);
	~Server();

	bool send(std::string& data);
	bool receive(std::string& data);

private:
	void initServer();
	void creatServerTcp(int port);

	bool m_initDone;
	int m_port;

	std::thread* threadInit;

	boost::asio::io_service m_ioService;
	boost::asio::ip::tcp::socket* m_socketTcp;
	boost::system::error_code m_error;
};



class Client
{
public:
	Client(std::string& ip, int port);
	~Client();

	bool send(std::string& data);
	bool receive(std::string& data);

private:
	void creatClientTcp(int port, std::string& ip);

	std::string m_ipServer;
	int m_port;

	boost::asio::io_service m_ioService;
	boost::asio::ip::tcp::socket* m_socketTcp;
	boost::system::error_code m_error;
};


