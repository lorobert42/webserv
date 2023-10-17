
#include "Request.hpp"

// ### Constructor ###
Request::Request() {}

Request::Request(std::string read)
{
	std::cout << "Welcome to the Request Class" << std::endl;
	std::cout << read << std::endl;	
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

