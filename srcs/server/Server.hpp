/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 09:46:17 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/11 15:53:40 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../config/ConfigServer.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <stdexcept>
#include <sys/socket.h>

class Server {
	public:
		bool setup();
		ConfigServer* getConfig() const;
		std::string getName() const;
		std::string getHost() const;
		int getPort() const;
		int getSocket() const;
		struct sockaddr_in getAddr() const;

		Server();
		Server(ConfigServer* config);
		Server(Server const& other);
		~Server();
		Server &operator=(Server const& other);

	private:
		ConfigServer*		_config;
		std::string			_name;
		std::string			_host;
		int					_port;
		int					_socket;
		struct sockaddr_in	_addr;
};
