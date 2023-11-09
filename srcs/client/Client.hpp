/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/09 13:24:11 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "Request.hpp"
#include "../config/ConfigServer.hpp"
#include "../config/ConfigHostname.hpp"
#include "../cgi/CgiHandler.hpp"
#include "../autoindex/AutoIndex.hpp"
#include "ClientHelper.hpp"
#include "Response.hpp"

#define D_BUFF_SIZE 1024

class Client
{
	public:
		Client(ConfigServer *config, int &client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client(void);

		int				getSocket(void) const;
		ConfigServer	*getConfigServer(void) const;
		Request			*getRequest(void) const;

		int				readHandler(void);
		int 			writeHandler(void);
		void			_clear();

	private:
		ConfigServer	*_config_server;
		Request			*_request;
		int				_socket;

		std::string					_read;
		int							_nb_read;
		int							_total_bytes_send;
		bool						_headerOk;
		bool						_responseOK;
		bool						_CGI_on;
		bool						_error;
		Response*		_response;
		std::string					_server_message;

		Client(void);
};
