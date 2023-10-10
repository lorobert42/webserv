/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:29:33 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/10 16:54:37 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"
#include <vector>

static bool	epollCtlAdd(int epfd, int fd, uint32_t events)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		std::cerr << "Unable to add socket to epoll: " << strerror(errno) << std::endl;
		return (false);
	}
	return (true);
}

static bool	epollCtlMod(int epfd, int fd, uint32_t events)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
	{
		std::cerr << "Unable to modify epoll: " << strerror(errno) << std::endl;
		return (false);
	}
	return (true);
}

/*
static int  epollCtlDel(int epfd, int fd)
{
  if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
  {
    std::cerr << "Unable to delete client socket: " << strerror(errno) << std::endl;
    return (-1);
  }
  return (0);
}*/

// ### Constructor ###
ServerManager::ServerManager()
{}

ServerManager::ServerManager(Config config) :
	_config(config)
{}

// ### Copy Constructor ###
ServerManager::ServerManager(ServerManager const& other) :
	_config(other._config)
{}

// ### Destructor ###
ServerManager::~ServerManager()
{}

// ### Overload operator ###
ServerManager& ServerManager::operator=(ServerManager const& other)
{
	if (this == &other)
		return (*this);
	_config = other._config;
	return (*this);
}

// ### Member Function ###
void ServerManager::setup()
{
	std::vector<ConfigServer*> servers = _config.getServers();
	for (std::vector<ConfigServer*>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		Server new_serv(**it);
		if (new_serv.setup() == false)
		{
			std::cerr << "Unable to setup server: " << new_serv.getName() << std::endl;
			continue;
		}
		_servers.push_back(new_serv);
	}
}

bool ServerManager::_isServerSocket(int socket)
{
	for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if ((*it).getSocket() == socket)
		{
			return (true);
		}
	}
	return (false);
}

void ServerManager::_acceptClient(int server_socket)
{
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
	if (client_socket == -1)
	{
		std::cerr << "Cannot accept client connection: " << strerror(errno) << std::endl;
		return;
	}
	if (!epollCtlAdd(_epfd, client_socket, EPOLLIN | EPOLLET))
		return;
}

bool ServerManager::run()
{
	std::cout << "3) Create the epoll structure:" << std::endl;
	_epfd = epoll_create(1);
	if (_epfd == -1)
	{
		std::cerr << "Epoll creation error: " << strerror(errno) << std::endl;
		return (false);
	}
	std::cout << "- Add socket to epoll" << std::endl;
	for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (!epollCtlAdd(_epfd, (*it).getSocket(), EPOLLIN))
		{
			std::cerr << "Unable to add server to epoll: " << (*it).getName() << std::endl;
			continue;
		}
	}

	int nfds;
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct epoll_event events[D_MAX_EVENTS];

	std::cout << "- Listening to connections:" << std::endl;
	while (true)
	{
		nfds = epoll_wait(_epfd, events, D_MAX_EVENTS, -1);
		if (nfds == -1)
		{
			std::cerr << "Epoll wait error: " << strerror(errno) << std::endl;
			return (false);
		}

		for (int n = 0; n < nfds; ++n)
		{
			if (_isServerSocket(events[n].data.fd))
			{
				std::cout << "[Received new connection]" << std::endl;
				_acceptClient(events[n].data.fd);
			}
			else if (events[n].events & EPOLLIN)
			{
				std::cout << "[Receiving data on fd:] " << events[n].data.fd << std::endl;
				if (readHandler(events[n].data.fd) == 0)
				{
					if (!epollCtlMod(_epfd, events[n].data.fd, EPOLLOUT | EPOLLET))
						return (false);
				}
			}
			else if (events[n].events & EPOLLOUT)
			{
				std::cout << "[Sending data on fd:] " << events[n].data.fd << std::endl;
				writeHandler(events[n].data.fd);
				int fd = events[n].data.fd;
				epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
			else
				std::cerr << "Unexpected event" << std::endl;
		}
	}
}

int ServerManager::readHandler(int fd)
{
	char buffer[D_BUFFER_SIZE] = {0};

	int read = recv(fd, buffer, D_BUFFER_SIZE, MSG_DONTWAIT);
	if (read <= 0)
	{
		close(fd);
		_requests[fd].erase();
		if (read == 0)
			std::cout << "Connection closed by client" << std::endl;
		else
			std::cerr << "Read error" << std::endl;
		return (-1);
	}
	_requests[fd] += std::string(buffer);
	std::cout << "Request until now:\n" << _requests[fd] << std::endl;
	// Every line finish by a \r\n and the header and body are separated
	// by a \r\n so the end of the header must be a \r\n for the last line
	// and another \r\n so the separation of the header to the body
	// must be \r\n\r\n
	if (_requests[fd].rfind("\n\n") != std::string::npos || _requests[fd].rfind("\r\n\r\n") != std::string::npos)
	{
		std::cout << "End of request" << std::endl;
		return (0);
	}
	return (1);
}

static std::string	readHtmlFile(void)
{
	std::string		line;
	std::string		all;
	std::ifstream	ifs("www/example/index.html");

	if (!ifs)
	{
		std::cout << "file cannot be read" << std::endl;
		return (NULL);
	}
	while (std::getline(ifs, line))
		all.append(line);
	return (all);
}

int ServerManager::writeHandler(int fd)
{
	std::cout << "Write handler" << std::endl;
	// Need to change server_message by our own
	std::string	server_message = "HTTP/1.1 200 OK\nContent-type: text/html\nContent-legth";
	// Have to get the set the reponse by the html we need
	std::string response = readHtmlFile();
	server_message.append("\n\n");
	server_message.append(response);
	
	int	bytes_send = 0;
	int	total_bytes_send = 0;
	while (total_bytes_send < static_cast<int>(server_message.size()))
	{
		bytes_send = send(fd, server_message.c_str(), server_message.size(), 0);
		if (bytes_send < 0)
			std::cout << "Could not send reposne" << std::endl;
		total_bytes_send += bytes_send;
	}
	return (0);
}
