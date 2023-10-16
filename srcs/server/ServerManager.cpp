/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:29:33 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/11 16:57:49 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

extern bool g_should_stop;

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
{
	for (std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		delete it->second;
	}
}

// ### Overload operator ###
ServerManager& ServerManager::operator=(ServerManager const& other)
{
	if (this == &other)
		return (*this);
	_config = other._config;
	return (*this);
}

// ### Member Function ###
void	ServerManager::start()
{
	_setupServers();
	if (_setupEpoll() == false)
	{
		return;
	}
	_run();
}

void ServerManager::_setupServers()
{
	// Setup all servers and add them to the manager
	std::vector<ConfigServer*> servers = _config->getServers();
	Server* new_serv = NULL;
	for (std::vector<ConfigServer*>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << (*it)->getPort() << std::endl;
		new_serv = new Server(*it);
		if (new_serv->setup() == false)
		{
			std::cerr << "Unable to setup server: " << new_serv->getName() << std::endl;
			delete new_serv;
			continue;
		}
		_servers[new_serv->getSocket()] = new_serv;
	}
}

bool ServerManager::_setupEpoll()
{
	std::cout << "3) Create the epoll structure:" << std::endl;
	_epfd = epoll_create(1);
	if (_epfd == -1)
	{
		std::cerr << "Epoll creation error: " << strerror(errno) << std::endl;
		return (false);
	}
	std::cout << "- Add socket to epoll" << std::endl;
	for (std::map<int, Server*>::const_iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (!_epollCtlAdd(_epfd, it->second->getSocket(), EPOLLIN))
		{
			std::cerr << "Unable to add server to epoll: " << it->second->getName() << std::endl;
			continue;
		}
	}
	return true;
}

bool ServerManager::_run()
{
	int nfds;
	struct epoll_event events[D_MAX_EVENTS];

	std::cout << "- Listening to connections:" << std::endl;
	while (g_should_stop == false)
	{
		nfds = epoll_wait(_epfd, events, D_MAX_EVENTS, D_TIMEOUT);
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
				_newClient(events[n].data.fd); // create client
			}
			else if (events[n].events & EPOLLIN)
			{
				Client	client(_getClientBySocket(events[n].data.fd));
				std::cout << "[Receiving data on fd:] " << client.getSocket() << std::endl;
				if (client.readHandler() == 0)
				{
					if (!_epollCtlMod(_epfd, events[n].data.fd, EPOLLOUT | EPOLLET))
						return (false);
				}
			}
			else if (events[n].events & EPOLLOUT)
			{
				Client	client(_getClientBySocket(events[n].data.fd));
				std::cout << "[Sending data on fd:] " << client.getSocket() << std::endl;
				client.writeHandler();
				int fd = client.getSocket();
				epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
				close(fd);
			}
			else
				std::cerr << "Unexpected event" << std::endl;
		}
	}
	return (true);
}

void ServerManager::_newClient(int server_socket)
{
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	Client	*new_client = NULL;

	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
	if (client_socket == -1)
	{
		std::cerr << "Cannot accept client connection: " << strerror(errno) << std::endl;
		return;
	}
	if (!_epollCtlAdd(_epfd, client_socket, EPOLLIN | EPOLLET))
		return;
	new_client = new Client(_getServerBySocket(server_socket), client_socket);
	_clients[client_socket] = new_client;
}

bool ServerManager::_isServerSocket(int socket) const
{
	// Check if socket file descriptor belongs to a server
	std::map<int, Server*>::const_iterator search = _servers.find(socket);
	return (search != _servers.end());
}

Server* ServerManager::_getServerBySocket(int socket)
{
	// Find server instance linked to a socket file descriptor
	std::map<int, Server*>::iterator search = _servers.find(socket);
	if (search == _servers.end())
	{
		// TODO: Better error management
		throw std::runtime_error("Server does not exist");
	}
	return (search->second);
}

Client ServerManager::_getClientBySocket(int socket)
{
	// Find client instance linked to a socket file descriptor
	std::map<int, Client*>::iterator search = _clients.find(socket);
	if (search == _clients.end())
	{
		// TODO: Better error management
		throw std::runtime_error("Client does not exist");
	}
	return (*search->second);
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
