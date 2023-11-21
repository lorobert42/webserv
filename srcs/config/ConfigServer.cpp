/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:57 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:58 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"

const std::string   DEFAULT_SERVER_NAME             = "webserv";
const int           DEFAULT_PORT                    = 8080;
const double		DEFAULT_CLIENT_MAX_BODY_SIZE	= 1 * 1024 * 1024;
const int			DEFAULT_TIMEOUT					= 60;
const std::string   DEFAULT_ERROR_PAGE_400          = "srcs/config/www/400.html";
const std::string   DEFAULT_ERROR_PAGE_403          = "srcs/config/www/403.html";
const std::string   DEFAULT_ERROR_PAGE_404          = "srcs/config/www/404.html";
const std::string   DEFAULT_ERROR_PAGE_405          = "srcs/config/www/405.html";
const std::string   DEFAULT_ERROR_PAGE_411          = "srcs/config/www/411.html";
const std::string   DEFAULT_ERROR_PAGE_413          = "srcs/config/www/413.html";
const std::string   DEFAULT_ERROR_PAGE_500          = "srcs/config/www/500.html";
const std::string   DEFAULT_ERROR_PAGE_501          = "srcs/config/www/501.html";
const std::string   DEFAULT_ERROR_PAGE_505          = "srcs/config/www/505.html";
const std::string   DEFAULT_ERROR_PAGE_508			= "srcs/config/www/508.html";

ConfigServer::ConfigServer():
	_name(DEFAULT_SERVER_NAME),
	_port(DEFAULT_PORT),
	_client_max_body_size(DEFAULT_CLIENT_MAX_BODY_SIZE),
	_timeout(DEFAULT_TIMEOUT),
	_error_page_400(DEFAULT_ERROR_PAGE_400),
	_error_page_403(DEFAULT_ERROR_PAGE_403),
	_error_page_404(DEFAULT_ERROR_PAGE_404),
	_error_page_405(DEFAULT_ERROR_PAGE_405),
	_error_page_411(DEFAULT_ERROR_PAGE_411),
	_error_page_413(DEFAULT_ERROR_PAGE_413),
	_error_page_500(DEFAULT_ERROR_PAGE_500),
	_error_page_501(DEFAULT_ERROR_PAGE_501),
	_error_page_505(DEFAULT_ERROR_PAGE_505),
	_error_page_508(DEFAULT_ERROR_PAGE_508)
{
	this->_hostnames.push_back(new ConfigHostname());
}

ConfigServer::ConfigServer(const std::string &serverConfig) {

	try {
		// Set default values
		this->_name = DEFAULT_SERVER_NAME;
		this->_port = DEFAULT_PORT;
		this->_client_max_body_size = DEFAULT_CLIENT_MAX_BODY_SIZE;
		this->_timeout = DEFAULT_TIMEOUT;
		this->_error_page_400 = DEFAULT_ERROR_PAGE_400;
		this->_error_page_403 = DEFAULT_ERROR_PAGE_403;
		this->_error_page_404 = DEFAULT_ERROR_PAGE_404;
		this->_error_page_405 = DEFAULT_ERROR_PAGE_405;
		this->_error_page_411 = DEFAULT_ERROR_PAGE_411;
		this->_error_page_413 = DEFAULT_ERROR_PAGE_413;
		this->_error_page_500 = DEFAULT_ERROR_PAGE_500;
		this->_error_page_501 = DEFAULT_ERROR_PAGE_501;
		this->_error_page_505 = DEFAULT_ERROR_PAGE_505;
		this->_error_page_508 = DEFAULT_ERROR_PAGE_508;

		// Parse server config
		std::string line;
		std::istringstream serverConfigStream(serverConfig);

		while (std::getline(serverConfigStream, line)) {
			if (line.empty())
				continue;

			// Check if line contains strictly "hostname:"
			if (line == "hostname:") {
				std::string hostnameConfig;
				while (std::getline(serverConfigStream, line) && (line[0] == '\t' || line.empty())) {
					// Remove tabulation
					line.erase(0, 1);
					hostnameConfig += line + "\n";
				}
				if (line != ";") {
					throw InvalidCloseDirectiveException(line);
				}
				if (hostnameConfig.empty()) {
					this->_hostnames.push_back(new ConfigHostname());
				} else {
					this->_hostnames.push_back(new ConfigHostname(hostnameConfig));
				}
			}

			std::string option = line.substr(0, line.find_first_of("="));
			std::string value = line.substr(line.find_first_of("=") + 1);

			// Set server options
			if (option == "name")
				this->_name = value;
			else if (option == "port")
				this->_port = ConfigHelper::convertStringToPort(value);
			else if (option == "client_max_body_size")
				this->_client_max_body_size = ConfigHelper::convertStringToClientMaxBodySize(value);
			else if (option == "timeout")
				this->_timeout = ConfigHelper::convertStringToInt(value);
			else if (option == "error_page_400")
				this->_error_page_400 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_400, value);
			else if (option == "error_page_403")
				this->_error_page_403 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_403, value);
			else if (option == "error_page_404")
				this->_error_page_404 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_404, value);
			else if (option == "error_page_405")
				this->_error_page_405 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_405, value);
			else if (option == "error_page_411")
				this->_error_page_411 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_411, value);
			else if (option == "error_page_413")
				this->_error_page_413 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_413, value);
			else if (option == "error_page_500")
				this->_error_page_500 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_500, value);
			else if (option == "error_page_501")
				this->_error_page_501 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_501, value);
			else if (option == "error_page_505")
				this->_error_page_505 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_505, value);
			else if (option == "error_page_508")
				this->_error_page_508 = ConfigHelper::getValidErrorPage(DEFAULT_ERROR_PAGE_508, value);
			else if (option == ";")
				continue;
			else
				throw InvalidOptionKeyException(option);
		}
	} catch (std::exception &e) {
		if (this->_hostnames.size() > 0) {
			for (std::vector<ConfigHostname*>::iterator it = this->_hostnames.begin(); it != this->_hostnames.end(); ++it) {
				delete (*it);
			}
		}
		throw;
	}
}

