#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Client *client) {

	// TODO: Set environment variables dedicated for the server
	this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
	this->_env["SERVER_NAME"] = "TODO";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SCRIPT_FILENAME"] = "/home/webserv/www/cgi/env.cgi.php";

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

std::string CgiHandler::executeCgi() {
	pid_t pid;
	int pipefd[2];  // File descriptor array for the pipe
	char** env = this->_getEnv();
	std::string body;

	if (pipe(pipefd) == -1) {
		throw std::runtime_error("[CGI] pipe() failed");
	}

	pid = fork();

	if (pid < 0) {
		throw std::runtime_error("[CGI] fork() failed");
	}
	// This code will be executed by the child process
	else if (pid == 0) {
		// Close the read end of the pipe
		close(pipefd[0]);

		// Redirect stdout to the write end of the pipe
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		// Prepare the argument list
		char* argv[3] = {const_cast<char*>("/usr/bin/php-cgi82"), const_cast<char*>("/home/webserv/www/cgi/env.cgi.php"), NULL};

		// Execute the CGI script
		if (execve("/usr/bin/php-cgi82", argv, env) == -1) {
			perror("[CGI] execve() failed");
			_exit(EXIT_FAILURE);
		}
	}
	// This code will be executed by the parent process
	else {
		// Close the write end of the pipe
		close(pipefd[1]);

		// Read the output from the pipe into the 'body' variable
		char buffer[4096];
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			body.append(buffer, bytesRead);
		}

		// Wait for the child process to finish
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			perror("[CGI] waitpid() failed");
		} else {
			if (WIFEXITED(status)) {
				int exitStatus = WEXITSTATUS(status);
				// You can process the exit status here if needed
				(void)exitStatus;
			} else if (WIFSIGNALED(status)) {
				int terminatingSignal = WTERMSIG(status);
				// You can handle the signal termination here if needed
				(void)terminatingSignal;
			}
		}
		// Close the read end of the pipe
		close(pipefd[0]);
	}

	// Free env
	for (int i = 0; env[i]; i++) {
		delete[] env[i];
	}
	delete[] env;

	std::cout << "Body: " << body << std::endl;

	return body;
}
