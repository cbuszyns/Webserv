#include "../incs/WebServer.h"

bool run = 1;

static void signal_handler(int i)
{
	if (i == SIGINT)
		run = 0;
}

std::vector<Server *> initServers(std::map<std::string, std::vector<Configs> > portConfigs)
{
	std::map<std::string, std::vector<Configs> >::iterator it = portConfigs.begin();
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
					if (servers[i]->AddConnection(connection))
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
				int errSend = 0;
				if (FD_ISSET(servers[i]->_clients[j].first, &readfd))
				{
					do
					{
						bytesRead = recv(servers[i]->_clients[j].first, buffer, 8192, 0);
						totalBytesRead += bytesRead;
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
						errSend = 1;
					}
				}
				std::string respChunck;
				int dataSent = 0;
				if(errSend == 0 && !resp.empty())
				{
					for(size_t k = 0; resp.size() != 0; k++)
					{
						respChunck = resp.substr(0, 35000);
						dataSent = send(servers[i]->_clients[j].first, servers[i]->_clients[j].second.c_str(), respChunck.size(), 0);
						if (dataSent <= 0)
						{
							break;
						}
						resp = resp.substr(dataSent);
					}
					bufferStr.clear();
					FD_CLR(servers[i]->_clients[j].first, &active);
					close(servers[i]->_clients[j].first);
				}
				usleep(100);
			}
		}
	}	
	for(size_t i = 0; i < servers.size(); i++)
	{
		FD_CLR(servers[i]->GetSocketfd(), &active);
		for(size_t j = 0 ; j < servers[i]->_clients.size(); j++)
		{
			close(servers[i]->_clients[j].first);
			FD_CLR(servers[i]->_clients[j].first, &active);
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
