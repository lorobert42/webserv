/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 16:44:56 by lorobert          #+#    #+#             */
/*   Updated: 2023/10/10 16:49:52 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <string>

class Client
{
	public:
		Server&			getServer() const;
		int				getSocket() const;
		std::string		getRequest() const;

		Client(Server const& server, int client_socket);
		Client(Client const& other);
		Client& operator=(Client const& other);
		~Client();
	
	private:
		Server 			_server;
		int				_socket;
		std::string		_request;

		Client();
};