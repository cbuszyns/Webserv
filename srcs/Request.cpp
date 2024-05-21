#include "../incs/WebServer.h"

Request::Request(std::string string, size_t n)
{
    parser(string, n);
}

Request::Request()
{}

Request::~Request()
{}

void Request::parser(const std::string string, size_t n) {
	std::istringstream resp(string);
	std::string header;
	std::string::size_type index;
	size_t j = 2;
	while (std::getline(resp, header) && header != "\r") {
		size_t pos = 0;
		while ((pos = header.find(13)) != std::string::npos)
			header.erase(pos);
		index = header.find(':', 0);
		if(index != std::string::npos)
		{
			std::pair<std::string, std::string> tmp = std::make_pair(header.substr(0, index), header.substr(index + 2));
			if (_requestsMap.find("Cookie") != _requestsMap.end()
				&& tmp.first == "Cookie")
				_requestsMap["Cookie"].append("; " + tmp.second);
			else
				_requestsMap.insert(tmp);
		}
		else
			_requestsMap.insert(std::make_pair("Method", header));
		j += header.size() + 2;
	}
	if (n > j)
		_requestsMap.insert(std::make_pair("Body", string.substr(j, n - j)));
}

std::map<std::string, std::string> Request::GetHeaders() const
{
    return _requestsMap;
}


std::string Request::GetMethod() const
{
    try{
		return _requestsMap.at("Method").substr(0, _requestsMap.at("Method").find(' ', 0));
	}
	catch(const std::exception& e){	
		return std::string ();
	}
}

std::string Request::GetAccept() const
{
    return _requestsMap.at("Accept");
}

std::string Request::GetHost() const
{
    return _requestsMap.at("Host").substr(0, _requestsMap.at("Host").find(':', 0));
}

std::string Request::GetPath() const {
	size_t temp = _requestsMap.at("Method").find(' ', 0) + 1;
	std::string path = _requestsMap.at("Method").substr(temp, _requestsMap.at("Method").find(' ', temp + 1) - temp);
	temp = path.find('?', 0);

	if (temp == std::string::npos)
		return (path);
	else
		return path.substr(0, temp);
}


std::string Request::GetBody() const
{
    try
    {
        return _requestsMap.at("Body");
    }
    catch(const std::exception& e)
    {
        return std::string();
    }
}

std::string Request::GetQueryString() const
{
    std::string path = _requestsMap.at("Method").substr(_requestsMap.at("Method").find(' ', 0) + 1);
    path = path.substr(0, path.find(' ', 0));
    std::string::size_type index = path.find('?', 0);
    
    if(index != std::string::npos)
        return path.substr(index + 1);
    return std::string();
}