ConfigServer::~ConfigServer() {
	if (this->_hostnames.size() > 0) {
		for (std::vector<ConfigHostname*>::iterator it = this->_hostnames.begin(); it != this->_hostnames.end(); ++it) {
			delete (*it);
		}
	}
}

ConfigServer::ConfigServer(ConfigServer const &src) {
	*this = src;
}

ConfigServer	&ConfigServer::operator=(ConfigServer const &rhs) {
	if (this != &rhs) {
		this->_name = rhs._name;
		this->_port = rhs._port;
		this->_client_max_body_size = rhs._client_max_body_size;
		this->_timeout = rhs._timeout;
		this->_error_page_400 = rhs._error_page_400;
		this->_error_page_403 = rhs._error_page_403;
		this->_error_page_404 = rhs._error_page_404;
		this->_error_page_405 = rhs._error_page_405;
		this->_error_page_411 = rhs._error_page_411;
		this->_error_page_413 = rhs._error_page_413;
		this->_error_page_500 = rhs._error_page_500;
		this->_error_page_501 = rhs._error_page_501;
		this->_error_page_505 = rhs._error_page_505;
		this->_error_page_508 = rhs._error_page_508;
		this->_hostnames = rhs._hostnames;
	}
	return (*this);
}

std::string	ConfigServer::getName() const {
	return this->_name;
}

int	ConfigServer::getPort() const {
	return this->_port;
}

std::string	ConfigServer::getPortAsString() const {
	std::stringstream ss;
	ss << this->_port;
	return ss.str();
}

double	ConfigServer::getClientMaxBodySize() const {
	return this->_client_max_body_size;
}

int	ConfigServer::getTimeout() const {
	return this->_timeout;
}

std::string	ConfigServer::getErrorPage400() const {
	return this->_error_page_400;
}

std::string	ConfigServer::getErrorPage403() const {
	return this->_error_page_403;
}

std::string	ConfigServer::getErrorPage404() const {
	return this->_error_page_404;
}

std::string	ConfigServer::getErrorPage405() const {
	return this->_error_page_405;
}

std::string	ConfigServer::getErrorPage411() const {
	return this->_error_page_411;
}

std::string	ConfigServer::getErrorPage413() const {
	return this->_error_page_413;
}

std::string	ConfigServer::getErrorPage500() const {
	return this->_error_page_500;
}

std::string	ConfigServer::getErrorPage501() const {
	return this->_error_page_501;
}

std::string	ConfigServer::getErrorPage505() const {
	return this->_error_page_505;
}

std::string	ConfigServer::getErrorPage508() const {
	return this->_error_page_508;
}

std::string ConfigServer::getErrorPageByCode(const int &code) const {
	switch (code) {
		case 400:
			return this->_error_page_400;
		case 403:
			return this->_error_page_403;
		case 404:
			return this->_error_page_404;
		case 405:
			return this->_error_page_405;
		case 411:
			return this->_error_page_411;
		case 413:
			return this->_error_page_413;
		case 500:
			return this->_error_page_500;
		case 501:
			return this->_error_page_501;
		case 505:
			return this->_error_page_505;
		case 508:
			return this->_error_page_508;
		default:
			return this->_error_page_500;
	}
}

std::vector<ConfigHostname*>	ConfigServer::getHostnames() const {
	return this->_hostnames;
}

ConfigHostname	*ConfigServer::getHostnameWithName(const std::string &name) const {
	for (std::vector<ConfigHostname*>::const_iterator it = this->_hostnames.begin(); it != this->_hostnames.end(); ++it) {
		if ((*it)->getName() == name) {
			std::cout << "Hostname found: " << (*it)->getName() << std::endl;
			return (*it);
		}
	}
	std::cout << "Hostname not found: " << name << ", using first hostname: " << this->_hostnames[0]->getName() << std::endl;
	// Return the first hostname if no hostname with name is found
	return this->_hostnames[0];
}

std::ostream    &operator<<(std::ostream &o, ConfigServer const &rhs) {
	o << "=== Server ===" << std::endl;
	o << "name: " << rhs.getName() << std::endl;
	o << "port: " << rhs.getPort() << std::endl;
	o << "client_max_body_size: " << rhs.getClientMaxBodySize() << std::endl;
	o << "timeout: " << rhs.getTimeout() << std::endl;
	o << "error_page_400: " << rhs.getErrorPage400() << std::endl;
	o << "error_page_403: " << rhs.getErrorPage403() << std::endl;
	o << "error_page_404: " << rhs.getErrorPage404() << std::endl;
	o << "error_page_405: " << rhs.getErrorPage405() << std::endl;
	o << "error_page_411: " << rhs.getErrorPage411() << std::endl;
	o << "error_page_413: " << rhs.getErrorPage413() << std::endl;
	o << "error_page_500: " << rhs.getErrorPage500() << std::endl;
	o << "error_page_501: " << rhs.getErrorPage501() << std::endl;
	o << "error_page_505: " << rhs.getErrorPage505() << std::endl;
	o << "error_page_508: " << rhs.getErrorPage508() << std::endl;

	std::vector<ConfigHostname*> hostnames = rhs.getHostnames();
	for (std::vector<ConfigHostname*>::iterator it = hostnames.begin(); it != hostnames.end(); ++it) {
		o << *(*it) << std::endl;
	}
	return (o);
}
