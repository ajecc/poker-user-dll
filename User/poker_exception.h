#pragma once
#include <exception>
#include <string>
#include "debug.h"


class poker_exception_t : public std::exception
{
private:
	std::string message;
public:
	explicit 
	poker_exception_t(const std::string& message)
		: message(message) {}

	virtual const char* 
	what() const throw()
	{
		LOG_F(INFO, message.c_str());
		return message.c_str();
	}
};
