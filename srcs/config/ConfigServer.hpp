#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ConfigRoute.hpp"
#include "ConfigExceptions.hpp"

class ConfigServer {
    public:
		ConfigServer();
		ConfigServer(const std::string &serverConfig);
        ~ConfigServer();
        ConfigServer(ConfigServer const &src);
        ConfigServer    &operator=(ConfigServer const &rhs);

		std::string					getName() const;
		std::string					getHost() const;
		int							getPort() const;
		std::string					getErrorPage400() const;
		std::string					getErrorPage403() const;
		std::string					getErrorPage404() const;
		std::string					getErrorPage405() const;
		std::string					getErrorPage413() const;
		std::string					getErrorPage500() const;
        std::vector<ConfigRoute*>	getRoutes() const;
		ConfigRoute					*getRouteWithUri(const std::string &uri) const;

    private:
		std::string					_name;
		std::string					_host;
		int							_port;
		std::string					_error_page_400;
		std::string					_error_page_403;
		std::string					_error_page_404;
		std::string					_error_page_405;
		std::string					_error_page_413;
		std::string					_error_page_500;
        std::vector<ConfigRoute*>	_routes;
};

std::ostream    &operator<<(std::ostream &o, ConfigServer const &rhs);
