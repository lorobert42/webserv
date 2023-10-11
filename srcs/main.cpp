#include "config/Config.hpp"
#include "server/ServerManager.hpp"

int	main(int argc, char **argv)
{
	if (argc > 2) {
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return (1);
	}
	try {
		Config *config = argc == 2 ? new Config(argv[1]) : new Config();
		//std::cout << *config;
		ServerManager sm(config);
		sm.setup();
		sm.run();
		delete config;
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
