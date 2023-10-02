#pragma once

#include <iostream>
#include <list>
#include <string>

class ConfigRoute {
    public:
		ConfigRoute();
        ~ConfigRoute();
        ConfigRoute(ConfigRoute const &src);
        ConfigRoute    &operator=(ConfigRoute const &rhs);

        std::string getRoute() const;
		std::string getPath() const;
    private:
		std::string	_route;
		std::string	_path;
		std::string	_index;
};

std::ostream    &operator<<(std::ostream &o, ConfigRoute const &rhs);