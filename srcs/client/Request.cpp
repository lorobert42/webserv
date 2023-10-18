
#include "Request.hpp"

// ### Constructor ###
Request::Request() {}

Request::Request(std::string request)
{
	std::cout << "Welcome to the Request Class" << std::endl;
	std::cout << request << std::endl;
	std::stringstream	ss(request);
	ss >> _method;
	ss >> _index;
	ss >> _version;

	char		c = ':';
	std::string	key;
	std::string	value;
	std::string	read;

	while (std::getline(ss, read))
	{
		if (read.size() != 0)
		{
			read.erase(remove_if(read.begin(), read.end(), ::isspace), read.end());
			read.find(c) < read.size() ? key.assign(read, 0, read.find(c)) : \
				key.assign(read, 0, read.size());
			read.find(c) < read.size() ? value.assign(read, read.find(c) + 1, read.size() - 1) : \
				value = "wrong";
			_map[key] = value;
		}
	}
	std::cout << "Method : [" << _method << "]" << std::endl \
			  << "Index : [" << _index  << "]" << std::endl \
			  << "Version : [" << _version << "]" << std::endl \
			  << "MAP : " << std::endl;
	for (std::map<std::string, std::string>::iterator it = _map.begin(); \
			it != _map.end(); it++)
	{
		std::cout << "[KEY] : " << it->first << std::endl \
			      << "[VALUE] : " << it->second << std::endl;
	}
}

// ### Copy Constructor ###
Request::Request(Request const& other) :
	_map(other._map)
{}

// ### Destructor ###
Request::~Request()
{}

// ### Overload operator ###
Request& Request::operator=(Request const& other)
{
	if (this == &other)
		return (*this);
	this->_map = other._map;
	return (*this);
}

//	### Member Function [PUBLIC] ###

