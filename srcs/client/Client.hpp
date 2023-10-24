/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/17 14:21:32 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include "Request.hpp"
#include "../config/ConfigServer.hpp"
#include "../cgi/CgiHandler.hpp"

class CgiHandler;

class Client
{
	public:
		Client(void);
		Client(ConfigServer *config, int &client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client(void);

		int				getSocket(void) const;
		ConfigServer	*getConfigServer(void) const;
		Request			*getRequest(void) const;

		int	readHandler(void);
		int writeHandler(void);
	
	private:
		ConfigServer	*_config_server;
		int				_socket;
		std::string		_read;
		Request			*_request;
};
