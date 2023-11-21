/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHostname.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:47 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:48 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "ConfigRoute.hpp"
#include "ConfigExceptions.hpp"
#include "ConfigHelper.hpp"

class ConfigHostname {
public:
	ConfigHostname();
	ConfigHostname(const std::string &config_hostname);
	~ConfigHostname();
	ConfigHostname(ConfigHostname const &src);
	ConfigHostname    &operator=(ConfigHostname const &rhs);

	std::string					getName() const;
	std::vector<ConfigRoute*>	getRoutes() const;
	ConfigRoute					*getRouteWithUri(const std::string &uri) const;

private:
	std::string					_name;
	std::vector<ConfigRoute*>	_routes;
};

std::ostream    &operator<<(std::ostream &o, ConfigHostname const &rhs);
