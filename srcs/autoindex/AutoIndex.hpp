/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lorobert <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 11:08:30 by lorobert          #+#    #+#             */
/*   Updated: 2023/11/21 11:08:32 by lorobert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <sstream>

class AutoIndex {
	public:
		AutoIndex(const std::string &uri, const std::string &pwd);
		AutoIndex(const AutoIndex &src);
		AutoIndex &operator=(const AutoIndex &rhs);
		~AutoIndex();

		std::string getBody();
		bool		isExistingFile();

	private:
		AutoIndex();

		std::string 				_uri;
		std::string 				_pwd;
		std::string 				_body;
		std::vector<std::string> 	_folders;
		std::vector<std::string> 	_files;
};