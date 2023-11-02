/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 22:21:36 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/25 10:35:40 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// ### Accessors ###
ConfigServer* Server::getConfig() const
{
	return (_config);
}

std::string Server::getName() const
{
	return (_name);
}

int Server::getPort() const
{
	return (_port);
}

int Server::getSocket() const
{
	return (_socket);
}

struct sockaddr_in Server::getAddr() const
{
	return (_addr);
}

// ### Constructor ###
Server::Server()
{}

Server::Server(ConfigServer* config) :
	_config(config)
{
	_name = config->getName();
	_port = config->getPort();
	_socket = -1;
	// Fill the _addr structure with host and port
	std::cout << "1) Configure the listen address: [" << _port <<  "]" << std::endl;
	std::memset((char *)&_addr, 0, sizeof(struct sockaddr_in));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);
}

// ### Copy Constructor ###
Server::Server(Server const &other) :
	_config(other._config),
	_name(other._name),
	_port(other._port),
	_socket(other._socket),
	_addr(other._addr)
{}

// ### Destructor ###
Server::~Server()
{}

// ### Overload operator ###
Server &Server::operator=(Server const &other)
{
	if (this == &other)
		return (*this);
	_config = other._config;
	_name = other._name;
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
	if (_socket == -1) {
		std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
		return (false);
	}
	// Set the port to be reuse after use
	int option_value = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) < 0) {
		std::cerr << "Socket reuse port error: " << strerror(errno) << std::endl;
		return (false);
	}
	// Bind the socket to IP address and port defined in _addr
	std::cout << "- Binding socket to address." << std::endl;
	if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1) {
		std::cerr << "Socket binding error: " << strerror(errno) << std::endl;
		return (false);
	}
	// Listen to incoming connections
	std::cout << "- Make the socket listen." << std::endl;
	if (listen(_socket, SOMAXCONN) == -1) {
		std::cerr << "Socket listen error: " << strerror(errno) << std::endl;
		return (false);
	}
	return (true);
}
