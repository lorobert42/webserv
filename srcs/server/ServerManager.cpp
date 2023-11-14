/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:29:33 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/14 16:03:43 by mjulliat         ###   ########.fr       */
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
ServerManager::ServerManager(ServerManager const& other)
{
	*this = other;
}

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
	_epfd = other._epfd;
	_servers = other._servers;
	_clients = other._clients;
	_timeout = other._timeout;
	return (*this);
}

// ### Initialization ###
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
		//std::cout << (*it)->getPort() << std::endl;
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

// ### Main loop ###
void ServerManager::_run()
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
			break;
		}
		if (nfds == 0)
		{
			std::map<int, time_t>::iterator it = _timeout.begin();
			while (it != _timeout.end())
			{
				t_timeout	timeout;
				if (_isTimeoutOK(&timeout, it->first) == -1)
					continue ;
				if (difftime(timeout.now, it->second) > timeout.server->getTimeout())
				{
					std::cout << "closing client [" << it->first << "] due to a timeout." << std::endl;
					std::map<int, time_t>::iterator tmp = it;
					it++;
					_closeClient(tmp->first);
				}
				else
					it++;
			}
		}
		for (int n = 0; n < nfds; ++n)
		{
			std::map<int, time_t>::iterator it = _timeout.find(events[n].data.fd);
			if (it != _timeout.end())
			{
				time_t	now;
				time(&now);
				it->second = now;
			}

			if (_isServerSocket(events[n].data.fd))
			{
				int socket;
				std::cout << "[Received new connection]" << std::endl;
				socket = _newClient(events[n].data.fd); // create client
				time_t	start;
				time(&start);
				_timeout.insert(std::pair<int, time_t>(socket, start));
			}
			else if (events[n].events & EPOLLIN)
			{
				if (!_handleRead(events[n].data.fd))
				{
					g_should_stop = true;
					break;
				}
			}
			else if (events[n].events & EPOLLOUT)
			{
				if (!_handleWrite(events[n].data.fd))
				{
					g_should_stop = true;
					break;
				}
			}
			else
				std::cerr << "Unexpected event" << std::endl;
		}
	}
	std::cout << "Closing remaining clients" << std::endl;
	_closeAllClients();
	std::cout << "Clients closed\n";
}

// ### I/O management ###
// Returns false only in case of fatal error
bool	ServerManager::_handleRead(int fd)
{
	Client*	client = _getClientBySocket(fd);
	if (!client)
		return (true);
	std::cout << "[Receiving data on fd:] " << client->getSocket() << std::endl;
	int	handler_response = client->readHandler();
	if (handler_response == 0)
	{
		if (!_epollCtlMod(_epfd, fd, EPOLLOUT))
			return (false);
	}
	else if (handler_response == -1)
	{
		std::cout << "Client disconnected: " << fd << std::endl;
		_closeClient(fd);
	}
	return (true);
}

// Returns false only in case of fatal error
bool	ServerManager::_handleWrite(int fd)
{
	Client*	client = _getClientBySocket(fd);
	if (!client)
		return (true);
	std::cout << "[Sending data on fd:] " << client->getSocket() << std::endl;
	int res = client->writeHandler();
	if (res == 0)
	{
		if (!_epollCtlMod(_epfd, fd, EPOLLIN))
			return (false);
		client->_clear();
		return (true);
	}
	else if (res == -1)
	{
		std::cout << "Server close connection on [" << fd << "]" << std::endl;
		if (!_epollCtlDel(_epfd, fd))
			return (false);
		_closeClient(fd);
	}
	return (true);
}

// ### Client management ###
int	ServerManager::_newClient(int server_socket)
{
	int client_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	Client	*new_client = NULL;

	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
	if (client_socket == -1)
	{
		std::cerr << "Cannot accept client connection: " << strerror(errno) << std::endl;
		return -1;
	}
	Server	*server = _getServerBySocket(server_socket);
	if (!server)
	{
		close(client_socket);
		return -1;
	}
	if (!_epollCtlAdd(_epfd, client_socket, EPOLLIN))
	{
		close(client_socket);
		return -1;
	}
	new_client = new Client(server->getConfig(), client_socket);
	_clients[client_socket] = new_client;
	return (client_socket);
}

Client* ServerManager::_getClientBySocket(int socket)
{
	// Find client instance linked to a socket file descriptor
	std::map<int, Client*>::iterator search = _clients.find(socket);
	if (search == _clients.end())
	{
		std::cerr << "Client " << socket << " does not exist" << std::endl;
		return NULL;
	}
	return (search->second);
}

void	ServerManager::_closeClient(int socket)
{
	Client*	client = _getClientBySocket(socket);
	if (!client)
		return;
	delete client;
	close(socket);
	_clients.erase(socket);
	_timeout.erase(socket);
}

void	ServerManager::_closeAllClients()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		delete it->second;
		close(it->first);
	}
}

// ### Servers management ###
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
		std::cerr << "Server " << socket << " does not exist" << std::endl;
		return NULL;
	}
	return (search->second);
}

int	ServerManager::_isTimeoutOK(t_timeout *timeout, int socket)
{
	Client	*client = _getClientBySocket(socket);
	if (client == NULL)
		return (-1);
	ConfigServer *server= client->getConfigServer();
	if (server == NULL)
		return (-1);
	timeout->server = server;
	time(&timeout->now);
	return (0);
}

// ### Epoll management ###
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
