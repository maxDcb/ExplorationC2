#include "ListenerTcp.hpp"
#include "ListenerHttp.hpp"
#include "Tools.hpp"


using namespace std;


int IdxSession = 0;


int createlistener(std::vector<std::unique_ptr<Listener>>& listeners)
{
	bool exit = false;
	while (!exit)
	{
		int localPort = 5555;
		string localHost = "127.0.0.1";
		string type = "http";

		string input;
		cout << "listener> ";
		cin >> input;

		std::vector<std::string> splitedCmd;
		std::string delimiter = " ";
		splitList(input, delimiter, splitedCmd);

		if (splitedCmd[0] == "exit")
			exit = true;
		else if (splitedCmd[0] == "info")
		{
			std::cout << "TYPE " << type << std::endl;
			std::cout << "LPORT " << localPort << std::endl;
			std::cout << "LHOST " << localHost << std::endl;
		}
		else if (splitedCmd[0] == "type")
		{
			if (splitedCmd[1] == "tcp")
				type = "tcp";
			else if (splitedCmd[1] == "http")
				type = "http";
		}
		else if (splitedCmd[0] == "run")
		{
			if (type == "tcp")
			{
				std::unique_ptr<ListenerTcp> listenerTcp = make_unique<ListenerTcp>(IdxSession, localPort);
				listeners.push_back(std::move(listenerTcp));
				IdxSession++;
			}
			else if (type == "http")
			{
				std::unique_ptr<ListenerHttp> listenerHttp = make_unique<ListenerHttp>(IdxSession, localPort);
				listeners.push_back(std::move(listenerHttp));
				IdxSession++;
			}
			else
			{
				std::cout << "type " << type << " unknown" << std::endl;
			}

			exit = true;
		}
	}

	return 0;
}


int main(int argc, char* argv[])
{
	std::vector<std::unique_ptr<Listener>> listeners;

	bool exit = false;
	while (!exit)
	{
		cout << "C2> ";
		string input;
		cin >> input;

		if (input == "listener")
		{
			createlistener(listeners);
		}
		else if (input == "session")
		{
			listeners[0]->connectSession();
		}
		else if (input == "exit")
			exit = true;
	}

}

