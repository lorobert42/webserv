#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 16

int	main()
{
	std::cout << "Creating server socket" << std::endl;
	int	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1)
	{
		perror("Creating socket");
		return (1);
	}

	struct sockaddr_in	hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8080);
	//inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

	std::cout << "Binding socket to address" << std::endl;
	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
	{
		perror("Binding socket");
		return (1);
	}

	std::cout << "Make the socket listen" << std::endl;
	if (listen(listening, SOMAXCONN) == -1)
	{
		perror("Listening");
		return (1);
	}

	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);

	std::cout << "Accepting client connections" << std::endl;
	int	clientSocket = accept(listening, (sockaddr *)&client, &clientSize);
	if (clientSocket == -1)
	{
		perror("Connecting wiith client");
		return (1);
	}

	std::cout << "Client address: " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << std::endl;
	close(listening);

	std::cout << "Let's chat !" << std::endl;
	char	buf[BUF_SIZE];
	while (true)
	{
		// Clear buffer
		memset(buf, 0, BUF_SIZE - 1);

		// Wait message
		int	bytesRead = recv(clientSocket, buf, BUF_SIZE - 1, 0);
		if (bytesRead == -1)
		{
			perror("Connection error");
			break;
		}
		if (bytesRead == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		// Display message
		std::cout << "Received: " << std::string(buf, 0, bytesRead);

		// Return message
		send(clientSocket, buf, bytesRead + 1, 0);
	}

	close(clientSocket);
	return (0);
}