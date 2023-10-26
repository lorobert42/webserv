#include <csignal>
#include <iostream>
#include "config/Config.hpp"
#include "server/ServerManager.hpp"

bool	g_should_stop = false;

static void sigHandler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << "Received SIGINT signal" << std::endl;
		g_should_stop = true;
	}
}

int	main(int argc, char **argv)
{
	if (argc > 2) {
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		return (1);
	}
	try {
		if (signal(SIGINT, sigHandler) == SIG_ERR)
		{
			std::cerr << "Cannot handle signal" << std::endl;
		}
		Config *config = argc == 2 ? new Config(argv[1]) : new Config();
		//std::cout << *config;
		ServerManager server_manager(config);
		server_manager.start();
		delete config;
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
