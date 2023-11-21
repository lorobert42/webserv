/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:29 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:30 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <exception>
#include "ConfigServer.hpp"
#include "ConfigExceptions.hpp"

class Config {
    public:
		Config();
        Config(const char *configPath);
        ~Config();
        Config(Config const &src);
        Config	&operator=(Config const &rhs);

        std::vector<ConfigServer*>  getServers() const;
    private:
        std::vector<ConfigServer*>  _servers;
};

std::ostream    &operator<<(std::ostream &o, Config const &rhs);
