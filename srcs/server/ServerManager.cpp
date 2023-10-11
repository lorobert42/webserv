/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:29:33 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/11 15:56:22 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"

// ### Constructor ###
ServerManager::ServerManager()
{}

ServerManager::ServerManager(Config* config) :
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
	// Setup all servers and add them to the manager
	std::vector<ConfigServer*> servers = _config->getServers();
	for (std::vector<ConfigServer*>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << (*it)->getPort() << std::endl;
		Server new_serv(*it);
		if (new_serv.setup() == false)
		{
			std::cerr << "Unable to setup server: " << new_serv.getName() << std::endl;
			continue;
		}
		_servers[new_serv.getSocket()] = new_serv;
	}
}


bool ServerManager::_isServerSocket(int socket) const
{
	// Check if socket file descriptor belongs to a server
	std::map<int, Server>::const_iterator search = _servers.find(socket);
	return (search != _servers.end());
}

Server& ServerManager::_getServerBySocket(int socket)
{
	// Find server instance linked to a socket file descriptor
	std::map<int, Server>::iterator search = _servers.find(socket);
	if (search == _servers.end())
	{
		// TODO: Better error management
		throw std::runtime_error("Server does not exist");
	}
	return (search->second);
}

void ServerManager::_newClient(int server_socket)
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
	if (!_epollCtlAdd(_epfd, client_socket, EPOLLIN | EPOLLET))
		return;
	//_clients[client_socket] = Client(_getServerBySocket(server_socket), client_socket);
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
	for (std::map<int, Server>::const_iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (!_epollCtlAdd(_epfd, it->second.getSocket(), EPOLLIN))
		{
			std::cerr << "Unable to add server to epoll: " << it->second.getName() << std::endl;
			continue;
		}
	}

	int nfds;
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
				_newClient(events[n].data.fd);
			}
			else if (events[n].events & EPOLLIN)
			{
				std::cout << "[Receiving data on fd:] " << events[n].data.fd << std::endl;
				if (readHandler(events[n].data.fd) == 0)
				{
					if (!_epollCtlMod(_epfd, events[n].data.fd, EPOLLOUT | EPOLLET))
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

bool ServerManager::_epollCtlAdd(int epfd, int fd, uint32_t events)
{
	// Add scoket file descriptor to the epoll structure, watching events
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

bool ServerManager::_epollCtlMod(int epfd, int fd, uint32_t events)
{
	// Modify the events watched by epoll for fd
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

bool ServerManager::_epollCtlDel(int epfd, int fd)
{
	// Remove a socket fd from epoll
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
	{
		std::cerr << "Unable to delete client socket: " << strerror(errno) << std::endl;
		return (false);
	}
	return (true);
}

int ServerManager::readHandler(int fd)
{
	(void)fd;
	return (0);
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
