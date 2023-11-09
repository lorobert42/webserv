/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:29:33 by mjulliat          #+#    #+#             */
/*   Updated: 2023/11/09 13:53:40 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"

// ### Constructor ###
Client::Client()
{}

Client::Client(ConfigServer *config, int &client_socket) :
	_config_server(config),
	_socket(client_socket),
	_nb_read(0),
	_total_bytes_send(0),
	_headerOk(false),
	_responseOK(false),
	_error(false)
{
	_request = new Request(config->getClientMaxBodySize());
	_response = new Response(config, _request);
}

// ### Copy Constructor ###
Client::Client(Client const& other) :
	_config_server(other._config_server),
	_socket(other._socket), 
	_nb_read(other._nb_read),
	_total_bytes_send(other._total_bytes_send),
	_headerOk(other._headerOk),
	_responseOK(other._responseOK),
	_error(other._error)
{}

// ### Destructor ###
Client::~Client()
{
	delete _request;
	delete _response;
}

// ### Overload operator ###
Client& Client::operator=(Client const& other)
{
	if (this == &other)
		return (*this);
	this->_config_server = other._config_server;
	this->_socket = other._socket;
	_headerOk = other._headerOk;
	_nb_read = other._nb_read;
	_error = other._error;
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

Request	*Client::getRequest(void) const
{
	return (this->_request);
}

int	Client::readHandler(void)
{
	std::cout << "Read Handler" << std::endl;
	char buffer[D_BUFF_SIZE] = {0};
	std::cout << "nb_read: " << _nb_read << std::endl;

	int read = recv(_socket, buffer, D_BUFF_SIZE, MSG_DONTWAIT);
	if (read < 0)
		return (ERROR);
	else if (read == 0)
	{
		std::cout << "Nothing to read, client quit" << std::endl;
		return (ERROR);
	}
	if (read != 0)
	{
		_request->appendRawRequest(buffer, read);
		_nb_read++;
	}
	if (!_headerOk)
	{
		int res = _request->parseHeader();
		if (res == TOO_SHORT)
			return (TOO_SHORT);
		else if (res == ERROR)
			return (OK);
		_headerOk = true;
		if (_request->getMethod() != "POST")
			return (OK);
	}
	if (_headerOk && _request->getMethod() == "POST")
	{
		std::cout << "Should check body" << std::endl;
		if (_request->checkBody() == TOO_SHORT)
			return (TOO_SHORT);
		return (OK);
	}
	return (TOO_SHORT);
}

int	Client::writeHandler(void)
{
	std::cout << "Write handler" << std::endl;

	if (_responseOK == false)
	{
		_response->createResponse();
		_server_message = _response->getHeader() + _response->getBody();
	}
	_responseOK = true;

	int	bytes_send = 0;
	std::string	buffer;
	buffer.assign(_server_message, _total_bytes_send, _total_bytes_send + D_BUFF_SIZE);
	bytes_send = send(_socket, buffer.c_str(), buffer.size(), 0); 
	if (bytes_send < 0)
		return (-1);
	_total_bytes_send += bytes_send;
	if (_total_bytes_send < static_cast<int>(_server_message.size()))
		return (1);
	if (_response->shouldClose())
		return (-1);
	return (0);
}

//	### Member Function [PRIVATE]
void	Client::_clear()
{
	_request->clear();
	_read = "";
	_nb_read = 0;
	_total_bytes_send = 0;
	_headerOk = false;
	_responseOK = false;
	_CGI_on = false;
	_error = false;
	_server_message = "";
}
