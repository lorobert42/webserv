#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(ConfigServer* config) {

	// Set environment variables dedicted for the server
	this->_env["SERVER_SOFTWARE"] = "TODO";
	this->_env["SERVER_NAME"] = "TODO";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// Set environment variables dedicted for the request
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

	// Set environment variables from the client
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

