/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjulliat <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:29:33 by mjulliat          #+#    #+#             */
/*   Updated: 2023/10/26 12:04:30 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// ### Constructor ###
Client::Client()
{}

Client::Client(ConfigServer *config, int &client_socket) :
	_config_server(config), _socket(client_socket), _headerOk(false)
{}

// ### Copy Constructor ###
Client::Client(Client const& other) :
	_config_server(other._config_server), _socket(other._socket), _headerOk(other._headerOk)
{}

// ### Destructor ###
Client::~Client()
{
	delete _request;
}

// ### Overload operator ###
Client& Client::operator=(Client const& other)
{
	if (this == &other)
		return (*this);
	this->_config_server = other._config_server;
	this->_socket = other._socket;
	_headerOk = other._headerOk;
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

Request*	Client::getRequest(void) const
{
	return (_request);
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
		std::cout << "Nothing to read, client quit" << std::endl;
		return (-1);
	}
	if (read != 0)
		_read.append(buffer);
	if (!_headerOk && _read.rfind("\r\n\r\n") != std::string::npos)
	{
		_request = new Request(_read);
		_request->parseHeader();
		_headerOk = true;
		if (_request->getMethod() != "POST")
			return (0);
	}
	else if (_headerOk && _request->getMethod() == "POST")
	{
		_request->setBody(_request->getBody() + buffer);
		std::cout << "Expected Content-Length: " << _request->getValue("Content-Length") << std::endl;
		std::cout << "Body until now: " << _request->getBody() << std::endl;
		int check = _request->checkBody();
		switch (check)
		{
			case ERROR:
				return (-1);
				break;
			case TOO_SHORT:
				return (1);
				break;
		}
		std::cout << "End of request" << std::endl;
		return (0);
	}
	return (1);
}

static std::string	readHtmlFile(std::string path_with_index)
{
	std::string		line;
	std::string		all;
	std::ifstream	ifs(path_with_index.c_str());

	std::cout << path_with_index << std::endl;
	if (!ifs)
	{
		std::cout << "file cannot be read" << std::endl;
		//TODO better error handling
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
	std::string	server_message;
	_route = _config_server->getRouteWithUri(_request->getIndex());
	_path = _route->getPath();
	if (_route == NULL)
	{
		std::cout << "Route not found" << std::endl;
		return (_requestNotFound());
	}
	if (_checkFile() == true)
		server_message = _requestFound();
	else
	{
		std::cout << "#\nFile not found" << std::endl;
		return (_requestNotFound());
	}

	server_message = "HTTP/1.1 200 OK\ncontent-type: text/html\ncontent-length:";
	// Have to get the set the reponse by the html we need
	std::string response = readHtmlFile(_route->getPathWithIndex());

	// TODO
	//CgiHandler cgi(this);
	//std::string response = cgi.executeCgi();

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

	if (stat(_path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR) // it's a directory
		{
			_path.append(_route->getIndex());
			if (access(_path.c_str(), R_OK))
				return (true);
			else
				return (false);
		}
		if (s.st_mode & S_IFREG) // it's a file
		{
			if (access(_path.c_str(), R_OK))
				return (true);
			else
				return (false);
		}
		else
			return (false);
	}
	else
		return (false);
}

std::string	Client::_requestFound(void)
{
	std::string	header_message = D_200_MESSAGE;
	std::string	content_type;
	std::stringstream ss(_request->getValue("Accept"));


	std::getline(ss,content_type,',');
	header_message.append("\ncontent_type: ");
	header_message.append(content_type);
	header_message.append("\ncontent_length: ");
	return (header_message);
}

int	Client::_requestNotFound(void)
{
	std::cout << "Request not Found" << std::endl;
	return (0);
}
