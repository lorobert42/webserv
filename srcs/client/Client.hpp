/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/11 14:39:18 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../server/Server.hpp"
#include <iostream>
#include <string>

class Client
{
	public:
		Client(void);
		Client(Server const& server, int client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client(void);
	
	private:
		Server			_server;
		int				_socket;
		std::string		_request;


};
