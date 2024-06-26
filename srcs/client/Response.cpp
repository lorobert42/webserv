/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 12:57:48 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 17:45:34 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(ConfigServer* config, Request* request) :
	_config_server(config),
	_config_hostname(NULL),
	_route(NULL),
	_request(request),
	_header(""),
	_body(""),
	_path(""),
	_should_close(false)
{}

Response::Response(Response const& other)
{
	*this = other;
}

Response& Response::operator=(Response const& other)
{
	if (this != &other)
	{
		_config_server = other._config_server;
		_config_hostname = other._config_hostname;
		_route = other._route;
		_request = other._request;
		_header = other._header;
		_body = other._body;
		_path = other._path;
		_uri_segments = other._uri_segments;
		_should_close = other._should_close;
	}
	return (*this);
}

Response::~Response()
{}

void	Response::createResponse()
{
	if (!_checkRequestOK())
		return ;
	_config_hostname = _config_server->getHostnameWithName(_request->getHostname());


	_createRoute();
	if (!_checkRouteExists())
		return ;
	if (!_checkMethodAllowed())
		return ;
	if (_checkRedirection())
		return ;
	if (_checkCgi())
		return ;
	_path = _createPathFromUri(_request->getUri());
	if (!_checkPath())
		return ;
	if (!_createBody())
	{
		_createHeader(500);
		return ;
	}
	_createHeader(200);
	if (_request->getValue("Connection") == "close")
		_should_close = true;
}

ConfigServer*	Response::getConfigServer()
{
	return (_config_server);
}

ConfigRoute*	Response::getConfigRoute()
{
	return (_route);
}

Request* Response::getRequest()
{
	return (_request);
}

std::string const&	Response::getHeader() const
{
	return (_header);
}

std::string const&	Response::getBody() const
{
	return (_body);
}

bool	Response::shouldClose() const
{
	return (_should_close);
}

void	Response::_createRoute()
{
	_route = _config_hostname->getRouteWithUri(_request->getUri());
	if (_route == NULL)
	{
		_parseRoute(_request->getUri());
		for (std::vector<std::string>::iterator it = _uri_segments.begin(); it != _uri_segments.end(); it++)
		{
			std::cout << "🔵Segment: " << *it << std::endl;
			_route = _config_hostname->getRouteWithUri(*it);
			if (_route != NULL)
				break ;
		}
		if (_route == NULL)
            _route = _config_hostname->getRouteWithUri("/");
		if (_route && _route->getAutoindex() == false)
			_route = NULL;
	}
}

void	Response::_parseRoute(std::string const& uri)
{
	std::stringstream ss(uri);
	std::string segment;
	std::string tempUri = "";

	_uri_segments.clear();  // Clearing the existing segments to avoid any unexpected behavior

	while (std::getline(ss, segment, '/'))
	{
		if (!segment.empty()) {
			tempUri += "/" + segment;
			_uri_segments.insert(_uri_segments.begin(), tempUri);
		}
	}
}

bool	Response::_checkRequestOK()
{
	int error = _request->getError();
	if (error == 0)
		return (true);
	std::cout << "There was an error while parsing the request: " << error << std::endl;
	_createErrorResponse(error);
	return (false);
}

bool	Response::_checkRouteExists()
{
	if (_route != NULL)
		return (true);
	std::cout << "Route not found\n";
	_createErrorResponse(404);
	return (false);
}

bool	Response::_checkMethodAllowed()
{
	if (ClientHelper::isMethodAllowed(_route, _request->getMethod()))
		return (true);
	_createErrorResponse(405);
	return (false);
}

bool	Response::_checkRedirection()
{
	if (_route->getRedirect() != "")
	{
		_createHeader(302);
		return (true);
	}
	return (false);
}

bool	Response::_checkCgi()
{
	if (_route->getCgiScript() == "" || _route->getCgiBin() == "")
		return (false);
	CgiHandler cgi(this);

	// Get extension of cgi script
	std::string extension = _route->getCgiScript().substr(_route->getCgiScript().find_last_of(".") + 1);

	if (extension == "py") {
		_body = cgi.executeCgi();
		if (cgi.getStatusCode() != 200 || cgi.getStatusCode() != 201)
			_should_close = true;
	}
	else if (extension == "php") {
		_body = cgi.executeCgi();
		_header = "HTTP/1.1 ";
		switch (cgi.getStatusCode()) {
			case 200:
				_header.append("200 OK\r\n");
				break;
			case 201:
				_header.append("201 Created\r\n");
				break;
			case 508:
				_should_close = true;
				_createErrorResponse(508);
				break;
			default:
				_should_close = true;
				_header.append("500 Internal Server Error\r\n");
				break;
		}
	}
	return (true);
}

