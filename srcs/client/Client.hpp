/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/23 20:20:13 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "Request.hpp"
#include "../config/ConfigServer.hpp"

#define D_BUFF_SIZE 4096
#define D_200_MESSAGE "200 ok"
#define D_404_MESSAGE "404"

class Client
{
	public:
		Client(void);
		Client(ConfigServer *config, int &client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client(void);

		int	getSocket(void) const;
		std::string getRequest(void) const;

		int	readHandler(void);
		int writeHandler(void);
	
		bool		_checkFile(void);
		std::string	_requestFound(void);
		int			_requestNotFound(void);

	private:
		ConfigServer	*_config_server;
		Request			*_request;
		ConfigRoute		*_route;
		int				_socket;
		std::string		_read;
		std::string		_uri;
};
