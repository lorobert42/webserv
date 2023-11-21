/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readFile.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:10:11 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:10:12 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "readFile.hpp"

std::string	readFile(std::string const& path)
{
	std::string line;
	std::string all;
	std::ifstream ifs(path.c_str());

	if (!ifs.is_open())
		throw std::runtime_error("Cannot read file: " + path);
	while (std::getline(ifs, line))
		all.append(line + "\n");
	ifs.close();
	return (all);
}
