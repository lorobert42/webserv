#include "strtolower.hpp"

void	strtolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}
