/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientHelper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:01 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:02 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "../config/ConfigRoute.hpp"

class ClientHelper {
	public:
		static bool isMethodAllowed(ConfigRoute *route, std::string const &method);
};