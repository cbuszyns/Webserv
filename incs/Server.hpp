#pragma once

#include "WebServer.h"

template<typename T>
std::string to_string(const T & value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

class Configs;

class Server {
	private:
		int _sockfd;
		int _port;
		std::string _host;
		sockaddr_in _sockAddr;
		
		Configs *_config;
	public:
		Server(unsigned int port, std::string host, Configs *config);
		~Server();
		std::vector<std::pair<int, std::string> > _clients;
		void connect();
		void disconnect();
		int AddConnection(int fd);
		int GetSocketfd() { return _sockfd; }
		int GetPort() { return _port; }
		std::string GetHost() { return _host; }
		std::string GetHostPort()
		{
			std::string pog2;
			std::stringstream pog;
			std::string hostPort = _host;
			hostPort.append(":");
			pog << _port;
			pog >> pog2;
			return hostPort.append(pog2);
		}
		sockaddr_in* GetSockAddr() { return &_sockAddr; };
		// struct kevent* GetEvSet() { return &_evSet; };
		Configs GetConfig();
};
