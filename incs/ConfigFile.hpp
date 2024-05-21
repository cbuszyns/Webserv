#pragma once

#include "WebServer.h"

#define DEFAULT_CONF = "default_config_file.conf" 

class ConfigFile
{
	private:
		std::ifstream _file;
		std::string _content;
		std::vector<std::string> _configString;
		std::vector<Configs> _configs;
		std::map<std::string, std::vector<Configs> > _mapConfigs;
		void setMapConfigs();
		std::vector<std::string> splitString(std::string input);
	public:
		ConfigFile(const std::string& file);
		~ConfigFile();
		Configs GetConfig(std::string host, std::string servername);
		std::map<std::string, std::vector<Configs> > GetMapConf();
		std::vector<Configs> GetConfVec();
};