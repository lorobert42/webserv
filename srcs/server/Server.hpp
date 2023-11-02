/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 09:46:17 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/25 10:35:38 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../config/ConfigServer.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

class Server {
	public:
		ConfigServer* getConfig() const;
		std::string getName() const;
		int getPort() const;
		int getSocket() const;
		struct sockaddr_in getAddr() const;

		bool setup();

		Server(ConfigServer* config);
		Server(Server const& other);
		~Server();
		Server &operator=(Server const& other);

	private:
		ConfigServer*					_config;
		std::vector<ConfigHostname*>	_hostnames;
		std::string						_name;
		int								_port;
		int								_socket;
		struct sockaddr_in				_addr;

		Server();
};
