#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ConfigHostname.hpp"
#include "ConfigExceptions.hpp"
#include "ConfigHelper.hpp"

class ConfigServer {
    public:
		ConfigServer();
		ConfigServer(const std::string &serverConfig);
        ~ConfigServer();
        ConfigServer(ConfigServer const &src);
        ConfigServer    &operator=(ConfigServer const &rhs);

		std::string						getName() const;
		int								getPort() const;
		double							getClientMaxBodySize() const;
		int								getTimeout() const;
		std::string						getPortAsString() const;
		std::string						getErrorPageByCode(const int &code) const;
		std::string						getErrorPage400() const;
		std::string						getErrorPage403() const;
		std::string						getErrorPage404() const;
		std::string						getErrorPage405() const;
		std::string						getErrorPage411() const;
		std::string						getErrorPage413() const;
		std::string						getErrorPage500() const;
		std::string						getErrorPage501() const;
		std::string						getErrorPage505() const;
		std::string						getErrorPage508() const;
        std::vector<ConfigHostname*>	getHostnames() const;
		ConfigHostname					*getHostnameWithName(const std::string &name) const;

    private:
		std::string						_name;
		int								_port;
		double 							_client_max_body_size;
		int 							_timeout;
		std::string						_error_page_400;
		std::string						_error_page_403;
		std::string						_error_page_404;
		std::string						_error_page_405;
		std::string						_error_page_411;
		std::string						_error_page_413;
		std::string						_error_page_500;
		std::string						_error_page_501;
		std::string						_error_page_505;
		std::string 					_error_page_508;
        std::vector<ConfigHostname*>	_hostnames;
};

std::ostream    &operator<<(std::ostream &o, ConfigServer const &rhs);
