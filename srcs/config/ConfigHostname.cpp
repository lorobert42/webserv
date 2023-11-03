#include "ConfigHostname.hpp"

const std::string   DEFAULT_NAME = "localhost";

ConfigHostname::ConfigHostname(): _name(DEFAULT_NAME) {
	this->_routes.push_back(new ConfigRoute());
}

ConfigHostname::ConfigHostname(const std::string &serverConfig) {
	// Set default values
	this->_name = DEFAULT_NAME;

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
		} else {
			this->_routes.push_back(new ConfigRoute());
		}

		std::string option = line.substr(0, line.find_first_of("="));
		std::string value = line.substr(line.find_first_of("=") + 1);

		// Set server options
		if (option == "name")
			this->_name = value;
		else if (option == ";")
			continue;
		else
			throw InvalidOptionKeyException(option);
	}
}

ConfigHostname::~ConfigHostname() {
	if (this->_routes.size() > 0) {
		for (std::vector<ConfigRoute*>::iterator it = this->_routes.begin(); it != this->_routes.end(); ++it) {
			delete (*it);
		}
	}
}

ConfigHostname::ConfigHostname(ConfigHostname const &src) {
	*this = src;
}

ConfigHostname	&ConfigHostname::operator=(ConfigHostname const &rhs) {
	if (this != &rhs) {
		this->_name = rhs._name;
		this->_routes = rhs._routes;
	}
	return (*this);
}

std::string	ConfigHostname::getName() const {
	return this->_name;
}

std::vector<ConfigRoute*>	ConfigHostname::getRoutes() const {
	return this->_routes;
}

ConfigRoute	*ConfigHostname::getRouteWithUri(const std::string &uri) const {
	for (std::vector<ConfigRoute*>::const_iterator it = this->_routes.begin(); it != this->_routes.end(); ++it) {
		if ((*it)->getUri() == uri)
			return (*it);
	}
	return (NULL);
}

std::ostream    &operator<<(std::ostream &o, ConfigHostname const &rhs) {
	o << "=== ConfigHostname ===" << std::endl;
	o << "name: " << rhs.getName() << std::endl;

	std::vector<ConfigRoute*> routes = rhs.getRoutes();
	for (std::vector<ConfigRoute*>::iterator it = routes.begin(); it != routes.end(); ++it) {
		o << *(*it) << std::endl;
	}
	return (o);
}