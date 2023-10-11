/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjulliat <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:29:33 by mjulliat          #+#    #+#             */
/*   Updated: 2023/10/11 14:41:04 by mjulliat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// ### Constructor ###
Client::Client()
{}

Client::Client(Server const& server, int client_socket) :
	_server(server), _socket(client_socket)
{}

// ### Copy Constructor ###
Client::Client(Client const& other) :
	_server(other._server), _socket(other._socket)
{}

// ### Destructor ###
Client::~Client()
{}

// ### Overload operator ###
Client& Client::operator=(Client const& other)
{
	if (this == &other)
		return (*this);
	this->_server = other._server;
	this->_socket = other._socket;
	return (*this);
}
