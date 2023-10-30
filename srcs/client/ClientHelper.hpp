#pragma once

#include <string>
#include "../config/ConfigRoute.hpp"

class ClientHelper {
	public:
		static bool isMethodAllowed(ConfigRoute *route, std::string const &method);
};