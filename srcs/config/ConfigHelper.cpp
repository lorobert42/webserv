#include "ConfigHelper.hpp"

bool ConfigHelper::convertStringToBool(std::string const &str) {
	if (str == "true")
		return true;
	else if (str == "false")
		return false;
	else
		throw InvalidOptionValueException(str);
}

int ConfigHelper::convertStringToPort(std::string const &str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!std::isdigit(*it))
			throw InvalidOptionValueException(str);
	}
	int port = std::atoi(str.c_str());
	if (port < 0 || port > 65535)
		throw InvalidOptionValueException(str);
	return port;
}