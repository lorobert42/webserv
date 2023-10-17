#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(ConfigServer* config) {
	(void)config;

	// TODO: Set environment variables dedicated for the server
	this->_env["SERVER_SOFTWARE"] = "TODO";
	this->_env["SERVER_NAME"] = "TODO";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// TODO: Set environment variables dedicated for the request
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_PORT"] = "TODO";
	this->_env["REQUEST_METHOD"] = "TODO";
	this->_env["PATH_INFO"] = "TODO";
	this->_env["PATH_TRANSLATED"] = "TODO";
	this->_env["SCRIPT_NAME"] = "TODO";
	this->_env["QUERY_STRING"] = "TODO";
	this->_env["REMOTE_HOST"] = "TODO";
	this->_env["REMOTE_ADDR"] = "TODO";
	this->_env["AUTH_TYPE"] = "TODO";
	this->_env["REMOTE_USER"] = "TODO";
	this->_env["REMOTE_IDENT"] = "TODO";
	this->_env["CONTENT_TYPE"] = "TODO";
	this->_env["CONTENT_LENGTH"] = "TODO";

	// TODO: Set environment variables from the client
	this->_env["HTTP_ACCEPT"] = "TODO";
	this->_env["HTTP_ACCEPT_CHARSET"] = "TODO";
	this->_env["HTTP_ACCEPT_ENCODING"] = "TODO";
	this->_env["HTTP_ACCEPT_LANGUAGE"] = "TODO";
	this->_env["HTTP_CONNECTION"] = "TODO";
	this->_env["HTTP_HOST"] = "TODO";
	this->_env["HTTP_REFERER"] = "TODO";
	this->_env["HTTP_USER_AGENT"] = "TODO";
	this->_env["HTTP_COOKIE"] = "TODO";
}

CgiHandler::CgiHandler(CgiHandler const &src) {
	*this = src;
}

CgiHandler::~CgiHandler() {}

CgiHandler   	&CgiHandler::operator=(CgiHandler const &rhs)
{
	if (this != &rhs)
	{
		this->_env = rhs._env;
		this->_body = rhs._body;
	}
	return (*this);
}

char	**CgiHandler::_getEnv() {
	char**	env = new char*[this->_env.size() + 1];
	int		i = 0;

	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++) {
		env[i] = new char[it->first.length() + it->second.length() + 2];
		env[i] = strcat(env[i], it->first.c_str());
		env[i] = strcat(env[i], "=");
		env[i] = strcat(env[i], it->second.c_str());
		i++;
	}
	env[i] = NULL;
	return (env);
}

std::string	CgiHandler::executeCgi() {
	pid_t		pid = fork();
	char**		env = this->_getEnv();
	std::string	body;

	if (pid == -1)
		throw std::runtime_error("[CGI] fork() failed");

	// TODO

	// Free env
	for (int i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	return (body);
}