#include "../incs/WebServer.h"

Clients::Clients() { return ; }

Clients::~Clients()
{
}

Clients::data *Clients::GetConnection(int fd) {
	int n = 0;
	std::vector<data>::iterator i = _clients.begin();

	for ( ; i != _clients.end(); i++, n++)
		if ((*i).fd == fd)
			return &(*i);
	return NULL;
}

int Clients::AddConnection(int fd, int evIdent) {
	if (fd < 1)
		return (-1);
	int flags = fcntl(fd, F_GETFL, 0);

	assert(flags >= 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	_clients.push_back(data(fd, evIdent));
	return 0;
}

int Clients::DelConnection(int fd) {
	if (fd < 1)
		return (-1);
	data* cd = GetConnection(fd);

	if (!cd)
		return (-1);
	std::vector<data>::iterator it = _clients.begin();
	for ( ; (*it).fd != (*cd).fd; it++);
	_clients.erase(it);
	return close(fd);
}