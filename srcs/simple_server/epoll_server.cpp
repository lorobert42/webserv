#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT		8080
#define MAX_EVENTS	10
#define BUF_SIZE	16

static int	setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFD, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
	{
		return (-1);
	}
	return (0);
}

static void	epoll_ctl_add(int epfd, int fd, uint32_t events)
{
	struct epoll_event	ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		perror("Adding socket to epoll");
		exit(1);
	}
}

int	main()
{
	std::cout << "Creating server socket" << std::endl;
	int	listen_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (listen_sock == -1)
	{
		perror("Creating socket");
		return (1);
	}

	std::cout << "Configure the listen address:port as 0.0.0.0:8080" << std::endl;
	struct sockaddr_in	srv_addr;
	bzero((char *)&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = htons(PORT);

	std::cout << "Binding socket to address" << std::endl;
	if (bind(listen_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1)
	{
		perror("Binding socket");
		return (1);
	}

	/* std::cout << "Make the socket non-blocking" << std::endl;
	if (setnonblocking(listen_sock) == -1)
	{
		perror("Setting socket as non-blocking");
		return (1);
	} */

	std::cout << "Make the socket listen" << std::endl;
	if (listen(listen_sock, SOMAXCONN) == -1)
	{
		perror("Listening");
		return (1);
	}

	std::cout << "Create the epoll structure" << std::endl;
	int	epfd = epoll_create(1);
	if (epfd == -1)
	{
		perror("Creating epoll");
		return (1);
	}

	std::cout << "Add socket to epoll" << std::endl;
	epoll_ctl_add(epfd, listen_sock, EPOLLIN | EPOLLOUT | EPOLLET);

	std::cout << "Setting variables for future use" << std::endl;
	int	ready_fds; // Number of fds that are ready to be used by epoll
	struct epoll_event	events[MAX_EVENTS]; // Events that must be treated
	int	i; // Avoid creating variable multiple time
	int	conn_sock; // Store new connection socket
	struct sockaddr_in	cli_addr; // Client socket address
	socklen_t	socklen = sizeof(cli_addr); // Length of client struct
	char	buf[BUF_SIZE];	// Buffer to receive data
	int		n;	// To store number of read bytes


	std::cout << "Listening to connections" << std::endl;
	for (;;)
	{
		ready_fds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (ready_fds == -1)
		{
			perror("epoll_wait");
			return (1);
		}
		for (i = 0; i < ready_fds; i++)
		{
			if (events[i].data.fd == listen_sock)
			{
				std::cout << "Received new connection" << std::endl;
				conn_sock = accept(listen_sock, (struct sockaddr *)&cli_addr, &socklen);
				if (conn_sock == -1)
				{
					perror("Connecting wiith client");
					return (1);
				}
				std::cout << "Client address: " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) << std::endl;
				if (setnonblocking(conn_sock) == -1)
				{
					perror("Setting socket as non-blocking");
					return (1);
				}
				epoll_ctl_add(epfd, conn_sock, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);
			}
			else if (events[i].events & EPOLLIN)
			{
				std::cout << "Received EPOLLIN event" << std::endl;
				for (;;)
				{
					bzero(buf, sizeof(buf) - 1);
					n = recv(events[i].data.fd, buf, sizeof(buf) - 1, 0);
					if (n <= 0)
					{
						break;
					}
					else
					{
						std::cout << buf;
						send(events[i].data.fd, buf, n + 1, 0);
					}
				}
			}
			else
			{
				std::cerr << "Unexpected event" << std::endl;
			}
			if (events[i].events & (EPOLLRDHUP | EPOLLHUP))
			{
				std::cout << "Connection closed" << std::endl;
				epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
				close(events[i].data.fd);
				continue;
			}
		}
	}
}