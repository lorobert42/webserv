#pragma once

#include <iostream>
#include <string>
#include <map>
#include "../config/ConfigServer.hpp"

class CgiHandler
{
	public:
		CgiHandler(ConfigServer* config);
		CgiHandler(CgiHandler const &src);
		~CgiHandler();

		CgiHandler   	&operator=(CgiHandler const &rhs);
		std::string		executeCgi(const std::string &scriptName);

	private:
		CgiHandler();
		std::map<std::string, std::string>	_env;
		std::string							_body;
};