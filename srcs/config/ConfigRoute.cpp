#include "ConfigRoute.hpp"

const std::string	DEFAULT_URI 					= "/";
const std::string	DEFAULT_PATH 					= "www/example";
const std::string	DEFAULT_INDEX 					= "index.html";
const bool			DEFAULT_GET 					= true;
const bool			DEFAULT_POST 					= false;
const bool			DEFAULT_DELETE 					= false;
const bool			DEFAULT_AUTOINDEX 				= false;
const std::string	DEFAULT_CGI 					= "";
const double		DEFAULT_CLIENT_MAX_BODY_SIZE	= 1 * 1024 * 1024;
const std::string   DEFAULT_UPLOAD_DIR				= "www/example/upload";

ConfigRoute::ConfigRoute() :
	_uri(DEFAULT_URI),
	_path(DEFAULT_PATH),
	_index(DEFAULT_INDEX),
	_get(DEFAULT_GET),
	_post(DEFAULT_POST),
	_delete(DEFAULT_DELETE),
	_autoindex(DEFAULT_AUTOINDEX),
	_cgi(DEFAULT_CGI),
	_client_max_body_size(DEFAULT_CLIENT_MAX_BODY_SIZE),
	_upload_dir(DEFAULT_UPLOAD_DIR)
	{}

ConfigRoute::ConfigRoute(const std::string &routeConfig) {
	// Set default values
	this->_uri = DEFAULT_URI;
	this->_path = DEFAULT_PATH;
	this->_index = DEFAULT_INDEX;
	this->_get = DEFAULT_GET;
	this->_post = DEFAULT_POST;
	this->_delete = DEFAULT_DELETE;
	this->_autoindex = DEFAULT_AUTOINDEX;
	this->_cgi = DEFAULT_CGI;
	this->_client_max_body_size = DEFAULT_CLIENT_MAX_BODY_SIZE;
	this->_upload_dir = DEFAULT_UPLOAD_DIR;

	// Parse route config
	std::string line;
	std::istringstream routeConfigStream(routeConfig);

	while (std::getline(routeConfigStream, line)) {
		if (line.empty())
			continue;

		std::string option = line.substr(0, line.find_first_of("="));
		std::string value = line.substr(line.find_first_of("=") + 1);

		// Set route options
		if (option == "uri")
			this->_uri = value;
		else if (option == "path")
			this->_path = value;
		else if (option == "index")
			this->_index = value;
		else if (option == "get")
			this->_get = ConfigHelper::convertStringToBool(value);
		else if (option == "post")
			this->_post = ConfigHelper::convertStringToBool(value);
		else if (option == "delete")
			this->_delete = ConfigHelper::convertStringToBool(value);
		else if (option == "autoindex")
			this->_autoindex = ConfigHelper::convertStringToBool(value);
		else if (option == "cgi")
			this->_cgi = value;
		else if (option == "client_max_body_size")
			this->_client_max_body_size = ConfigHelper::convertStringToClientMaxBodySize(value);
		else if (option == "upload_dir")
			this->_upload_dir = value;
		else
			throw InvalidOptionKeyException(option);
	}
}

ConfigRoute::~ConfigRoute() {}

ConfigRoute::ConfigRoute(ConfigRoute const &src) {
	*this = src;
}

ConfigRoute	&ConfigRoute::operator=(ConfigRoute const &rhs) {
	if (this != &rhs) {
		this->_uri = rhs._uri;
		this->_path = rhs._path;
		this->_index = rhs._index;
		this->_get = rhs._get;
		this->_post = rhs._post;
		this->_delete = rhs._delete;
		this->_autoindex = rhs._autoindex;
		this->_cgi = rhs._cgi;
	}
	return (*this);
}

std::string ConfigRoute::getUri() const {
	return this->_uri;
}

std::string ConfigRoute::getPath() const {
	return this->_path;
}

std::string ConfigRoute::getIndex() const {
	return this->_index;
}

bool	ConfigRoute::getGet() const {
	return this->_get;
}

bool	ConfigRoute::getPost() const {
	return this->_post;
}

bool	ConfigRoute::getDelete() const {
	return this->_delete;
}

bool	ConfigRoute::getAutoindex() const {
	return this->_autoindex;
}

std::string	ConfigRoute::getCgi() const {
	return this->_cgi;
}

double	ConfigRoute::getClientMaxBodySize() const {
	return this->_client_max_body_size;
}

std::string	ConfigRoute::getUploadDir() const {
	return this->_upload_dir;
}

std::ostream &operator<<(std::ostream &o, ConfigRoute const &rhs) {
	o << "=== Route ===" << std::endl;
	o << "uri: " << rhs.getUri() << std::endl;
	o << "path: " << rhs.getPath() << std::endl;
	o << "index: " << rhs.getIndex() << std::endl;
	o << "get: " << rhs.getGet() << std::endl;
	o << "post: " << rhs.getPost() << std::endl;
	o << "delete: " << rhs.getDelete() << std::endl;
	o << "autoindex: " << rhs.getAutoindex() << std::endl;
	o << "cgi: " << rhs.getCgi() << std::endl;
	o << "client_max_body_size: " << rhs.getClientMaxBodySize() << std::endl;
	o << "upload_dir: " << rhs.getUploadDir() << std::endl;
	return (o);
}