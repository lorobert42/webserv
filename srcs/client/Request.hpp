
#pragma once

#include <iostream>
#include <map>
#include <string>

class Request
{
	public :
		Request(std::string read);
		Request(Request const &other);
		Request &operator=(Request const &other);
		~Request(void);

	private :
		Request(void);

		std::map<std::string, std::string>	*_map;
};
