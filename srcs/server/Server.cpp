/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:29:33 by lorobert          #+#    #+#             */
/*   Updated: 2023/09/28 13:27:55 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// ### Constructor ###
Server::Server()
{}

Server::Server(unsigned int host, int port)
{
	_host = host;
	_port = port;
	_socket = -1;
	// Fill the _addr structure with host and port
	std::cout << "1) Configure the listen address: [port]" << std::endl;
	std::memset((char *)&_addr, 0, sizeof(struct sockaddr_in));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(_host);
	_addr.sin_port = htons(_port);
}

// ### Copy Constructor ###
Server::Server(Server const& other) :
	_host(other._host),
	_port(other._port),
	_socket(other._socket),
	_addr(other._addr)
{}

// ### Destructor ###
Server::~Server()
{}

// ### Overload operator ###
Server& Server::operator=(Server const& other)
{
	if (this == &other)
		return (*this);
	_host = other._host;
	_port = other._port;
	_socket = other._socket;
	_addr = other._addr;
	return (*this);
}

// ### Member Function ###
bool Server::setup()
{
	// Create socket in non-blocking mode for an IP address
	std::cout << "2) Creating server socket:" << std::endl;
	_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_socket == -1)
	{
		std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
		return (false);
	}
	// Set the port to be reuse after use
	int	option_value = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) < 0)
	{
		std::cerr << "Socket reuse port error: " << strerror(errno) << std::endl;
		return (false);
	}
	// Bind the socket to IP address and port defined in _addr
	std::cout << "- Binding socket to address." << std::endl;
	if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		std::cerr << "Socket binding error: " << strerror(errno) << std::endl;
		return (false);
	}
	// Listen to incoming connections
	std::cout << "- Make the socket listen." << std::endl;
	if (listen(_socket, SOMAXCONN) == -1)
	{
		std::cerr << "Socket listen error: " << strerror(errno) << std::endl;
		return (false);
	}
	return (true);
}

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

bool Server::run()
{
	std::cout << "3) Create the epoll structure:" << std::endl;
	int	epfd = epoll_create(1);
	if (epfd == -1)
	{
		std::cerr << "Epoll creation error: " << strerror(errno) << std::endl;
		return (false);
	}
	std::cout << "- Add socket to epoll" << std::endl;
	if (!epollCtlAdd(epfd, _socket, EPOLLIN))
		return (false);

	int nfds;
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct epoll_event events[D_MAX_EVENTS];

	std::cout << "- Listening to connections:" << std::endl;
	while (true)
	{
		nfds = epoll_wait(epfd, events, D_MAX_EVENTS, -1);
		if (nfds == -1)
		{
			std::cerr << "Epoll wait error: " << strerror(errno) << std::endl;
			return (false);
		}

		for (int n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == _socket)
			{
				std::cout << "[Received new connection]" << std::endl;
				client_socket = accept(_socket, (struct sockaddr *)&client_addr, &client_len);
				if (client_socket == -1)
				{
					std::cerr << "Cannot accept client connection: " << strerror(errno) << std::endl;
					return (false);
				}
				if (!epollCtlAdd(epfd, client_socket, EPOLLIN | EPOLLET))
					return (false);
			}
			else if (events[n].events &EPOLLIN)
			{
				std::cout << "[Receiving data on fd:] " << events[n].data.fd << std::endl;
				if (readHandler(events[n].data.fd) == 0)
				{
					if (!epollCtlMod(epfd, events[n].data.fd, EPOLLOUT | EPOLLET))
						return (false);
				}
			}
			else if (events[n].events &EPOLLOUT)
			{
				std::cout << "[Sending data on fd:] " << events[n].data.fd << std::endl;
				writeHandler(events[n].data.fd);
				int fd = events[n].data.fd;
				epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
			else
				std::cerr << "Unexpected event" << std::endl;
		}
	}
}

int Server::readHandler(int fd)
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
	if (_requests[fd].rfind("\n\n") != std::string::npos || _requests[fd].rfind("\r\n\r\n") != std::string::npos)
	{
		std::cout << "End of request" << std::endl;
		return (0);
	}
	return (1);
}

int Server::writeHandler(int fd)
{
	std::cout << "Write handler" << std::endl;
	std::string	server_message = "HTTP/1.1 200 OK\nContent-type: text/html\nContent-legth";
	std::string response = "<html><h1>Hello, World!</h1></html>";
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
