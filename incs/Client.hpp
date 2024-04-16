#pragma once

#include "WebServer.h"

class Clients
{
private:
	struct data
	{
		int fd;
		int evIdent;
		Configs *config;
		data(int conn, int evId) : fd(conn), evIdent(evId){ return; }
	} ;

	std::vector<data> _clients;
public:
	Clients();
	~Clients();

	data *GetConnection(int fd);
	int AddConnection(int fd, int evIdent);
	int DelConnection(int fd);
};
