/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <lorobert@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:19:14 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/07 13:05:23 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <string>
#include <map>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "../config/Config.hpp"
#include "../client/Client.hpp"
#include "Server.hpp"

#define D_MAX_EVENTS 10050
#define D_TIMEOUT 200
#define D_TIMEOUT_REQUEST 60

typedef	struct s_timeout {
	ConfigServer	*server;
	time_t			now;
}	t_timeout;

class ServerManager {

	public:
		void	start();

		ServerManager(Config* config);
		ServerManager(ServerManager const& other);
		~ServerManager();
		ServerManager& operator=(ServerManager const& other);

	private:
		Config*					_config;
		int						_epfd;
		std::map<int, Server*> 	_servers;
		std::map<int, Client*>	_clients;
		std::map<int, time_t>	_timeout;

		// Initialization
		void	_setupServers();
		bool	_setupEpoll();
		// Main loop
		void	_run();
		// I/O management
		bool	_handleRead(int fd);
		bool	_handleWrite(int fd);
		// Clients management
		int		_newClient(int server_socket);
		Client*	_getClientBySocket(int socket);
		void	_closeClient(int client_socket);
		void	_closeAllClients();
		// Servers management
		bool	_isServerSocket(int socket) const;
		Server*	_getServerBySocket(int socket);
		int		_isTimeoutOK(t_timeout *timeout, int socket);
		// Epoll management
		bool	_epollCtlAdd(int epfd, int fd, uint32_t events);
		bool	_epollCtlMod(int epfd, int fd, uint32_t events);
		bool	_epollCtlDel(int epfd, int fd);

		ServerManager();
};
