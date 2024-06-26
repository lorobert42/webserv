/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:26 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:27 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {
	this->_servers.push_back(new ConfigServer());
}

Config::Config(const char *configPath) {

	try {
		// Check extension must be .lms
		std::string path(configPath);
		if (path.substr(path.find_last_of(".") + 1) != "lms") {
			throw InvalidExtensionException();
		}

		// Open config file
		std::ifstream config(configPath);
		if (!config.is_open()) {
			throw CouldNotOpenFileException();
		}

		// Read config file
		std::string line;
		while (std::getline(config, line)) {
			if (line.empty())
				continue;

			// Check if line contains strictly "server:"
			if (line == "server:") {
				std::string serverConfig;
				while (std::getline(config, line) && (line[0] == '\t' || line.empty())) {
					// Remove tabulation
					line.erase(0, 1);
					serverConfig += line + "\n";
				}
				if (line != ";") {
					throw InvalidCloseDirectiveException(line);
				}
				if (serverConfig.empty()) {
					this->_servers.push_back(new ConfigServer());
				} else {
					this->_servers.push_back(new ConfigServer(serverConfig));
				}
			}
		}

		// Check if at least one server is defined
		if (this->_servers.size() == 0) {
			throw NoServerDefinedException();
		}
	} catch (std::exception &e) {
		if (this->_servers.size() > 0) {
			for (std::vector<ConfigServer*>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it) {
				delete (*it);
			}
		}
		throw;
	}
}

Config::~Config() {
	if (this->_servers.size() > 0) {
		for (std::vector<ConfigServer*>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it) {
			delete (*it);
		}
	}
}

Config::Config(Config const &src){
	*this = src;
}

Config	&Config::operator=(Config const &rhs){
	if (this != &rhs) {
		this->_servers = rhs._servers;
	}
	return (*this);
}

std::vector<ConfigServer*>   Config::getServers() const{
	return this->_servers;
}

std::ostream    &operator<<(std::ostream &o, Config const &rhs){
	std::vector<ConfigServer*> servers = rhs.getServers();
	if (servers.size() == 0) {
		o << "No servers found" << std::endl;
		return (o);
	}
	o << "=== Configuration ===" << std::endl;
	for (std::vector<ConfigServer*>::iterator it = servers.begin(); it != servers.end(); ++it) {
		o << *(*it) << std::endl;
	}
	return (o);
}
