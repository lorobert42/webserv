/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:19:14 by lorobert          #+#    #+#             */
/*   Updated: 2023/09/28 14:14:31 by mjulliat         ###   ########.fr       */
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

#define D_MAX_EVENTS 10050
#define D_BUFFER_SIZE 1024

class Server {

	public:
		bool	setup();
		bool	run();
		int		readHandler(int fd);
		int		writeHandler(int fd);

		Server(unsigned int host, int port);
		Server(Server const& other);
		~Server();
		Server& operator=(Server const& other);

	private:
		Server();

		unsigned int				_host;
		int							_port;
		int							_socket;
		struct sockaddr_in			_addr;
		std::map<int, std::string>	_requests;
};
