#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Client *client) : _client(client) {

	// TODO: Set environment variables dedicated for the server
	this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
	this->_env["SERVER_NAME"] = client->getConfigServer()->getName();
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// TODO: Set environment variables dedicated for the request
	this->_env["SERVER_PROTOCOL"] = client->getRequest()->getVersion();
	this->_env["SERVER_PORT"] = client->getConfigServer()->getPortAsString();
	this->_env["REQUEST_METHOD"] = client->getRequest()->getMethod();
	this->_env["REMOTE_HOST"] = ""; // TODO: Get the hostname of the client
	this->_env["REMOTE_ADDR"] = ""; // TODO: Get the IP address of the client
	this->_env["AUTH_TYPE"] = client->getRequest()->getValue("Authorization");
	this->_env["REMOTE_USER"] = client->getRequest()->getValue("Authorization");
	this->_env["REMOTE_IDENT"] = client->getRequest()->getValue("Authorization");
	this->_env["CONTENT_TYPE"] = client->getRequest()->getValue("Content-Type");
	this->_env["CONTENT_LENGTH"] = client->getRequest()->getValue("Content-Length");

	// TODO: Set environment variables from the client
	this->_env["HTTP_ACCEPT"] = client->getRequest()->getValue("Accept");
	this->_env["HTTP_ACCEPT_CHARSET"] = client->getRequest()->getValue("Accept-Charset");
	this->_env["HTTP_ACCEPT_ENCODING"] = client->getRequest()->getValue("Accept-Encoding");
	this->_env["HTTP_ACCEPT_LANGUAGE"] = client->getRequest()->getValue("Accept-Language");
	this->_env["HTTP_CONNECTION"] = client->getRequest()->getValue("Connection");
	this->_env["HTTP_HOST"] = client->getRequest()->getValue("Host");
	this->_env["HTTP_REFERER"] = client->getRequest()->getValue("Referer");
	this->_env["HTTP_USER_AGENT"] = client->getRequest()->getValue("User-Agent");
	this->_env["HTTP_COOKIE"] = client->getRequest()->getValue("Cookie");

	// Set environment variables dedicated for PHP
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SCRIPT_FILENAME"] = client->getConfigRoute()->getCgiScript();
	this->_env["SCRIPT_CGI"] = client->getConfigRoute()->getCgiBin();
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

std::string CgiHandler::executeCgi() {
	pid_t pid;
	int fd[2];
	char buffer[4096];
	int bytesRead;
	std::string response;
	char** env = this->_getEnv();

	// Create a pipe
	if (pipe(fd) == -1) {
		throw std::runtime_error("[CGI] pipe() failed");
	}
	// Fork a child process
	if ((pid = fork()) == -1) {
		throw std::runtime_error("[CGI] fork() failed");
	}
	// Child process
	if (pid == 0) {
		close(fd[0]); // Close the read end of the pipe

		// Redirect stdout to the write end of the pipe
		if (dup2(fd[1], STDOUT_FILENO) == -1) {
			perror("dup2");
			exit(EXIT_FAILURE);
		}
		close(fd[1]);

		int input_fd[2];
		if (pipe(input_fd) == -1) {
			perror("input pipe");
			exit(EXIT_FAILURE);
		}

		write(input_fd[1], _client->getRequest()->getBody().c_str(), _client->getRequest()->getBody().size());
		close(input_fd[1]);

		if (dup2(input_fd[0], STDIN_FILENO) == -1) {
			perror("dup2 stdin");
			exit(EXIT_FAILURE);
		}
		close(input_fd[0]);

		char* argv[3];
		argv[0] = const_cast<char*>(this->_env["SCRIPT_CGI"].c_str());
		argv[1] = const_cast<char*>(this->_env["SCRIPT_FILENAME"].c_str());
		argv[2] = NULL;

		if (execve(argv[0], argv, env) == -1) {
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
	// Parent process
	else {
		close(fd[1]); // Close the write end of the pipe

		while ((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0) {
			response.append(buffer, bytesRead);
		}

		close(fd[0]);
		waitpid(pid, NULL, 0); // Wait for the child process to finish
	}

	// Free env
	for (int i = 0; env[i]; i++) {
		delete[] env[i];
	}
	delete[] env;

	return response;
}

