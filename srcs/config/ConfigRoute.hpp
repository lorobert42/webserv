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
		std::string getPathWithIndex() const;
		std::string getRedirect() const;
		bool		getGet() const;
		bool		getPost() const;
		bool		getDelete() const;
		bool		getAutoindex() const;
		std::string getCgiScript() const;
		std::string getCgiBin() const;
		double		getClientMaxBodySize() const;
		std::string getUploadDir() const;
    private:
		std::string	_uri;
		std::string	_path;
		std::string	_index;
		std::string	_redirect;
		bool		_get;
		bool		_post;
		bool		_delete;
		bool		_autoindex;
		std::string	_cgi_script;
		std::string	_cgi_bin;
		double 		_client_max_body_size;
		std::string	_upload_dir;
};

std::ostream    &operator<<(std::ostream &o, ConfigRoute const &rhs);