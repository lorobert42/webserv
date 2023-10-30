#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
#include "../client/Client.hpp"
#include "../client/Request.hpp"

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
		Client								*_client;
};