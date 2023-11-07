/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/02 16:43:28 by lorobert         ###   ########.fr       */
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

class CgiHandler;

enum	code_error {
		E_SUCCESS = 0,
		E_ACCESS = 1,
		E_FAIL = 2
};

#define D_BUFF_SIZE 1024
#define D_200_MESSAGE "HTTP/1.1 200 OK"
#define D_201_MESSAGE "HTTP/1.1 201 Created"
#define D_301_MESSAGE "HTTP/1.1 301 Moved Permanently"
#define D_302_MESSAGE "HTTP/1.1 302 Found"
#define D_400_MESSAGE "HTTP/1.1 400 Bad Request"
#define D_403_MESSAGE "HTTP/1.1 403 Forbidden"
#define D_404_MESSAGE "HTTP/1.1 404 Not Found"
#define D_405_MESSAGE "HTTP/1.1 405 Method Not Allowed"
#define D_411_MESSAGE "HTTP/1.1 411 Length Required"
#define D_413_MESSAGE "HTTP/1.1 413 Content Too Large"
#define D_501_MESSAGE "HTTP/1.1 501 Not Implemented"
#define D_505_MESSAGE "HTTP/1.1 505 HTTP Version Not Supported"

class Client
{
	public:
		Client(ConfigServer *config, int &client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client(void);

		int				getSocket(void) const;
		ConfigServer	*getConfigServer(void) const;
		ConfigRoute		*getConfigRoute(void) const;
		Request			*getRequest(void) const;

		int				readHandler(void);
		int 			writeHandler(void);
		void			_clear();

	private:
		ConfigServer	*_config_server;
		Request			*_request;
		ConfigHostname	*_config_hostname;
		ConfigRoute		*_route;
		int				_socket;

		std::string					_read;
		int							_nb_read;
		int							_total_bytes_send;
		bool						_headerOk;
		bool						_respondOK;
		bool						_CGI_on;
		bool						_should_close;
		bool						_error;
		std::string					_path;
		std::string					_header;
		std::string					_body;
		std::string					_server_message;
		std::vector<std::string>	_uriSegments;

		void			_createRespond(void);
		bool			_checkPath(void);
		int				_checkFile(void);
		std::string		_fileFound(void);
		void			_createErrorResponse(int status);
		void			_parseRoute(const std::string &uri);
		std::string		_calculatePathFromUri(const std::string &uri);

		Client(void);
};
