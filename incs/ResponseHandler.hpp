#ifndef RESPONSE_HANDLER_HPP
# define RESPONSE_HANDLER_HPP

# include "WebServer.h"

int execAutoindex();

class Server;
class RequestHandler;

class ResponseHandler
{
	private:
		Server *_server;
		RequestHandler *_request;
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
		ConfigsRoute getSimilarRoute(std::string path) const;

	public:
		ResponseHandler(Server *server, RequestHandler *request, Configs *config);
		ResponseHandler(RequestHandler *request, std::pair<std::string, std::string> error = std::make_pair("500", DEFAULT_ERROR_PATH));
		~ResponseHandler();
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

#endif