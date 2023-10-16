/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:19:14 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/16 14:59:13 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <stdexcept>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "../config/Config.hpp"
#include "../client/Client.hpp"
#include "Server.hpp"

#define D_MAX_EVENTS 10050
#define D_BUFFER_SIZE 1024
#define D_TIMEOUT 200

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

		void	_setupServers();
		bool	_setupEpoll();
		bool	_run();
		bool	_isServerSocket(int socket) const;
		Server*	_getServerBySocket(int socket);
		Client*	_getClientBySocket(int socket);
		void	_newClient(int server_socket);
		void	_closeClient(int client_socket);
		bool	_epollCtlAdd(int epfd, int fd, uint32_t events);
		bool	_epollCtlMod(int epfd, int fd, uint32_t events);
		bool	_epollCtlDel(int epfd, int fd);

		ServerManager();
};
