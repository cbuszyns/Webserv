# include "../incs/WebServer.h"

Server::Server(unsigned int port, std::string host, Configs *config) : _port(port), _host(host), _config(config) {}

Server::~Server() {}

int Server::AddConnection(int fd){
	if (fd < 1)
		return (-1);
	int flags = fcntl(fd, F_GETFL, 0);

	assert(flags >= 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	this->_clients.push_back(std::make_pair(fd, ""));
	return 0;
}

void Server::connect() {
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1) {
		throw SocketExcept(std::string("Failed to create socket. errno: ").append(to_string(errno)));
	}
	_sockAddr.sin_family = AF_INET;
	if ((_sockAddr.sin_addr.s_addr = inet_addr(_host.c_str())) == INADDR_NONE) {
		throw SocketExcept(std::string("Host ").append(std::string(_host)).append(" is not valid."));
	}
	bool b = 1;
	setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(int));
	_sockAddr.sin_port = htons(_port);
	if (bind(_sockfd, (struct sockaddr*)&_sockAddr, sizeof(_sockAddr)) < 0) {
		throw SocketExcept(std::string("Failed to bind port ").append(to_string(_port)).append(". errno: ").append(to_string(errno)));
	}
	if (listen(_sockfd, 10) < 0) {
		throw SocketExcept(std::string("Failed to listen on socket. errno: ").append(to_string(errno)));
	}
	std::cout << GREEN << "Connected to host: " << BLUE << _host << GREEN << " and port: " << BLUE << _port << RESET <<std::endl;
}

void Server::disconnect() {
	close(_sockfd);
}

Configs Server::GetConfig() {
	return *_config;
}
