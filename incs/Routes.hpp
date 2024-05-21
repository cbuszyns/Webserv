#pragma once

#include "WebServer.h"

class Route{
	private:
		std::string _path;
		std::map<std::string, std::string> _map;
		void setMap(std::string config);
	public:
		Route();
		Route(std::string config, std::string path);
		~Route();
		std::string GetPath();
		std::string GetRoot();
		std::vector<std::string> GetCGIPath();
		std::string GetUploadPath();
		std::string GetMethods();
		std::string GetIndex();
		bool GetAutoIndex();
};