#include "ClientHelper.hpp"

bool ClientHelper::isMethodAllowed(ConfigRoute *route, std::string const &method) {
	if (method == "GET" && route->getGet())
		return true;
	if (method == "POST" && route->getPost())
		return true;
	if (method == "DELETE" && route->getDelete())
		return true;
	return false;
}
