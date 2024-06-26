#ifndef WEBSERVER_H
#define WEBSERVER_H

# define RED "\e[0;31m"
# define GREEN "\e[0;32m"
# define YELLOW "\e[0;33m"
# define BLUE "\e[0;34m"
# define MAGENTA "\e[0;35m"
# define CYAN "\e[0;36m"
# define RESET "\e[0m"



#define MAX_EVENTS 1024
#define DEFAULT_ERROR_PATH "errors/error_500.html"
#define PYTHON3PATH "/bin/python3"
#define AUTOINDEXPATH "./utils/generate_directory_index.py"
#define MAX_BUFFER_SIZE 1024

# include <iomanip>
# include <fcntl.h>
# include <cstring>
# include <cstdlib>
# include <climits>
# include <ctype.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/wait.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <cassert>
# include <ctime>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <algorithm>
# include "Server.hpp"
# include "Config.hpp"
# include "ConfigFile.hpp"
# include "Routes.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "SocketExcept.hpp"
# include "ServerException.hpp"

#endif