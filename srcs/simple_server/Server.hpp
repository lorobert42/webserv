/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 13:19:14 by lorobert          #+#    #+#             */
/*   Updated: 2023/09/20 14:51:26 by lorobert         ###   ########.fr       */
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

#define MAX_EVENTS 1024
#define BUFFER_SIZE 1024

class Server {
	private:
		unsigned int				_host;
		int							_port;
		int							_socket;
		struct sockaddr_in			_addr;
		std::map<int, std::string>	_requests;

		Server();
	
	public:
		int setup();
		int run();
		int read_handler(int fd);
		int write_handler(int fd);

		Server(unsigned int host, int port);
		Server(Server const& other);
		~Server();
		Server& operator=(Server const& other);
};
