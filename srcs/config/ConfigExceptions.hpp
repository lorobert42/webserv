#pragma once

#include <iostream>
#include <exception>

class InvalidCloseDirectiveException : public std::exception {
	public:
		InvalidCloseDirectiveException(std::string const &directive) {
			this->_message = "[CONFIG] Invalid close directive: \"" + directive + "\"";
		}
		~InvalidCloseDirectiveException() throw() {}
		const char* what() const throw() {
			return this->_message.c_str();
		}

	private:
		std::string		_message;
};

class InvalidOptionException : public std::exception {
	public:
		InvalidOptionException(std::string const &option) {
			this->_message = "[CONFIG] Invalid option: \"" + option + "\"";
		}
		~InvalidOptionException() throw() {}
		const char* what() const throw() {
			return this->_message.c_str();
		}

	private:
		std::string		_message;
};