/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjulliat <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:29:33 by mjulliat          #+#    #+#             */
/*   Updated: 2023/10/18 15:09:47 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// ### Constructor ###
Client::Client()
{
	// TODO: Better error management
	throw std::runtime_error("[Client] : Wait, that's illegal");
}

Client::Client(ConfigServer *config, int &client_socket) :
	_config_server(config), _socket(client_socket)
{}

// ### Copy Constructor ###
Client::Client(Client const& other) :
	_config_server(other._config_server), _socket(other._socket)
{}

// ### Destructor ###
Client::~Client()
{}

// ### Overload operator ###
Client& Client::operator=(Client const& other)
{
	if (this == &other)
		return (*this);
	this->_config_server = other._config_server;
	this->_socket = other._socket;
	return (*this);
}

//	### Member Function [PUBLIC] ###

int	Client::getSocket(void) const
{
	return (this->_socket);	
}

std::string	Client::getRequest(void) const
{
	return (this->_read);
}

int	Client::readHandler(void)
{
	std::cout << "Read Handler" << std::endl;
	char buffer[4096] = {0};

	int read = recv(_socket, buffer, 4096, MSG_DONTWAIT);
	if (read < 0)
		throw std::runtime_error("[READ] : Error");
	else if (read == 0)
	{
		return (-1);
	}
	_read += std::string(buffer);
	if (_read.rfind("\r\n\r\n") != std::string::npos)
	{
		_request = new Request(_read);
		std::cout << "End of request" << std::endl;
		return (0);
	}
	return (1);
}

static std::string	readHtmlFile(void)
{
	std::string		line;
	std::string		all;
	std::ifstream	ifs("www/srcs/index.html");

	if (!ifs)
	{
		std::cout << "file cannot be read" << std::endl;
		return (NULL);
	}
	while (std::getline(ifs, line))
		all.append(line);
	return (all);
}

int	Client::writeHandler(void)
{
	std::cout << "Write handler" << std::endl;
	// Need to change server_message by our own
	std::string	server_message = "HTTP/1.1 200 OK\nContent-type: text/html\nContent-length";
	// Have to get the set the reponse by the html we need
	std::string response = readHtmlFile();
	server_message.append("\n\n");
	server_message.append(response);
	
	int	bytes_send = 0;
	int	total_bytes_send = 0;
	while (total_bytes_send <static_cast<int>(server_message.size()))
	{
		bytes_send = send(_socket, server_message.c_str(), server_message.size(), 0);
		if (bytes_send < 0)
			std::cout << "Could not send reposne" << std::endl;
		total_bytes_send += bytes_send;
	}

	// FIXME
	CgiHandler cgi(this);
	cgi.displayEnv();

	return (0);
}

//	### Member Function [PRIVATE]
