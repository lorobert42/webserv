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
	pid_t pid;
	int inpipefd[2]; // For writing input to CGI script
	int outpipefd[2]; // For reading output from CGI script
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	std::string response;
	char** env = this->_getEnv();

	if (pipe(inpipefd) == -1 || pipe(outpipefd) == -1) {
		throw std::runtime_error("[CGI] pipe() failed");
	}

	// Configure outpipefd[0] for non-blocking I/O
	fcntl(outpipefd[0], F_SETFL, O_NONBLOCK);

	// Create epoll instance
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		throw std::runtime_error("[CGI] epoll_create1() failed");
	}

	struct epoll_event ev;
	ev.events = EPOLLIN; // Interested in input events
	ev.data.fd = outpipefd[0];

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, outpipefd[0], &ev) == -1) {
		throw std::runtime_error("[CGI] epoll_ctl() failed");
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

		struct epoll_event events[1];
		int nr_events;
		int status;
		bool childExited = false;
		time_t startTime = time(NULL);

		while (!childExited || nr_events > 0) {

			// Check for timeout
			time_t currentTime = time(NULL);
			if (difftime(currentTime, startTime) > 5) {
				kill(pid, SIGKILL);
				_statusCode = 508;
				break;
			}

			// Check if child has exited
			if (waitpid(pid, &status, WNOHANG) > 0) {
				childExited = true;
			}

			// Wait for events on the epoll file descriptor
			nr_events = epoll_wait(epoll_fd, events, 1, 1000); // 1000 ms timeout

			if (nr_events == -1) {
				if (errno != EINTR) {
					perror("epoll_wait");
					break;
				}
			} else if (nr_events > 0) {
				if (events[0].data.fd == outpipefd[0]) {
					int bytesRead = read(outpipefd[0], buffer, BUFFER_SIZE);
					if (bytesRead > 0) {
						response.append(buffer, bytesRead);
					} else if (bytesRead == 0 ||
							   (bytesRead == -1 && errno != EAGAIN)) {
						break; // Pipe closed or error
					}
				}
			}
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
