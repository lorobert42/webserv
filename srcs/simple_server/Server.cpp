/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:29:33 by lorobert          #+#    #+#             */
/*   Updated: 2023/09/21 16:39:04 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int Server::setup()
{
	// Create socket in non-blocking mode for an IP address
	std::cout << "Creating server socket" << std::endl;
	_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_socket == -1)
	{
		std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
		return (-1);
	}
	// Bind the socket to IP address and port defined in _addr
	std::cout << "Binding socket to address" << std::endl;
	if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		std::cerr << "Socket binding error: " << strerror(errno) << std::endl;
		return (-1);
	}
	// Listen to incoming connections
	std::cout << "Make the socket listen" << std::endl;
	if (listen(_socket, SOMAXCONN) == -1)
	{
		std::cerr << "Socket listen error: " << strerror(errno) << std::endl;
		return (-1);
	}
	return (0);
}

static int	epoll_ctl_add(int epfd, int fd, uint32_t events)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		std::cerr << "Unable to add socket to epoll: " << strerror(errno) << std::endl;
		return (-1);
	}
	return (0);
}

static int	epoll_ctl_mod(int epfd, int fd, uint32_t events)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
	{
		std::cerr << "Unable to modify epoll: " << strerror(errno) << std::endl;
		return (-1);
	}
	return (0);
}

static int  epoll_ctl_del(int epfd, int fd)
{
  if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
  {
    std::cerr << "Unable to delete client socket: " << strerror(errno) << std::endl;
    return (-1);
  }
  return (0);
}

int Server::run()
{
	std::cout << "Create the epoll structure" << std::endl;
	int	epfd = epoll_create(1);
	if (epfd == -1)
	{
		std::cerr << "Epoll creation error: " << strerror(errno) << std::endl;
		return (-1);
	}
	std::cout << "Add socket to epoll" << std::endl;
	if (epoll_ctl_add(epfd, _socket, EPOLLIN) == -1)
	{
		return (-1);
	}

	int nfds;
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct epoll_event events[MAX_EVENTS];
	int n;

	std::cout << "Listening to connections" << std::endl;
	for (;;)
	{
		nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds == -1)
		{
			std::cerr << "Epoll wait error: " << strerror(errno) << std::endl;
			return (-1);
		}

		for (n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == _socket)
			{
				std::cout << "Received new connection" << std::endl;
				client_socket = accept(_socket, (struct sockaddr *)&client_addr, &client_len);
				if (client_socket == -1)
				{
					std::cerr << "Cannot accept client connection: " << strerror(errno) << std::endl;
					return (-1);
				}
				if (epoll_ctl_add(epfd, client_socket, EPOLLIN | EPOLLET) == -1)
				{
					return (-1);
				}
			}
			else if (events[n].events & EPOLLIN)
			{
				std::cout << "Receiving data on fd: " << events[n].data.fd << std::endl;
				if (read_handler(events[n].data.fd) == 0)
				{
					if (epoll_ctl_mod(epfd, events[n].data.fd, EPOLLOUT | EPOLLET) == -1)
					{
						return (-1);
					}
				}
			}
			else if (events[n].events & EPOLLOUT)
			{
				std::cout << "Sending data on fd: " << events[n].data.fd << std::endl;
				write_handler(events[n].data.fd);
				int fd = events[n].data.fd;
				epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
			else
			{
				std::cerr << "Unexpected event" << std::endl;
			}
		}
	}
}

int Server::read_handler(int fd)
{
	char buffer[BUFFER_SIZE] = {0};

	int read = recv(fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (read <= 0)
	{
		close(fd);
		_requests[fd].erase();
		if (read == 0)
		{
			std::cout << "Connection closed by client" << std::endl;
		}
		else
		{
			std::cerr << "Read error" << std::endl;
		}
		return (-1);
	}
	_requests[fd] += std::string(buffer);
	std::cout << "Request until now:\n" << _requests[fd] << std::endl;
	if (_requests[fd].rfind("\n\n") != std::string::npos)
	{
		std::cout << "End of request" << std::endl;
		return (0);
	}
	return (1);
}

int Server::write_handler(int fd)
{
	std::cout << "Write handler" << std::endl;
	send(fd, "Hello\n", 7, 0);
	return (0);
}

Server::Server()
{}

Server::Server(unsigned int host, int port)
{
	_host = host;
	_port = port;
	_socket = -1;
	// Fill the _addr structure with host and port
	std::cout << "Configure the listen address:port" << std::endl;
	std::memset((char *)&_addr, 0, sizeof(struct sockaddr_in));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(_host);
	_addr.sin_port = htons(_port);
}

Server::Server(Server const& other) :
	_host(other._host),
	_port(other._port),
	_socket(other._socket),
	_addr(other._addr)
{}

Server::~Server()
{}

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
