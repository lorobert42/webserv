#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Response *response) :
	_response(response),
	_bodyContent(_response->getRequest()->getBody()),
	_statusCode(200)
{

	// Set environment variables dedicated for the server
	this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
	this->_env["SERVER_NAME"] = response->getConfigServer()->getName();
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// TODO: Set environment variables dedicated for the request
	this->_env["SERVER_PROTOCOL"] = response->getRequest()->getVersion();
	this->_env["SERVER_PORT"] = response->getConfigServer()->getPortAsString();
	this->_env["REQUEST_METHOD"] = response->getRequest()->getMethod();
	this->_env["REMOTE_HOST"] = ""; // TODO: Get the hostname of the client
	this->_env["REMOTE_ADDR"] = ""; // TODO: Get the IP address of the client
	this->_env["AUTH_TYPE"] = response->getRequest()->getValue("Authorization");
	this->_env["REMOTE_USER"] = response->getRequest()->getValue("Authorization");
	this->_env["REMOTE_IDENT"] = response->getRequest()->getValue("Authorization");
	this->_env["CONTENT_TYPE"] = response->getRequest()->getValue("Content-Type");
	this->_env["CONTENT_LENGTH"] = response->getRequest()->getValue("Content-Length");
	this->_env["HTTP_X_FILENAME"] = response->getRequest()->getValue("X-Filename");

	// Set environment variables from the response
	this->_env["HTTP_ACCEPT"] = response->getRequest()->getValue("Accept");
	this->_env["HTTP_ACCEPT_CHARSET"] = response->getRequest()->getValue("Accept-Charset");
	this->_env["HTTP_ACCEPT_ENCODING"] = response->getRequest()->getValue("Accept-Encoding");
	this->_env["HTTP_ACCEPT_LANGUAGE"] = response->getRequest()->getValue("Accept-Language");
	this->_env["HTTP_CONNECTION"] = response->getRequest()->getValue("Connection");
	this->_env["HTTP_HOST"] = response->getRequest()->getValue("Host");
	this->_env["HTTP_REFERER"] = response->getRequest()->getValue("Referer");
	this->_env["HTTP_USER_AGENT"] = response->getRequest()->getValue("User-Agent");
	this->_env["HTTP_COOKIE"] = response->getRequest()->getValue("Cookie");

	// Set environment variables dedicated for PHP
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SCRIPT_FILENAME"] = response->getConfigRoute()->getCgiScript();
	this->_env["SCRIPT_CGI"] = response->getConfigRoute()->getCgiBin();
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
		this->_response = rhs._response;
		this->_bodyContent = rhs._bodyContent;
	}
	return (*this);
}

void	freeEnv(char** env) {
	for (int i = 0; env[i]; i++) {
		delete[] env[i];
	}
	delete[] env;
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
	int inpipefd[2]; // For writing input to CGI script
	int outpipefd[2]; // For reading output from CGI script
	char buffer[4096];
	int bytesRead;
	std::string response;
	char** env = this->_getEnv();

	if (pipe(inpipefd) == -1 || pipe(outpipefd) == -1) {
		throw std::runtime_error("[CGI] pipe() failed");
	}
	if ((pid = fork()) == -1) {
		throw std::runtime_error("[CGI] fork() failed");
	}
	if (pid == 0) {
		close(outpipefd[0]); // Close the read end of the out pipe
		close(inpipefd[1]);  // Close the write end of the in pipe

		if (dup2(inpipefd[0], STDIN_FILENO) == -1 || dup2(outpipefd[1], STDOUT_FILENO) == -1) {
			perror("dup2");
			exit(EXIT_FAILURE);
		}
		close(inpipefd[0]);
		close(outpipefd[1]);

		char* argv[3];
		argv[0] = const_cast<char*>(this->_env["SCRIPT_CGI"].c_str());
		argv[1] = const_cast<char*>(this->_env["SCRIPT_FILENAME"].c_str());
		argv[2] = NULL;

		if (execve(argv[0], argv, env) == -1) {
			perror("execve");
			exit(EXIT_FAILURE);
		}
	} else {
		close(outpipefd[1]); // Close the write end of the out pipe
		close(inpipefd[0]);  // Close the read end of the in pipe

		// If there's body content, write it to the input of the child process.
		if (!_bodyContent.empty()) {
			write(inpipefd[1], _bodyContent.c_str(), _bodyContent.length());
		}
		close(inpipefd[1]); // Close the write end once data is written

		int status;
		time_t start = time(NULL);
		while (waitpid(pid, &status, WNOHANG) == 0) {
			time_t	now = time(NULL);
			if (difftime(now, start) > _response->getConfigServer()->getTimeout()) {
				kill(pid, SIGKILL);
				_statusCode = 508;
				break;
			}
		}

		while ((bytesRead = read(outpipefd[0], buffer, sizeof(buffer))) > 0) {
			response.append(buffer, bytesRead);
		}

		close(outpipefd[0]);
	}
	freeEnv(env);

	// Get the status code from the response
	if (response.find("Status: ") != std::string::npos) {
		std::string statusCode = response.substr(response.find("Status: ") + 8, 3);
		_statusCode = std::atoi(statusCode.c_str());
	}

	return response;
}

int	CgiHandler::getStatusCode() const {
	return _statusCode;
}