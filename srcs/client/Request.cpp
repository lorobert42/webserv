
#include "Request.hpp"
#include <exception>
#include <stdexcept>

//	### Static Member Function

bool	isSpace(char c)
{
	if (c == ' ' || c == '\t')
		return (true);
	else
		return (false);
}

// ### Constructor ###
Request::Request() {}

Request::Request(std::string request)
{
	_rawRequest = request;
}

// ### Copy Constructor ###
Request::Request(Request const& other) :
	_header(other._header)
{}

// ### Destructor ###
Request::~Request()
{}

// ### Overload operator ###
Request& Request::operator=(Request const& other)
{
	if (this == &other)
		return (*this);
	this->_header = other._header;
	return (*this);
}

//	### Member Function [PUBLIC] ###

std::string	Request::_getline(std::string& data)
{
	size_t ret = data.find("\r\n");
	if (ret == std::string::npos)
		throw std::runtime_error("No end of line");
	std::string line = data.substr(0, ret);
	data = data.substr(ret + 2);
	return (line);
}

// ### Header parser
void	Request::parseHeader()
{
	// Get all the Request 
	// TODO: if method is not implemented, send 501 Not Implemented, if not allowed, send 405
	// TODO: if uri is in absolute form, replace Host with the host of the uri
	// TODO: Host field must be present -> 400
	std::string	read = _getline(_rawRequest);
	if (_parseFirstLine(read) == false)
		return;

	char		c = ':';
	std::string	key;
	std::string	value;

	// TODO: keys and values are case-insensitive
	// TODO: no whitespace allowed between key and : -> 400
	while (true)
	{
		try {
			read = _getline(_rawRequest);
		}
		catch (const std::exception& e)
		{
			_error = 400;
			return;
		}
		std::cout << read << std::endl;
		if (read == "")
			break;
		read.erase(remove_if(read.begin(), read.end(), ::isspace), read.end());
		read.find(c) != std::string::npos ? key.assign(read, 0, read.find(c)) : \
			key = "none";
		read.find(c) != std::string::npos ? value.assign(read, read.find(c) + 1, read.size() - 1) : \
			value = "none";
		_header[key] = value;
	}
	_body = _rawRequest;
	//	### Printing ###
	std::cout << "Method : [" << _method << "]" << std::endl \
			  << "Uri : [" << _uri  << "]" << std::endl \
			  << "Version : [" << _version << "]" << std::endl \
			  << "HEADER : " << std::endl;
	for (std::map<std::string, std::string>::iterator it = _header.begin(); \
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
	return (true);
}

int	Request::checkBody()
{
	std::cout << "Expected Content-Length: " << getValue("Content-Length") << std::endl;
	std::cout << "Body length: " << getBody().length() << std::endl;
	std::cout << "Body until now: " << getBody() << std::endl;
	std::string	content_length = getValue("Content-Length");
	std::string transfer_encoding = getValue("Transfer-Encoding");
	if ((content_length == "" && transfer_encoding == "") || (content_length != "" && transfer_encoding != ""))
	{
		setBody("");
		_error = 400;
		return (ERROR);
	}
	else if (transfer_encoding != "" && transfer_encoding != "chunked")
	{
		setBody("");
		_error = 501;
		return (ERROR);
	}
	if (content_length != "")
	{
		return (_checkBodyContentLength(std::strtol(content_length.c_str(), NULL, 10)));
	}
	return (_checkBodyChunked());
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
		// TODO: decode chunked body
		return (OK);
	}
	return (TOO_SHORT);
}

void	Request::appendBody(std::string const& to_add)
{
	_body += to_add;
}

void	Request::setBody(std::string const& new_body)
{
	_body = new_body;
}

//	### GETTER ###
const std::string	&Request::getMethod(void) const
{
	return (_method);
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

	find = _header.find(key);
	if (find != _header.end())
		return (find->second);
	else
		return ("");
}

const std::string &Request::getBody() const
{
	return (_body);
}
