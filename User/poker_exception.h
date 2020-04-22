#pragma once
#include <exception>
#include <string>
#include "debug.h"


class poker_exception_t : public std::exception
{
private:
	std::string message;
public:
	explicit poker_exception_t(const std::string& message) : message(message) 
	{
#ifdef _DEBUG
		DLOG(INFO, ("Exception encountered (" + message + "). Breaking...").c_str());
		__debugbreak();
#endif
	}
	virtual const char* what() const throw()
	{
		return message.c_str();
	}
};
