
#include "Request.hpp"
#include <cstddef>
#include <cstdlib>

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
// ### Header parser
void	Request::parseHeader()
{
	// Get all the Request 
	std::stringstream	ss(_rawRequest);
	ss >> _method;
	ss >> _index;
	ss >> _version;

	char		c = ':';
	std::string	key;
	std::string	value;
	std::string	read;

	std::getline(ss,read); // SKIP the \r\n after dada collected in the first line
	while (std::getline(ss, read))
	{
		if (read.size() != 0)
		{
			if (read.size() == 1 && read == "\r") // STOP reading when the header is done
				break;
			read.erase(remove_if(read.begin(), read.end(), ::isspace), read.end());
			read.find(c) < read.size() ? key.assign(read, 0, read.find(c)) : \
				key = "none";
			read.find(c) < read.size() ? value.assign(read, read.find(c) + 1, read.size() - 1) : \
				value = "none";
			_header[key] = value;
		}
	}
	ss >> _body;
	//	### Printing ###
	std::cout << "Method : [" << _method << "]" << std::endl \
			  << "Index : [" << _index  << "]" << std::endl \
			  << "Version : [" << _version << "]" << std::endl \
			  << "HEADER : " << std::endl;
	for (std::map<std::string, std::string>::iterator it = _header.begin(); \
			it != _header.end(); it++)
	{
		std::cout << "[KEY] : " << it->first << std::endl \
			      << "[VALUE] : " << it->second << std::endl;
	}
	if (_body.size() != 0)
		std::cout << "BODY :" << std::endl << _body << std::endl;
}

size_t	Request::checkBody()
{
	std::string	content_length = getValue("Content-Length");
	if (content_length == "")
		return (ERROR);
	size_t size = std::strtol(getValue("Content-Length").c_str(), NULL, 10);
	if (_body.length() == size)
	{
		return (OK);
	}
	else if (_body.length() < size)
	{
		return (TOO_SHORT);
	}
	return (ERROR);
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

const std::string	&Request::getIndex(void) const
{
	return (_index);
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
