#pragma once

# include "WebServer.h"

class Server;
class Request;

class Response
{
	private:
		Server *_server;
		Request *_request;
		Configs*_config;
		std::map<int, std::string> _code;
		std::string _path;
		std::string _content;
		std::string _contentType;
		int _contentLenght;
		std::map<std::string, std::string> _env;
		std::pair<std::string, std::string> _error;

		void setCodeMap();
		void setPath();
		void setContent();
		void setContentType(std::string path, std::string type = "");
		void setEnv();
		char **getEnvAsCstrArray() const;
		std::string executeCgi(const std::vector<std::string>& cgiPas);
		Route getSimilarRoute(std::string path) const;

	public:
		Response(Server *server, Request *request, Configs *config);
		Response(Request *request, std::pair<std::string, std::string> error = std::make_pair("500", DEFAULT_ERROR_PATH));
		~Response();
		std::string createResp(int code) const;
		std::string getRespCode(int code) const;
		std::string getContentType() const;
		std::string getDate() const;
		std::pair<std::string, std::string> getError() const;
		std::string getPath()
		{
			return _path;
		}
};

int execAutoindex();