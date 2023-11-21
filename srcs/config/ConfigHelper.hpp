/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHelper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:09:40 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:09:41 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdlib.h>
#include <fstream>
#include "ConfigExceptions.hpp"

class ConfigHelper {
	public:
		static bool convertStringToBool(std::string const &str);
		static int convertStringToPort(std::string const &str);
		static std::string checkValidPathFolder(std::string const &str);
		static double convertStringToClientMaxBodySize(std::string const &str);
		static std::string getValidErrorPage(std::string const &default_path, std::string const &new_path);
		static int convertStringToInt(std::string const &str);
		static std::string checkValidCGIScript(std::string const &str);
		static std::string checkValidCGIBin(std::string const &str);
};