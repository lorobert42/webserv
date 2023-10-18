
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

		const std::string	&getMethod(void) const;
		const std::string	&getIndex(void) const;
		const std::string	&getVersion(void) const;
		const std::string	getValue(const std::string &key) const;
	private :
		Request(void);

		std::map<std::string, std::string>	_header;
		std::string	_method;
		std::string	_index;
		std::string	_version;
		std::string	_body;
};
