/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjulliat <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:29:33 by mjulliat          #+#    #+#             */
/*   Updated: 2023/10/26 10:49:14 by mjulliat         ###   ########.fr       */
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

ConfigServer	*Client::getConfigServer(void) const
{
	return (this->_config_server);
}

ConfigRoute		*Client::getConfigRoute(void) const
{
	return (this->_route);
}

Request	*Client::getRequest(void) const
{
	return (this->_request);
}

int	Client::readHandler(void)
{
	std::cout << "Read Handler" << std::endl;
	char buffer[D_BUFF_SIZE] = {0};

	int read = recv(_socket, buffer, D_BUFF_SIZE, MSG_DONTWAIT);
	if (read < 0)
		throw std::runtime_error("[READ] : Error");
	else if (read == 0)
	{
		std::cout << "read error -1" << std::endl;
		return (-1);
	}
	if (read != 0)
		_read.append(buffer);
	if (_read.rfind("\r\n\r\n") != std::string::npos)
	{
		_request = new Request(_read);
		std::cout << "End of request" << std::endl;
		return (0);
	}
	return (1);
}

//static std::string	readHtmlFile(std::string path_with_index)
//{
//	std::cout << "Path with index: " << path_with_index << std::endl;
//	std::string		line;
//	std::string		all;
//	std::ifstream	ifs(path_with_index.c_str());
//
//	if (!ifs)
//	{
//		std::cout << "file cannot be read" << std::endl;
//		return (NULL);
//	}
//	while (std::getline(ifs, line))
//		all.append(line);
//	return (all);
//}

int	Client::writeHandler(void)
{
	std::cout << "Write handler" << std::endl;
	// Need to change server_message by our own
	std::string	server_message;
	_uri = _request->getIndex();
	_route = _config_server->getRouteWithUri("/");
/*	if (_route == NULL)
	{
		std::cout << "Route not found" << std::endl;
		return (_requestNotFound());
	}
	if (_checkFile() == true)
		server_message = _requestFound();
	else
	{
		std::cout << "File not found" << std::endl;
		return (_requestNotFound());
	}*/

	server_message = "HTTP/1.1 200 OK\ncontent-type: text/html\ncontent-length:";
	// Have to get the set the reponse by the html we need
//	std::string body = readHtmlFile(_route->getPathWithIndex());
//	AutoIndex autoindex(_uri, _route->getPath() + _uri);
//	std::string body = autoindex.getBody();


	// TODO
	CgiHandler cgi(this);
	std::string body = cgi.executeCgi();

	server_message.append("\n\n");
	server_message.append(body);
	
	int	bytes_send = 0;
	int	total_bytes_send = 0;
	while (total_bytes_send <static_cast<int>(server_message.size()))
	{
		bytes_send = send(_socket, server_message.c_str(), server_message.size(), 0);
		if (bytes_send < 0)
			std::cout << "Could not send reposne" << std::endl;
		total_bytes_send += bytes_send;
	}

	return (0);
}

//	### Member Function [PRIVATE]

bool	Client::_checkFile(void)
{
	struct	stat s;

	if (stat(_uri.c_str(), &s))
	{
		std::cout << _uri << std::endl;
		if (s.st_mode & S_IFDIR) // it's a directory
		{
			_uri.append(_route->getIndex());
			if (access(_uri.c_str(), R_OK))
				return (true);
			else
				return (false);
		}
		if (s.st_mode & S_IFREG) // it's a file
		{
			if (access(_uri.c_str(), R_OK))
				return (true);
			else
				return (false);
		}
		else
			return (false);
	}
	else
	{
		std::cout << "here" << std::endl;
		return (false);
	}
}

std::string	Client::_requestFound(void)
{
	std::cout << "Request Found" << std::endl;
	return (NULL);
}

int	Client::_requestNotFound(void)
{
	std::cout << "Request not Found" << std::endl;
	return (0);
}
