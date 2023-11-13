#pragma once

#include <iostream>
#include <map>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <sys/wait.h>
#include <sys/types.h>
#include "../client/Response.hpp"
#include "../client/Request.hpp"

class Response;

class CgiHandler
{
	public:
		CgiHandler(Response  *response);
		CgiHandler(CgiHandler const &src);
		~CgiHandler();

		CgiHandler   	&operator=(CgiHandler const &rhs);
		std::string		executeCgi();
		int				getStatusCode() const;

	private:
		CgiHandler();
		char**								_getEnv();
		std::map<std::string, std::string>	_env;
		Response*							_response;
		std::string							_bodyContent;
		int									_statusCode;
};
