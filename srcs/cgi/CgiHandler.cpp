/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:08:37 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 16:19:19 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() :
	_response(NULL),
	_bodyContent(""),
	_statusCode(0)
{}

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
		this->_statusCode = rhs._statusCode;
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
	pid_t		pid;
	int			fd[2]; // For writing input to CGI script
	char 		buffer[4096];
	int 		bytesRead;
	std::string	response;
	char** 		env = this->_getEnv();

	// Create a temporary file to store the output of the CGI script
	char tempFileName[] = "/tmp/cgi_output.XXXXXX";
	int tempFileDescriptor = mkstemp(tempFileName);
	if (tempFileDescriptor == -1) {
		throw std::runtime_error("[CGI] mkstemp() failed");
	}

	if (pipe(fd) == -1) {
		throw std::runtime_error("[CGI] pipe() failed");
	}

	if ((pid = fork()) == -1) {
		throw std::runtime_error("[CGI] fork() failed");
	}

	if (pid == 0) { // Child process
		close(fd[1]);  // Close the write end of the in pipe

		// Redirection de la sortie standard vers le fichier temporaire
		if (dup2(fd[0], STDIN_FILENO) == -1 || dup2(tempFileDescriptor, STDOUT_FILENO) == -1) {
			perror("dup2");
			exit(EXIT_FAILURE);
		}
		close(fd[0]);
		close(tempFileDescriptor);

		char* argv[3];
		argv[0] = const_cast<char*>(this->_env["SCRIPT_CGI"].c_str());
		argv[1] = const_cast<char*>(this->_env["SCRIPT_FILENAME"].c_str());
		argv[2] = NULL;

		if (execve(argv[0], argv, env) == -1) {
			perror("execve");
			exit(EXIT_FAILURE);
		}
	} else { // Parent process
		close(fd[0]);  // Close the read end of the in pipe

		// Write body content to the input of the child process, if any
		if (!_bodyContent.empty()) {
			write(fd[1], _bodyContent.c_str(), _bodyContent.length());
		}
		close(fd[1]); // Close the write end once data is written

		// Wait for the child process to terminate or kill it after 5 seconds
		int status;
		time_t start = time(NULL);
		while (waitpid(pid, &status, WNOHANG) == 0) {
			time_t	now = time(NULL);
			if (difftime(now, start) > D_CGI_TIMEOUT) {
				kill(pid, SIGKILL);
				_statusCode = 508;
				break;
			}
		}

		close(tempFileDescriptor);

		// Open the temporary file
		FILE* tempFile = fopen(tempFileName, "r");
		if (tempFile == NULL) {
			throw std::runtime_error("[CGI] fopen() failed");
		}

		// Read the content of the temporary file
		while ((bytesRead = fread(buffer, 1, sizeof(buffer), tempFile)) > 0) {
			response.append(buffer, bytesRead);
		}

		// Close and delete the temporary file
		fclose(tempFile);
		remove(tempFileName);
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
