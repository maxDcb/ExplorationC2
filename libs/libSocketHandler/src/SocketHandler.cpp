#include "SocketHandler.hpp"

#include <iostream>


using boost::asio::ip::tcp;


void sendSocketTcp(boost::asio::ip::tcp::socket* socket_, char* data, int nbBytes, boost::system::error_code* err)
{
	boost::asio::const_buffers_1 buff(data, nbBytes);
	boost::asio::write(*socket_, buff, *err);
}


void readSocketTcp(boost::asio::ip::tcp::socket* socket_, char* data, int nbBytes, boost::system::error_code* err)
{
	boost::asio::mutable_buffers_1 buff(data, nbBytes);
	boost::asio::read(*socket_, buff, boost::asio::transfer_all(), *err);
}


Server::Server(int port)
{
	m_port = port;
	m_initDone=false;
	threadInit = new std::thread(&Server::initServer, this);
}

void Server::initServer()
{
	creatServerTcp(m_port);
	m_initDone=true;
}

Server::~Server()
{
	delete m_socketTcp;
}


void Server::creatServerTcp(int port)
{
	tcp::acceptor acceptor_(m_ioService, tcp::endpoint(tcp::v4(), port));
	m_socketTcp=new tcp::socket(m_ioService);
	acceptor_.accept(*m_socketTcp);
}


bool Server::send(std::string& data)
{
	if(!m_initDone)
	{
		std::cout << "Server: No connection" << std::endl;
		return false;
	}

	int nbBytes = data.size();
	sendSocketTcp(m_socketTcp, (char*)&nbBytes, sizeof(int), &m_error);

	if(m_error)
	{
		std::cerr << "send failed: " << m_error.message() << std::endl;
		return false;
	}

	sendSocketTcp(m_socketTcp, (char*)&data[0], nbBytes, &m_error);

	if(m_error)
	{
		std::cerr << "send failed: " << m_error << std::endl;
		return false;
	}

	return true;
}


bool Server::receive(std::string& data)
{
	int nbBytes=0;
	readSocketTcp(m_socketTcp, (char*)&nbBytes, sizeof(int), &m_error);
	data.resize(nbBytes);

	if(m_error)
	{
		std::cerr << "receive failed: " << m_error.message() << std::endl;
		return false;
	}

	readSocketTcp(m_socketTcp, &data[0], nbBytes, &m_error);


	if(m_error)
	{
		std::cerr << "receive failed: " << m_error.message() << std::endl;
		return false;
	}

	return true;
}


Client::Client(std::string& ip, int port)
{
	m_ipServer = ip;
	m_port = port;

	creatClientTcp(m_port, m_ipServer);
}


Client::~Client()
{
	delete m_socketTcp;
}


void Client::creatClientTcp(int port, std::string& ip)
{
	boost::system::error_code error;
	m_socketTcp=new tcp::socket(m_ioService);
	m_socketTcp->connect( tcp::endpoint( boost::asio::ip::address::from_string(ip), port ), error);
	while(error)
	{
		m_socketTcp->connect( tcp::endpoint( boost::asio::ip::address::from_string(ip), port ), error);
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000)));
//		if(error)
//			std::cerr << "Client connect failed: " << error.message() << std::endl;
	}

}


bool Client::send(std::string& data)
{
	int nbBytes = data.size();
	sendSocketTcp(m_socketTcp, (char*)&nbBytes, sizeof(int), &m_error);

	if(m_error)
	{
		std::cerr << "send failed: " << m_error.message() << std::endl;
		return false;
	}

	sendSocketTcp(m_socketTcp, (char*)&data[0], nbBytes, &m_error);

	if(m_error)
	{
		std::cerr << "send failed: " << m_error << std::endl;
		return false;
	}

	return true;
}


bool Client::receive(std::string& data)
{
	int nbBytes=0;
	readSocketTcp(m_socketTcp, (char*)&nbBytes, sizeof(int), &m_error);
	data.resize(nbBytes);

	if(m_error)
	{
		std::cerr << "receive failed: " << m_error.message() << std::endl;
		return false;
	}

	readSocketTcp(m_socketTcp, &data[0], nbBytes, &m_error);


	if(m_error)
	{
		std::cerr << "receive failed: " << m_error.message() << std::endl;
		return false;
	}

	return true;
}


