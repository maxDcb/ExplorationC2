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
		int localPort = 8080;
		string localHost = "127.0.0.1";
		string type = "http";

		string input;
		cout << "listener> ";
		std::getline(std::cin, input);

		if (!input.empty())
		{
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
			else if (splitedCmd[0] == "set")
			{
				if (splitedCmd.size() > 3)
				{
					if (splitedCmd[1] == "type")
					{
						if (splitedCmd[2] == "tcp")
							type = "tcp";
						else if (splitedCmd[2] == "http")
							type = "http";
						else
							std::cout << "type " << splitedCmd[1] << " unknown" << std::endl;
					}
					else if (splitedCmd[1] == "LPORT")
					{
						localPort = stoi(splitedCmd[2]);
					}
					else if (splitedCmd[1] == "LPORT")
					{
						localHost = splitedCmd[2];
					}
				}
				else
				{
					std::cout << "set LPORT 4444" << std::endl;
				}
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
		std::getline(std::cin, input);

		if (!input.empty())
		{
			std::vector<std::string> splitedCmd;
			std::string delimiter = " ";
			splitList(input, delimiter, splitedCmd);

			if (splitedCmd[0] == "listener")
			{
				createlistener(listeners);
			}
			else if (splitedCmd[0] == "session")
			{
				if (splitedCmd.size() == 2)
				{
					int idxSessionToInteractWith = stoi(splitedCmd[1]);
					for (int i = 0; i < listeners.size(); i++)
					{
						if (listeners[i]->getIdxSession() == idxSessionToInteractWith)
							listeners[i]->connectSession();
					}
				}
				else
				{
					for (int i = 0; i < listeners.size(); i++)
						std::cout << "session " << listeners[i]->getIdxSession() << std::endl;
				}
			}
			else if (splitedCmd[0] == "exit")
				exit = true;
		}
		else
			std::cout << std::endl;
	}

}

