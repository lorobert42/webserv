/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:06 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:09 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

//	### Static Member Function
bool	isSpace(char c)
{
	if (c == ' ' || c == '\t')
		return (true);
	else
		return (false);
}

// ### Constructor ###
Request::Request() :
	_rawRequest(""),
	_method(""),
	_uri(""),
	_version(""),
	_body(""),
	_error(0),
	_client_max_body_size(1 * 1024 * 1024)
{}

Request::Request(double client_max_body_size) :
	_rawRequest(""),
	_method(""),
	_uri(""),
	_version(""),
	_body(""),
	_error(0),
	_client_max_body_size(client_max_body_size)
{}

// ### Copy Constructor ###
Request::Request(Request const& other)
{
	*this = other;
}

// ### Destructor ###
Request::~Request()
{}

// ### Overload operator ###
Request& Request::operator=(Request const& other)
{
	if (this == &other)
		return (*this);
	_rawRequest = other._rawRequest;
	_header = other._header;
	_method = other._method;
	_uri = other._uri;
	_version = other._version;
	_body = other._body;
	_error = other._error;
	_client_max_body_size = other._client_max_body_size;
	return (*this);
}

//	### Member Function [PUBLIC] ###
// ### Header parser
int	Request::parseHeader()
{
	// Get all the Request 
	if (_rawRequest.rfind("\r\n\r\n") == std::string::npos)
		return (TOO_SHORT);
	std::string	read = _getline(_rawRequest);
	if (_parseFirstLine(read) == false)
		return (ERROR);

	if (_parseFields() == false)
		return (ERROR);

	_body = _rawRequest;
	_rawRequest = "";

	_printRequest();
	return (OK);
}

bool	Request::_parseFirstLine(std::string& line)
{
	size_t space = line.find(" ");
	if (space == std::string::npos)
	{
		_error = 400;
		return (false);
	}
	_method = line.substr(0, space);
	line = line.substr(space + 1);
	space = line.find(" ");
	if (space == std::string::npos)
	{
		_error = 400;
		return (false);
	}
	_uri = line.substr(0, space);
	_version = line.substr(space + 1, line.size());
	if (_method == "" || _uri == "" || _version == "" ||
			_method.find(" ") != std::string::npos ||
			_uri.find(" ") != std::string::npos ||
			_version.find(" ") != std::string::npos)
	{
		_error = 400;
		return (false);
	}
	if (_method == "CONNECT" || _method == "HEAD" || _method == "OPTIONS" ||
		_method == "PATCH" || _method == "PUT" || _method == "TRACE")
	{
		_error = 501;
		return (false);
	}
	if (_version.compare("HTTP/1.1") != 0)
	{
		_error = 505;
		return (false);
	}
	return (true);
}

bool	Request::_parseFields()
{
	char		c = ':';
	std::string	key;
	std::string	value;
	std::string line;

	while (true)
	{
		try
		{
			line = _getline(_rawRequest);
		}
		catch (const std::exception& e)
		{
			_error = 400;
			return (false);
		}
		if (line == "")
			break;
		line.find(c) != std::string::npos ? key.assign(line, 0, line.find(c)) : key = "none";
		if (key.find(" ") != std::string::npos)
		{
			_error = 400;
			return (false);
		}
		strtolower(key);
		line.find(c) != std::string::npos ? value.assign(line, line.find(c) + 1, line.size() - 1) : value = "none";
		value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
		strtolower(value);
		_header[key] = value;
	}
	if (getValue("Host") == "")
	{
		_error = 400;
		return (false);
	}
	return (true);
}

int	Request::checkBody()
{
	if (_rawRequest.length() != 0)
	{
		_body += _rawRequest;
		_rawRequest = "";
	}
	std::string	content_length = getValue("Content-Length");
	std::string transfer_encoding = getValue("Transfer-Encoding");
	if (content_length != "" && transfer_encoding != "")
	{
		setBody("");
		_error = 400;
		return (ERROR);
	}
	else if (content_length != "")
	{
		std::cout << "Expected Content-Length: " << getValue("Content-Length") << std::endl;
		std::cout << "Body length: " << getBody().length() << std::endl;
		if (content_length.find_first_not_of("0123456789") != std::string::npos)
		{
			setBody("");
			_error = 400;
			return (ERROR);
		}
		size_t expectedContentLength = std::strtol(content_length.c_str(), NULL, 10);
		if (expectedContentLength > _client_max_body_size)
		{
			std::cout << "Client max body size reached" << std::endl;
			setBody("");
			_error = 413;
			if (_body.length() >= expectedContentLength)
				return (ERROR);
			return (TOO_SHORT);
		}
		return (_checkBodyContentLength(expectedContentLength));
	}
	else if (transfer_encoding != "" && transfer_encoding.substr(transfer_encoding.length() - 7) != "chunked")
	{
		setBody("");
		_error = 400;
		return (ERROR);
	}
	else if (transfer_encoding != "")
	{
		if (_body.length() == 0)
			return (TOO_SHORT);
		return (_checkBodyChunked());
	}
	else if (content_length == "" && _body.length() != 0)
	{
		setBody("");
		_error = 411;
		return (ERROR);
	}
	_body = "";
	_header["content-length"] = "0";
	return (OK);
}

