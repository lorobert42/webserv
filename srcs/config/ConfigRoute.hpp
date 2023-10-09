#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "ConfigHelper.hpp"
#include "ConfigExceptions.hpp"

class ConfigRoute {
    public:
		ConfigRoute();
		ConfigRoute(const std::string &routeConfig);
        ~ConfigRoute();
        ConfigRoute(ConfigRoute const &src);
        ConfigRoute    &operator=(ConfigRoute const &rhs);

        std::string getUri() const;
		std::string getPath() const;
		std::string getIndex() const;
		bool		getGet() const;
		bool		getPost() const;
		bool		getDelete() const;
		bool		getAutoindex() const;
		std::string getCgi() const;
		std::string getClientMaxBodySize() const;
		std::string getUploadDir() const;
    private:
		std::string	_uri;
		std::string	_path;
		std::string	_index;
		bool		_get;
		bool		_post;
		bool		_delete;
		bool		_autoindex;
		std::string	_cgi;
		std::string	_client_max_body_size;
		std::string	_upload_dir;
};

std::ostream    &operator<<(std::ostream &o, ConfigRoute const &rhs);