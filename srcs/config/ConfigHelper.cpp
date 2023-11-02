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

std::string ConfigHelper::checkValidPathFolder(std::string const &str) {
	if (str[str.size() - 1] != '/')
		throw InvalidOptionValueException(str);
	return str;
}

double ConfigHelper::convertStringToClientMaxBodySize(std::string const &str) {
	std::string value = str.substr(0, str.size() - 1);
	std::string unit = str.substr(str.size() - 1);

	for (std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
		if (!std::isdigit(*it))
			throw InvalidOptionValueException(str);
	}

	double size = std::atof(value.c_str());

	if (unit == "K")
		size *= 1024;
	else if (unit == "M")
		size *= 1024 * 1024;
	else if (unit == "G")
		size *= 1024 * 1024 * 1024;
	else
		throw InvalidOptionValueException(str);

	return size;
}

std::string ConfigHelper::getValidErrorPage(std::string const &default_path, std::string const &new_path) {
	std::ifstream file(new_path.c_str());
	if (!file.is_open()) {
		file.close();
		return default_path;
	}
	return new_path;
}