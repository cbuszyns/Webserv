#pragma once

#include "WebServer.h"

class Request
{
	private:
		std::map<std::string, std::string> _requestsMap;
		void parser(const std::string string, size_t n);

	public:
		Request();
		Request(const std::string string, size_t n);
		~Request();

		std::map<std::string, std::string> GetHeaders() const;
		std::string GetHost() const;
		std::string GetMethod() const;
		std::string GetBody() const;
		std::string GetPath() const;
		std::string GetQueryString() const;
		std::string GetAccept() const;
};