int	Request::_checkBodyContentLength(size_t content_length)
{
	if (_body.length() == content_length)
	{
		return (OK);
	}
	else if (_body.length() < content_length)
	{
		return (TOO_SHORT);
	}
	return (ERROR);
}

int	Request::_checkBodyChunked()
{
	if (_body.rfind("0\r\n\r\n") != std::string::npos)
	{
		std::string line;
		std::string new_body = "";
		size_t size = 0;
		while (true)
		{
			try
			{
				line = _getline(_body);
			}
			catch (const std::exception& e)
			{
				_error = 400;
				return (ERROR);
			}
			std::cout << "Should get chunk size: [" << line << "]" << std::endl;
			if (line.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos)
			{
				_error = 400;
				return (ERROR);
			}
			size = std::strtoul(line.c_str(), NULL, 16);
			if (size == 0)
				break;
			else if (size + 2 > _body.length())
			{
				_error = 400;
				return (ERROR);
			}
			std::cout << "Chunk size: " << size << std::endl;
			std::cout << "Should insert: [" << _body.substr(0, size) << "]" << std::endl;
			new_body += _body.substr(0, size);
			_body = _body.substr(size + 2);
			size = 0;
		}
		return (OK);
	}
	return (TOO_SHORT);
}

std::string	Request::_getline(std::string& data)
{
	size_t ret = data.find("\r\n");
	if (ret == std::string::npos)
		throw std::runtime_error("No end of line");
	std::string line = data.substr(0, ret);
	data = data.substr(ret + 2);
	return (line);
}

void	Request::clear()
{
	_rawRequest = "";
	_header.clear();
	_method = "";
	_uri = "";
	_version = "";
	_body = "";
	_error = 0;
}

void	Request::_printRequest() const
{
	//	### Printing ###
	std::cout << "Method : [" << _method << "]" << std::endl \
		<< "Uri : [" << _uri  << "]" << std::endl \
		<< "Version : [" << _version << "]" << std::endl \
		<< "HEADER : " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); \
			it != _header.end(); it++)
	{
		std::cout << "[KEY] : " << it->first << std::endl \
			<< "[VALUE] : " << it->second << std::endl;
	}
	if (_body.size() != 0)
		std::cout << "BODY :\n" << _body << std::endl;
	else
		std::cout << "BODY : [EMPTY]" << std::endl;
}

void Request::appendRawRequest(char* to_add, int size)
{
	_rawRequest.insert(_rawRequest.length(), to_add, size);
}

void	Request::appendBody(char* to_add, int size)
{
	_body.insert(_body.length(), to_add, size);
}

void	Request::setBody(std::string const& new_body)
{
	_body = new_body;
}

//	### GETTER ###
const std::string &Request::getRawRequest() const
{
	return (_rawRequest);
}

const std::string	&Request::getMethod(void) const
{
	return (_method);
}

std::string	Request::getHostname(void) const
{
	std::string	hostnameWithPort = Request::getValue("Host");
	std::string	hostname = hostnameWithPort.substr(0, hostnameWithPort.find_first_of(":"));
	return (hostname);
}

const std::string	&Request::getUri(void) const
{
	return (_uri);
}

const std::string	&Request::getVersion(void) const
{
	return (_version);
}

const std::string	Request::getValue(const std::string &key) const
{
	std::map<std::string,std::string>::const_iterator	find;
	std::string	to_find = key;

	strtolower(to_find);
	find = _header.find(to_find);
	if (find != _header.end())
		return (find->second);
	else
		return ("");
}

const std::string &Request::getBody() const
{
	return (_body);
}

int	Request::getError() const
{
	return (_error);
}
