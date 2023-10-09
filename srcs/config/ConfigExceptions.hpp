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

class InvalidOptionKeyException : public std::exception {
	public:
		InvalidOptionKeyException(std::string const &key) {
			this->_message = "[CONFIG] Invalid option key: \"" + key + "\"";
		}
		~InvalidOptionKeyException() throw() {}
		const char* what() const throw() {
			return this->_message.c_str();
		}

	private:
		std::string		_message;
};

class InvalidOptionValueException : public std::exception {
	public:
		InvalidOptionValueException(std::string const &value) {
			this->_message = "[CONFIG] Invalid option value \"" + value + "\"";
		}
		~InvalidOptionValueException() throw() {}
		const char* what() const throw() {
			return this->_message.c_str();
		}

	private:
		std::string		_message;
};