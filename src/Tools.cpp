#include "tools.hpp"


void splitList(std::string list, std::string& delimiter, std::vector<std::string>& splitedList)
{
	size_t pos = 0;
	std::string token;
	while ((pos = list.find(delimiter)) != std::string::npos)
	{
		token = list.substr(0, pos);
		splitedList.push_back(token);
		list.erase(0, pos + delimiter.length());
	}
	splitedList.push_back(list);
}
