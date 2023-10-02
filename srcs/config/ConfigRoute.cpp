#include "ConfigRoute.hpp"

const std::string	DEFAULT_ROUTE	= "server_name";
const std::string	DEFAULT_PATH	= "/www/default";
const std::string	DEFAULT_INDEX	= "index.html";

ConfigRoute::ConfigRoute():
	_route(DEFAULT_ROUTE),
	_path(DEFAULT_PATH),
	_index(DEFAULT_INDEX) {}

ConfigRoute::~ConfigRoute() {}

ConfigRoute::ConfigRoute(ConfigRoute const &src) {
	*this = src;
}

ConfigRoute	&ConfigRoute::operator=(ConfigRoute const &rhs) {
	// TODO
	(void)rhs;
	return (*this);
}

std::string ConfigRoute::getRoute() const {
	return this->_route;
}

std::string ConfigRoute::getPath() const {
	return this->_path;
}

std::ostream &operator<<(std::ostream &o, ConfigRoute const &rhs) {
	o << "Route: " << rhs.getRoute() << std::endl;
	o << "Path: " << rhs.getPath() << std::endl;
	return (o);
}