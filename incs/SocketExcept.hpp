#include "WebServer.h"

class SocketExcept: public std::exception
{
	private:
		std::string _error;
	public:
		SocketExcept(std::string error): exception(), _error(error){}

		const char *what() const throw()
		{
			std::string resp("An error accured while opening the socket: ");
			resp.append(_error);
			const char * c_resp = resp.c_str();
			return c_resp;
		}
		~SocketExcept() throw(){}
};