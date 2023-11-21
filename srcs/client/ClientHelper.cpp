/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientHelper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:08:55 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:08:56 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientHelper.hpp"

bool ClientHelper::isMethodAllowed(ConfigRoute *route, std::string const &method) {
	if (method == "GET" && route->getGet())
		return true;
	if (method == "POST" && route->getPost())
		return true;
	if (method == "DELETE" && route->getDelete())
		return true;
	return false;
}
