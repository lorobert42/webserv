
#include "include.hpp"

int	main(void)
{
	Server	serv(INADDR_ANY, D_PORT);

	if(!serv.setup())
	{
		std::cout << "Fail to setup the serv." << std::endl;
		return (1);
	}
	if(!serv.run())
	{
		std::cout << "Server stoped" << std::endl;
		return (2);
	}
	return (0);
}
