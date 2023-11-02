/*
 * ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:29:33 by mjulliat          #+#    #+#             */
/*   Updated: 2023/11/02 14:32:42 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"

// ### Constructor ###
Client::Client()
{}

Client::Client(ConfigServer *config, int &client_socket) :
	_config_server(config),
	_socket(client_socket),
	_nb_read(0),
	_total_bytes_send(0),
	_headerOk(false),
	_respondOK(false),
	_error(false)
{
	_request = new Request(config->getClientMaxBodySize());
}

// ### Copy Constructor ###
Client::Client(Client const& other) :
	_config_server(other._config_server),
	_socket(other._socket), 
	_nb_read(other._nb_read),
	_total_bytes_send(other._total_bytes_send),
	_headerOk(other._headerOk),
	_respondOK(other._respondOK),
	_error(other._error)
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
	std::cout << "nb_read: " << _nb_read << std::endl;

	int read = recv(_socket, buffer, D_BUFF_SIZE, MSG_DONTWAIT);
	if (read < 0)
		throw std::runtime_error("[READ] : Error");
	else if (read == 0)
	{
		std::cout << "Nothing to read, client quit" << std::endl;
		return (-1);
	}
	if (read != 0)
	{
		_request->appendRawRequest(buffer, read);
		_nb_read++;
	}
	if (!_headerOk && _request->getRawRequest().rfind("\r\n\r\n") != std::string::npos)
	{
		if (_request->parseHeader() == false)
		{
			std::cerr << "Error parsing header" << std::endl;
			return (0);
		}
		_headerOk = true;
		if (_request->getMethod() != "POST")
			return (0);
	}
	if (_headerOk && _request->getMethod() == "POST")
	{
		std::cout << "Should check body" << std::endl;
		if (_nb_read > 1)
		{
			std::cout << "Second time coming, should add buffer to body" << std::endl;
			// std::cout << "buffer to add:\n" << buffer << std::endl;
			_request->appendBody(buffer, read);
		}
		int check = _request->checkBody();
		std::cout << "Check result: " << check << std::endl;
		switch (check)
		{
			case ERROR:
				return (0);
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

static std::string	readFile(std::string path_with_index)
{
	std::string		line;
	std::string		all;
	std::ifstream	ifs(path_with_index.c_str());

	if (!ifs)
	{
		std::cout << "file cannot be read: " << path_with_index << std::endl;
		//TODO better error handling
		return ("");
	}
	while (std::getline(ifs, line))
		all.append(line);
	return (all);
}

int	Client::writeHandler(void)
{
	std::cout << "Write handler" << std::endl;

	if (_respondOK == false)
	{
		_createRespond();
		if (_CGI_on == true && _error == false) {
			_server_message.append(D_200_MESSAGE);
			_server_message.append("\r\n");
		} else {
			_server_message.append(_header);
			std::ostringstream	body_len;
			body_len << _body.length();
			_server_message.append(body_len.str());
			_server_message.append("\r\n\r\n");
		}
		_server_message.append(_body);
	}
	_respondOK = true;

	int	bytes_send = 0;
	std::string	buffer;
	buffer.assign(_server_message, _total_bytes_send, _total_bytes_send + D_BUFF_SIZE);
	bytes_send = send(_socket, buffer.c_str(), buffer.size(), 0); 
	if (bytes_send < 0)
		return (-1);
	_total_bytes_send += bytes_send;
	if (_total_bytes_send < static_cast<int>(_server_message.size()))
		return (1);
	if (_should_close == true)
		return (-1);
	else
	{
		_clear();
		return (0);
	}
}

//	### Member Function [PRIVATE]

void	Client::_createRespond(void)
{
	_should_close = _request->getValue("connection") == "close";
	_total_bytes_send = 0;
	if (_request->getError() != 0)
	{
		_createErrorResponse(_request->getError());
		return ;
	}
	_route = _config_server->getRouteWithUri(_request->getUri());
	if (_route == NULL) {
		// Get the route using uri segments
		_parseRoute(_request->getUri());
		for (std::vector<std::string>::iterator it = _uriSegments.begin(); it != _uriSegments.end(); it++) {
			std::cout << "🔵Segment: " << *it << std::endl;
			_route = _config_server->getRouteWithUri(*it);
			if (_route != NULL)
				break;
		}
	}
	// Check if route has redirection
	if (_route != NULL && _route->getRedirect() != "")
		_route = _config_server->getRouteWithUri(_route->getRedirect());
	if (_route == NULL)
	{
		std::cout << "Route not found" << std::endl;
		_createErrorResponse(404);
		return ;
	}
	// Check if method is allowed
	if (!ClientHelper::isMethodAllowed(_route, _request->getMethod())) {
		_createErrorResponse(405);
		return ;
	}
	// Check if the route has a CGI
	if(_route->getCgiScript() != "" && _route->getCgiBin() != "")
	{
		std::cout << "CGI" << std::endl;
		_CGI_on = true;
		CgiHandler cgi(this);
		_body = cgi.executeCgi();
		return ;
	}
	_path = this->_calculatePathFromUri(_request->getUri());
	int	code_error;
	code_error = _checkFile();
	if (code_error == E_SUCCESS)
		_header = _fileFound();
	else if (code_error == E_ACCESS)
	{
		_createErrorResponse(403);
	}
	else
	{
		_createErrorResponse(404);
	}
	if (_body.size() == 0)
		_body = readFile(_path);
}

int	Client::_checkFile(void)
{
	struct	stat s;
	DIR		*fd;

	if (stat(_path.c_str(), &s) == -1)
		return (E_FAIL);
	if (s.st_mode & S_IFDIR) // check the path for a existing dir
	{
		if ((fd = opendir(_path.c_str())) == NULL) // check if we can open the dir
			return (E_ACCESS);
		else
		{
			closedir(fd);
			_path.append(_route->getIndex()); // if both is ok will append the the index to the path
		}
	}
	else
		return (E_FAIL);
	if (stat(_path.c_str(), &s) == -1)
		return (E_FAIL);
	if (s.st_mode & S_IFREG) // check the path for a existing file
	{
		if (access(_path.c_str(), R_OK) == 0) // check if we can read the file
			return (E_SUCCESS);
		else
			return (E_ACCESS);
	}
	else
		return (E_FAIL);
}

std::string	Client::_fileFound(void)
{
	std::string	header_message = D_200_MESSAGE;
	
	header_message.append("\r\nContent-Type: text/html");
	header_message.append("\r\nContent-Length: ");
	return (header_message);
}

void	Client::_createErrorResponse(int status)
{
	_header = "HTTP/1.1 ";
	switch (status)
	{
		case 400:
			_header.append("400 Bad Request\n");
			break;
		case 403:
			_header.append("403 Forbidden\n");
			break;
		case 404:
			_header.append("404 Not Found\n");
			break;
		case 405:
			_header.append("405 Method Not Allowed\n");
			break;
		case 411:
			_header.append("411 Length Required\n");
			break;
		case 413:
			_header.append("413 Content Too Large\n");
			break;
		case 501:
			_header.append("501 Not Implemented\n");
			break;
		case 505:
			_header.append("505 HTTP Version Not Supported\n");
			break;
		default:
			_header.append("500 Internal Server Error\n");
			break;
	}
	_header.append("Content-Type: text/html\nContent-Length: ");
	_body = readFile(_config_server->getErrorPageByCode(status));
	_error = true;
	_should_close = true;
}

void	Client::_parseRoute(const std::string &uri) {
	std::stringstream ss(uri);
	std::string segment;
	std::string tempUri = "";

	this->_uriSegments.clear();  // Clearing the existing segments to avoid any unexpected behavior

	while (std::getline(ss, segment, '/')) {
		if (!segment.empty()) {
			tempUri += "/" + segment;
			this->_uriSegments.insert(this->_uriSegments.begin(), tempUri);
		}
	}
}

std::string Client::_calculatePathFromUri(const std::string &uri) {
	std::string calculatedPath = _route->getPath();
	if (uri != _route->getUri()) {
		std::string uriWithoutRoute = uri.substr(_route->getUri().length() + 1);
		calculatedPath.append(uriWithoutRoute);
		calculatedPath.append("/");
	}

	return calculatedPath;
}

void	Client::_clear()
{
	_request->clear();
	_read = "";
	_nb_read = 0;
	_total_bytes_send = 0;
	_headerOk = false;
	_respondOK = false;
	_CGI_on = false;
	_should_close = false;
	_error = false;
	_path = "";
	_header = "";
	_body = "";
	_server_message = "";
	_uriSegments.clear();
}
