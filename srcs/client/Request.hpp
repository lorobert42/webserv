
#pragma once

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

class Request
{
	public :
		Request(std::string read);
		Request(Request const &other);
		Request &operator=(Request const &other);
		~Request(void);

	private :
		Request(void);

		std::map<std::string, std::string>	_map;
		std::string	_method;
		std::string	_index;
		std::string	_version;
};
