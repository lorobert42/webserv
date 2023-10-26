/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/26 13:14:08 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "Request.hpp"
#include "../config/ConfigServer.hpp"
#include "../cgi/CgiHandler.hpp"

class CgiHandler;

enum	code_error {
		E_SUCCESS = 0
		E_NO_ROUTE = 1,
		E_NO_DIRECTORY = 2,
		E_NO_FILE = 3;
};

#define D_BUFF_SIZE 4096
#define D_200_MESSAGE "HTTP/1.1 200 OK"
#define D_404_MESSAGE "HTTP/1.1 404"

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
	
		int			_checkFile(void);
		std::string	_requestFound(void);
		int			_requestNotFound(void);

	private:
		ConfigServer	*_config_server;
		Request			*_request;
		ConfigRoute		*_route;
		int				_socket;
		std::string		_read;
		std::string		_path;
};
