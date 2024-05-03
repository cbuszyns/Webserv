#include "../incs/WebServer.h"

bool run = 1;

static void signal_handler(int i)
{
	if (i == SIGINT)
		run = 0;
}

std::vector<Server *> initServers(std::map<std::string, std::vector<Configs>> portConfigs)
{
	std::map<std::string, std::vector<Configs>>::iterator it = portConfigs.begin();
	std::vector<Server *> servers;

	for (; it != portConfigs.end(); it++)
	{
		Server *s = new Server((*it).second[0].GetPort(), (*it).second[0].GetHost(), &((*it).second[0]));
		try
		{
			s->connect();
		}
		catch (const SocketExcept &e)
		{
			std::cout << RED << e.what() << RESET << std::endl;
		}
		servers.push_back(s);
	}
	return servers;
}

int main(int argc, char *argv[])
{
	std::string resp;
	if (argc > 2)
	{
		std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
		return 1;
	}
	ConfigFile cf(argc == 1 ? "default_config_file.conf" : argv[1]);
	signal(SIGINT, signal_handler);
	execAutoindex();

	std::vector<Server *> servers = initServers(cf.GetMapConfig());
	Clients clients;

	fd_set readfd, write, active;
	FD_ZERO(&active);
	for (size_t i = 0; i < servers.size(); i++)
	{
		FD_SET(servers[i]->GetSocketfd(), &active);
	}
	while (run)
	{
		readfd = write = active;
		if (select(FD_SETSIZE, &readfd, &write, NULL, NULL) < 0)
		{
			std::cerr << "Error: select: Bad file descriptor" << std::endl;
			break;
		}
		std::string bufferStr;
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (FD_ISSET(servers[i]->GetSocketfd(), &readfd))
			{
				int connection = accept(servers[i]->GetSocketfd(),NULL ,NULL);
				try
				{
					if (clients.AddConnection(connection, servers[i]->GetSocketfd()))
					{
						ResponseHandler resHeader(NULL, std::make_pair("500", DEFAULT_ERROR_PATH));
						throw ServerException("500", resHeader.createResp(500), connection);
					}
					else
					{
						servers[i]->_clients.push_back(std::make_pair(connection, ""));
						FD_SET(connection, &active);
					}
				}
				catch (const ServerException &e)
				{
					e.what();
				}
			}
			for (size_t j = 0; j < servers[i]->_clients.size(); j++)
			{
				Configs config;
				char buffer[8192];
				bzero(buffer, 8192);
				size_t totalBytesRead = 0;
				int bytesRead = 0;
				if (FD_ISSET(servers[i]->_clients[j].first, &readfd))
				{
					do
					{
						bytesRead = recv(servers[i]->_clients[j].first, buffer, 8192, 0);
						totalBytesRead += bytesRead;
						//std::cout << "totalBytesRead: "<<totalBytesRead<<std::endl;
						if (bytesRead >= 0)
							bufferStr.append(buffer, bytesRead);
						usleep(100000);
					} while (bytesRead > 0);
					RequestHandler reqHeader = RequestHandler(bufferStr, totalBytesRead + 1);
					try
					{
						config = cf.GetConfig((*servers[i]).GetHostPort(), reqHeader.GetHost());
						if (config.isEmpty())
							throw std::exception();
					}
					catch (const std::out_of_range &e)
					{
						continue;
					}
					catch (const std::exception &e)
					{
						std::cout << RED << "Error: no vaiable Config" << RESET << std::endl;
						continue;
					}
					ResponseHandler resHeader = ResponseHandler(servers[i], &reqHeader, &config);
					try
					{
						if (reqHeader.GetMethod() == "POST" && reqHeader.GetBody().length() > config.GetLimitSizeBody())
						{
							resHeader = ResponseHandler(NULL, std::make_pair("413", config.GetErrorPath("413")));
							throw ServerException(resHeader.getError().first,
												  resHeader.createResp(std::atoi(resHeader.getError().first.c_str())),
												  servers[i]->_clients[j].first);
						}
						else
						{
							if (!config.GetRedirectionCode())
							{
								resp.append(resHeader.createResp(200));
								servers[i]->_clients[j].second = resp;
							}
							else
							{
								resp.append(resHeader.createResp(config.GetRedirectionCode()));
								std::string redir("Location: ");
								redir.append(config.GetRedirectionUrl());
								redir.append("\r\n");
								resp.insert(resp.find('\n') + 1, redir);
								servers[i]->_clients[j].second = resp;
							}
							if (!resHeader.getError().first.empty())
							{
								std::cout << "error: " << resHeader.getError().second << std::endl;
								std::cout << "servers[i]->_clients[j].first: "<< servers[i]->_clients[j].first<<std::endl;
								resHeader = ResponseHandler(NULL, resHeader.getError());
								throw ServerException(resHeader.getError().first,
													  resHeader.createResp(std::atoi(resHeader.getError().first.c_str())),
													  servers[i]->_clients[j].first);
							}
						}
					}
					catch (const ServerException &e)
					{
						std::string errorResponse = e.what();
    					send(servers[i]->_clients[j].first, errorResponse.c_str(), errorResponse.size(), 0);
					}
					std::cout<<"resp: "<<resp<<std::endl;
				}
				std::string respChunck;
				int dataSent = 0;
				do
				{
					respChunck = resp.substr(0, 35000);
					dataSent = send(servers[i]->_clients[j].first, servers[i]->_clients[j].second.c_str(), respChunck.size(), 0);
					if (dataSent < 0)
						break;
					resp = resp.substr(dataSent);
				} while (resp.size());
	
				usleep(100);
			}
		}
	}
	
	std::cout << RED << "EXIT" << RESET << std::endl;
	std::vector<Server *>::iterator i = servers.begin();
	for (; i != servers.end(); i++)
	{
		(*i)->disconnect();
		delete (*i);
	}
	return 0;
}

// The line if (evList[i].filter == EVFILT_READ) is a condition used in code that utilizes the kqueue system call for event notification in Unix-like operating systems (such as BSD systems like macOS).

// kqueue is an advanced event notification mechanism that allows monitoring various types of events on file descriptors (sockets, pipes, files, etc.). It allows efficient monitoring of multiple file descriptors for various events, including reading, writing, and errors.

// In this specific condition, evList is an array of kevent structures, and EVFILT_READ is a constant indicating that the event filter is for read events. This condition checks if the event corresponds to a read event.

// To replicate a similar condition using select() instead of kqueue, you would typically use the FD_ISSET macro to check if a file descriptor is ready for reading. Here's how you can do it:

// c
// Copy code
// // Assume fd_set readfds is initialized and populated
// int max_fd = 0; // Maximum file descriptor
// // Populate readfds and set max_fd properly

// // Assume sockfd is the socket descriptor being checked
// if (FD_ISSET(sockfd, &readfds)) {
//     // sockfd is ready for reading
// }
// In this case, FD_ISSET(sockfd, &readfds) checks if the file descriptor sockfd is set in the readfds set, indicating that it is ready for reading.

// So, to replicate the condition if (evList[i].filter == EVFILT_READ) using select(), you would first set up the fd_set to monitor read events on specific file descriptors, and then use FD_ISSET to check if a particular file descriptor is ready for reading.
