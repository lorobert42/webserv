#include "ConfigServer.hpp"

const std::string   DEFAULT_SERVER_NAME             = "webserv";
const std::string   DEFAULT_HOST                    = "localhost";
const int           DEFAULT_PORT                    = 8080;
const std::string   DEFAULT_ERROR_PAGE_400          = "www/error_pages/400.html";
const std::string   DEFAULT_ERROR_PAGE_403          = "www/error_pages/403.html";
const std::string   DEFAULT_ERROR_PAGE_404          = "www/error_pages/404.html";
const std::string   DEFAULT_ERROR_PAGE_405          = "www/error_pages/405.html";
const std::string   DEFAULT_ERROR_PAGE_413          = "www/error_pages/413.html";
const std::string   DEFAULT_ERROR_PAGE_500          = "www/error_pages/500.html";

ConfigServer::ConfigServer():
	_name(DEFAULT_SERVER_NAME),
	_host(DEFAULT_HOST),
	_port(DEFAULT_PORT),
	_error_page_400(DEFAULT_ERROR_PAGE_400),
	_error_page_403(DEFAULT_ERROR_PAGE_403),
	_error_page_404(DEFAULT_ERROR_PAGE_404),
	_error_page_405(DEFAULT_ERROR_PAGE_405),
	_error_page_413(DEFAULT_ERROR_PAGE_413),
	_error_page_500(DEFAULT_ERROR_PAGE_500)
{
	this->_routes.push_back(new ConfigRoute());
}

ConfigServer::ConfigServer(const std::string &serverConfig) {
	// Set default values
	this->_name = DEFAULT_SERVER_NAME;
	this->_host = DEFAULT_HOST;
	this->_port = DEFAULT_PORT;
	this->_error_page_400 = DEFAULT_ERROR_PAGE_400;
	this->_error_page_403 = DEFAULT_ERROR_PAGE_403;
	this->_error_page_404 = DEFAULT_ERROR_PAGE_404;
	this->_error_page_405 = DEFAULT_ERROR_PAGE_405;
	this->_error_page_413 = DEFAULT_ERROR_PAGE_413;
	this->_error_page_500 = DEFAULT_ERROR_PAGE_500;

	// Parse server config
	std::string line;
	std::istringstream serverConfigStream(serverConfig);

	while (std::getline(serverConfigStream, line)) {
		if (line.empty())
			continue;

		// Check if line contains strictly "route:"
		if (line == "route:") {
			std::string routeConfig;
			while (std::getline(serverConfigStream, line) && (line[0] == '\t' || line.empty())) {
				// Remove tabulation
				line.erase(0, 1);
				routeConfig += line + "\n";
			}
			if (line != ";") {
				throw InvalidCloseDirectiveException(line);
			}
			this->_routes.push_back(new ConfigRoute(routeConfig));
			continue;
		}

		std::string option = line.substr(0, line.find_first_of("="));
		std::string value = line.substr(line.find_first_of("=") + 1);

		// Set server options
		if (option == "name")
			this->_name = value;
		else if (option == "host")
			this->_host = value;
		else if (option == "port")
			this->_port = 1;
		else if (option == "error_page_400")
			this->_error_page_400 = value;
		else if (option == "error_page_403")
			this->_error_page_403 = value;
		else if (option == "error_page_404")
			this->_error_page_404 = value;
		else if (option == "error_page_405")
			this->_error_page_405 = value;
		else if (option == "error_page_413")
			this->_error_page_413 = value;
		else if (option == "error_page_500")
			this->_error_page_500 = value;
		else if (option == ";")
			continue;
		else
			throw InvalidOptionException(option);
	}
}

ConfigServer::~ConfigServer() {
	if (this->_routes.size() > 0) {
		for (std::vector<ConfigRoute*>::iterator it = this->_routes.begin(); it != this->_routes.end(); ++it) {
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
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_error_page_400 = rhs._error_page_400;
		this->_error_page_403 = rhs._error_page_403;
		this->_error_page_404 = rhs._error_page_404;
		this->_error_page_405 = rhs._error_page_405;
		this->_error_page_413 = rhs._error_page_413;
		this->_error_page_500 = rhs._error_page_500;
		this->_routes = rhs._routes;
	}
	return (*this);
}

std::string	ConfigServer::getName() const {
	return this->_name;
}

std::string	ConfigServer::getHost() const {
	return this->_host;
}

int	ConfigServer::getPort() const {
	return this->_port;
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

std::string	ConfigServer::getErrorPage413() const {
	return this->_error_page_413;
}

std::string	ConfigServer::getErrorPage500() const {
	return this->_error_page_500;
}

std::vector<ConfigRoute*>	ConfigServer::getRoutes() const {
	return this->_routes;
}

void	ConfigServer::addRoute(ConfigRoute &route) {
	this->_routes.push_back(&route);
}

std::ostream    &operator<<(std::ostream &o, ConfigServer const &rhs) {
	o << "=== Server ===" << std::endl;
	o << "name: " << rhs.getName() << std::endl;
	o << "host: " << rhs.getHost() << std::endl;
	o << "port: " << rhs.getPort() << std::endl;
	o << "error_page_400: " << rhs.getErrorPage400() << std::endl;
	o << "error_page_403: " << rhs.getErrorPage403() << std::endl;
	o << "error_page_404: " << rhs.getErrorPage404() << std::endl;
	o << "error_page_405: " << rhs.getErrorPage405() << std::endl;
	o << "error_page_413: " << rhs.getErrorPage413() << std::endl;
	o << "error_page_500: " << rhs.getErrorPage500() << std::endl;

	std::vector<ConfigRoute*> routes = rhs.getRoutes();
	for (std::vector<ConfigRoute*>::iterator it = routes.begin(); it != routes.end(); ++it) {
		o << *(*it) << std::endl;
	}
	return (o);
}