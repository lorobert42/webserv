
#pragma once

#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#define OK 1
#define TOO_SHORT 2
#define ERROR 3

class Request
{
	public :
		Request(std::string read);
		Request(Request const &other);
		Request &operator=(Request const &other);
		~Request(void);

		void	parseHeader();
		int	checkBody();

		void	appendBody(std::string const& to_add);
		void	setBody(std::string const& new_body);
		const std::string	&getMethod(void) const;
		const std::string	&getIndex(void) const;
		const std::string	&getVersion(void) const;
		const std::string	getValue(const std::string &key) const;
		const std::string &getBody() const;
		int								getError() const;

	private :
		Request(void);
		bool	_parseFirstLine(std::string& line);
		int	_checkBodyContentLength(size_t content_length);
		int	_checkBodyChunked();

		std::string _rawRequest;
		std::map<std::string, std::string>	_header;
		std::string	_method;
		std::string	_index;
		std::string	_version;
		std::string	_body;
		int					_error;
};
