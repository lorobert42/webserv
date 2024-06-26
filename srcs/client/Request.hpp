/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:13 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:15 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <stdexcept>
#include "../utils/strtolower.hpp"

enum {
	ERROR = -1,
	OK = 0,
	TOO_SHORT = 1,
};

class Request
{
	public :
		Request();
		Request(double client_max_body_size);
		Request(Request const &other);
		Request &operator=(Request const &other);
		~Request(void);

		int	parseHeader();
		int	checkBody();
		void	clear();
		void	appendRawRequest(char* to_add, int size);
		void	appendBody(char* to_add, int size);
		void	setBody(std::string const& new_body);
		const std::string &getRawRequest() const;
		const std::string	&getMethod(void) const;
		std::string			getHostname(void) const;
		const std::string	&getUri(void) const;
		const std::string	&getVersion(void) const;
		const std::string	getValue(const std::string &key) const;
		const std::string 	&getBody() const;
		int					getError() const;

	private :
		std::string	_getline(std::string& data);
		bool	_parseFirstLine(std::string& line);
		bool	_parseFields();
		int	_checkBodyContentLength(size_t content_length);
		int	_checkBodyChunked();
		void	_printRequest() const;

		std::string _rawRequest;
		std::map<std::string, std::string>	_header;
		std::string	_method;
		std::string	_uri;
		std::string	_version;
		std::string	_body;
		int			_error;
		double		_client_max_body_size;
};
