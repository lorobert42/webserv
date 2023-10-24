#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Client *client) {
	// TODO: Set environment variables dedicated for the server
	this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
	this->_env["SERVER_NAME"] = "TODO";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// TODO: Set environment variables dedicated for the request
	this->_env["SERVER_PROTOCOL"] = client->getRequest()->getVersion();
	this->_env["SERVER_PORT"] = client->getConfigServer()->getPort();
	this->_env["REQUEST_METHOD"] = client->getRequest()->getMethod();
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
	this->_env["HTTP_ACCEPT"] = client->getRequest()->getValue("Accept");
	this->_env["HTTP_ACCEPT_CHARSET"] = "TODO";
	this->_env["HTTP_ACCEPT_ENCODING"] = client->getRequest()->getValue("Accept-Encoding");
	this->_env["HTTP_ACCEPT_LANGUAGE"] = client->getRequest()->getValue("Accept-Language");
	this->_env["HTTP_CONNECTION"] = client->getRequest()->getValue("Connection");
	this->_env["HTTP_HOST"] = client->getRequest()->getValue("Host");
	this->_env["HTTP_REFERER"] = "TODO"; // client->getRequest()->getValue("Referer");
	this->_env["HTTP_USER_AGENT"] = client->getRequest()->getValue("User-Agent");
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

void	CgiHandler::displayEnv() {
	char**	env = this->_getEnv();

	for (int i = 0; env[i]; i++) {
		std::cout << env[i] << std::endl;
	}
}

char	**CgiHandler::_getEnv() {
	char**	env = new char*[this->_env.size() + 1];
	int		i = 0;

	for (std::map<std::string, std::string>::iterator it = this->_env.begin(); it != this->_env.end(); it++) {
		std::string tmp = it->first + "=" + it->second;
		env[i] = new char[tmp.length() + 1];
		strcpy(env[i], tmp.c_str());
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
	for (int i = 0; env[i]; i++) {
		delete[] env[i];
	}
		
	delete[] env;

	return (body);
}