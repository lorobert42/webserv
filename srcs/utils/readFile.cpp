#include "readFile.hpp"

std::string	readFile(std::string const& path)
{
	std::string line;
	std::string all;
	std::ifstream ifs(path.c_str());

	if (!ifs)
		throw std::runtime_error("Cannot read file: " + path);
	while (std::getline(ifs, line))
		all.append(line);
	return (all);
}
