#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ConfigRoute.hpp"
#include "ConfigExceptions.hpp"
#include "ConfigHelper.hpp"

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
		double						getClientMaxBodySize() const;
		std::string					getPortAsString() const;
		std::string					getErrorPage400() const;
		std::string					getErrorPage403() const;
		std::string					getErrorPage404() const;
		std::string					getErrorPage405() const;
		std::string					getErrorPage411() const;
		std::string					getErrorPage413() const;
		std::string					getErrorPage500() const;
		std::string					getErrorPage501() const;
		std::string					getErrorPage505() const;
        std::vector<ConfigRoute*>	getRoutes() const;
		ConfigRoute					*getRouteWithUri(const std::string &uri) const;

    private:
		std::string					_name;
		std::string					_host;
		int							_port;
		double 						_client_max_body_size;
		std::string					_error_page_400;
		std::string					_error_page_403;
		std::string					_error_page_404;
		std::string					_error_page_405;
		std::string					_error_page_411;
		std::string					_error_page_413;
		std::string					_error_page_500;
		std::string					_error_page_501;
		std::string					_error_page_505;
        std::vector<ConfigRoute*>	_routes;
};

std::ostream    &operator<<(std::ostream &o, ConfigServer const &rhs);
