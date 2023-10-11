/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:19:14 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/11 16:36:19 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "../config/Config.hpp"
#include "../client/Client.hpp"
#include "Server.hpp"

#define D_MAX_EVENTS 10050
#define D_BUFFER_SIZE 1024

class ServerManager {

	public:
		void	setup();
		bool	run();
		int		readHandler(int fd);
		int		writeHandler(int fd);

		ServerManager(Config* config);
		ServerManager(ServerManager const& other);
		~ServerManager();
		ServerManager& operator=(ServerManager const& other);

	private:
		Config*					_config;
        std::map<int, Server*> 	_servers;
		int						_epfd;
		//std::map<int, Client>	_clients;

		bool	_isServerSocket(int socket) const;
		Server*	_getServerBySocket(int socket);
		void	_newClient(int server_socket);
		bool	_epollCtlAdd(int epfd, int fd, uint32_t events);
		bool	_epollCtlMod(int epfd, int fd, uint32_t events);
		bool	_epollCtlDel(int epfd, int fd);

		ServerManager();
};