std::string Response::_createPathFromUri(std::string const& uri)
{
	std::string calculatedPath = _route->getPath();

	if (_route->getAutoindex() == true) {
		if (uri != _route->getUri()) {
			if (_route->getUri() == "/")
				calculatedPath.append(uri.substr(_route->getUri().length()));
			else
				calculatedPath.append(uri.substr(_route->getUri().length() + 1));
			calculatedPath.append("/");
		}
		// Remove last slash
		if (calculatedPath[calculatedPath.length() - 1] == '/')
			calculatedPath.erase(calculatedPath.length() - 1);
	}
	return calculatedPath;
}

bool	Response::_checkPath()
{
	AutoIndex autoindex(_request->getUri(), _path);

	// If autoindex is true and file exists -> 200
	if (_route->getAutoindex() && autoindex.isExistingFile())
		return (true);

	int err_dir = _checkDir();
	int err_file = err_dir == E_SUCCESS ? _checkFile() : E_FAIL;

	std::cout << "err_dir: " << err_dir << std::endl;
	std::cout << "err_file: " << err_file << std::endl;

	// If folder and file exist -> 200
	if (err_dir == E_SUCCESS && err_file == E_SUCCESS)
		return (true);
	// If folder exists, but file doesn't or not readable -> 403
	else if (err_dir == E_SUCCESS && (err_file == E_FAIL || err_file == E_ACCESS)) {

		// If autoindex is true, display folder content
		if (_route->getAutoindex()) {
			_body = autoindex.getBody();
			_createHeader(200);
			_should_close = true;
			return (false);
		}
		_createErrorResponse(403);
	}
	// If folder exists, but it's not readable -> 403
	else if (err_dir == E_ACCESS)
		_createErrorResponse(403);
	// If folder doesn't exist -> 404
	else
		_createErrorResponse(404);
	return (false);
}

int	Response::_checkDir()
{
	struct stat s;
	DIR *fd;

	std::cout << "checkdir:" << _path.c_str() << std::endl;
	if (stat(_path.c_str(), &s) == -1)
		return (E_FAIL);
	if (s.st_mode & S_IFDIR) // check the path for a existing dir
	{
		if ((fd = opendir(_path.c_str())) == NULL) // check if we can open the dir
			return (E_ACCESS);
		closedir(fd);
		if (_path[_path.length() - 1] != '/')
			_path.append("/");
		_path.append(_route->getIndex()); // if both is ok will append the the index to the path
		return (E_SUCCESS);
	}
	return (E_FAIL);
}

int	Response::_checkFile()
{
	struct stat s;

	std::cout << "checkfile:" << _path.c_str() << std::endl;
	if (stat(_path.c_str(), &s) == -1)
		return (E_FAIL);
	if (s.st_mode & S_IFREG) // check the path for a existing file
	{
		if (access(_path.c_str(), R_OK) == 0) // check if we can read the file
			return (E_SUCCESS);
		return (E_ACCESS);
	}
	return (E_FAIL);
}

bool	Response::_createBody()
{
	try 
	{
		_body = readFile(_path);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
		_body = _get500Body();
		_should_close = true;
		return (false);
	}
	return (true);
}

void	Response::_createHeader(int status)
{
	_header = "HTTP/1.1 ";
	switch (status)
	{
		case 200:
			_header.append("200 OK\r\n");
			break;
		case 302:
			_header.append("302 Found\r\n");
			_header.append("Location: ");
			_header.append(_route->getRedirect());
			_header.append("\r\n");
			break;
		case 400:
			_header.append("400 Bad Request\r\n");
			break;
		case 403:
			_header.append("403 Forbidden\r\n");
			break;
		case 404:
			_header.append("404 Not Found\r\n");
			break;
		case 405:
			_header.append("405 Method Not Allowed\r\n");
			break;
		case 411:
			_header.append("411 Length Required\r\n");
			break;
		case 413:
			_header.append("413 Content Too Large\r\n");
			break;
		case 501:
			_header.append("501 Not Implemented\r\n");
			break;
		case 505:
			_header.append("505 HTTP Version Not Supported\r\n");
			break;
		case 508:
			_header.append("508 Loop Detected\r\n");
			break;
		default:
			_header.append("500 Internal Server Error\r\n");
			break;
	}
	_header.append("Content-Type: text/html\r\nContent-Length: ");
	std::ostringstream body_len;
	body_len << _body.length();
	_header.append(body_len.str());
	_header.append("\r\n\r\n");
}

void	Response::_createErrorResponse(int status)
{
	_path = _config_server->getErrorPageByCode(status);
	if (!_createBody())
		status = 500;
	_createHeader(status);
	_should_close = true;
}

std::string	Response::_get500Body()
{
	return ("<html>\n\t<head>\n\t\t<title>500 Internal Server Error</title>\n\t</head>\n\t<body>\n\t\t<center><h1>500 Internal Server Error</h1></center>\n\t\t<hr>\n\t\t<center>webserv</center>\n\t</body>\n</html>");
}

Response::Response() :
	_config_server(NULL),
	_config_hostname(NULL),
	_route(NULL),
	_request(NULL),
	_header(""),
	_body(""),
	_path(""),
	_should_close(false)
{}
