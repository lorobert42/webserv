#pragma once

#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "../client/Client.hpp"

class Client;

class CgiHandler
{
	public:
		CgiHandler(Client  *client);
		CgiHandler(CgiHandler const &src);
		~CgiHandler();

		CgiHandler   	&operator=(CgiHandler const &rhs);
		std::string		executeCgi();
		void			displayEnv();

	private:
		CgiHandler();
		char**								_getEnv();
		std::map<std::string, std::string>	_env;
		std::string							_body;
};