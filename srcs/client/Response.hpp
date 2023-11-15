/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 12:57:53 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/14 12:29:00 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <exception>
#include <sstream>
#include "Request.hpp"
#include "../config/ConfigServer.hpp"
#include "../config/ConfigHostname.hpp"
#include "../config/ConfigRoute.hpp"
#include "../cgi/CgiHandler.hpp"
#include "../utils/readFile.hpp"
#include "ClientHelper.hpp"
#include "../autoindex/Autoindex.hpp"

enum	error_code
{
	E_SUCCESS = 0,
	E_ACCESS = 1,
	E_FAIL = 2
};

class Response
{
	public:
		Response(ConfigServer* config, Request* request);
		Response(Response const& other);
		Response& operator=(Response const& other);
		~Response();

		void				createResponse();
		ConfigServer*		getConfigServer();
		ConfigRoute*		getConfigRoute();
		Request*			getRequest();
		std::string const&	getHeader() const;
		std::string const&	getBody() const;
		bool				shouldClose() const;

	private:
		void		_createRoute();
		void		_parseRoute(std::string const& uri);
		bool		_checkRequestOK();
		bool		_checkRouteExists();
		bool		_checkMethodAllowed();
		bool		_checkRedirection();
		bool		_checkCgi();
		std::string	_createPathFromUri(std::string const& uri);
		bool		_checkPath();
		int			_checkDir();
		int			_checkFile();
		int			_checkAutoindex();
		bool		_createBody();
		void		_createHeader(int status);
		void		_createErrorResponse(int status);
		std::string	_get500Body();

		Response();

		ConfigServer	*_config_server;
		ConfigHostname	*_config_hostname;
		ConfigRoute		*_route;
		Request			*_request;

		std::string					_header;
		std::string					_body;
		std::string					_path;
		std::vector<std::string>	_uri_segments;
		bool						_should_close;
};
