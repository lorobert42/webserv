/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strtolower.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:10:17 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:10:17 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "strtolower.hpp"

void	strtolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}
