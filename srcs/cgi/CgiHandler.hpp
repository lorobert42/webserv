#pragma once

#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "../config/ConfigServer.hpp"

class CgiHandler
{
	public:
		CgiHandler(ConfigServer* config);
		CgiHandler(CgiHandler const &src);
		~CgiHandler();

		CgiHandler   	&operator=(CgiHandler const &rhs);
		std::string		executeCgi();

	private:
		CgiHandler();
		char**								_getEnv();
		std::map<std::string, std::string>	_env;
		std::string							_body;
};