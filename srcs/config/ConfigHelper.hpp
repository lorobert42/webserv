#pragma once

#include <stdlib.h>
#include "ConfigExceptions.hpp"

class ConfigHelper {
	public:
		static bool convertStringToBool(std::string const &str);
		static int convertStringToPort(std::string const &str);
		static int convertStringToClientMaxBodySize(std::string const &str);
};