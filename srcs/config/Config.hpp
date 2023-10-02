#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <exception>
#include "ConfigServer.hpp"

class Config {
    public:
		Config();
        Config(const char *configPath);
        ~Config();
        Config(Config const &src);
        Config	&operator=(Config const &rhs);

        std::vector<ConfigServer*>   getServers() const;
    private:
        std::vector<ConfigServer*>   _servers;

	class CouldNotOpenFileException : public std::exception {
		public:
			virtual const char* what() const throw() {
				return ("[CONFIG] Could not open config file");
			}
	};
};

std::ostream    &operator<<(std::ostream &o, Config const &rhs);