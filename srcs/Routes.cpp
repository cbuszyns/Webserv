#include "../incs/WebServer.h"

Route::Route(){
}

Route::Route(std::string config, std::string path): _path(path){
	setMap(config);
}

Route::~Route(){
}

void Route::setMap(std::string config){
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string::size_type i;

	config = config.substr(2, config.size() - 2);
	do {
		i = 0;
		while(isspace(config[i]))
			i++;
		start = config.find('=', 0);
		end = config.find('\n', 0);
		if(end != std::string::npos)
			_map.insert(std::make_pair<std::string, std::string>(
				config.substr(i, start - i),
				config.substr(start + 1, end - start - 1)));
		config = config.substr(end + 1);
	} while (end < config.size());

}

std::string Route::GetPath(){
	return _path;
}

std::string Route::GetRoot(){
	return _map["root"];
}

std::vector<std::string> Route::GetCGIPath(){
	std::vector<std::string> cgi;
	int delimiter = _map["cgi_pass"].find(' ');
	cgi.push_back(_map["cgi_pass"].substr(0, delimiter));
	cgi.push_back(_map["cgi_pass"].substr(delimiter + 1, _map["cgi_pass"].size() - delimiter - 1));
	return cgi;
}

std::string Route::GetUploadPath(){
	std::string uploadPath = _map["upload_directory"];
	if (uploadPath.empty())
		return std::string();
	return uploadPath;
}

bool Route::GetAutoIndex(){
	return _map["autoindex"] == "on";
}

std::string Route::GetMethods(){
	return _map["methods"];
}

std::string Route::GetIndex(){
	return _map["index"];
}