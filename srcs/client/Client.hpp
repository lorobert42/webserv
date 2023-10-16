/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/16 12:14:22 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../server/Server.hpp"
#include <iostream>
#include <string>
#include <fstream>

class Client
{
	public:
		Client(void);
		Client(Server *server, int &client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client(void);

		int	getSocket(void) const;
		std::string getRequest(void) const;

		int	readHandler(void);
		int writeHandler(void);
	
	private:
		Server			*_server;
		int				_socket;
		std::string		_request;


};
