#pragma once

# include "WebServer.h"
# include "Routes.hpp"

class Route;

class Configs
{
    private:
    std::map<std::string, std::string> _map;
    std::vector<std::string> _methods;
    std::map<std::string, Route> _confRoute; 
    void setMap(std::string config);
    void setMethods();
    void setConfPath();

    public:
    Configs();
    ~Configs();
    Configs(std::string config);
    bool isEmpty();
    unsigned int GetPort();
    int GetRedir();
    size_t GetMaxBodySize();
    bool isMethod(std::string method);
    std::string GetHost();
    std::string GetUrl();
    std::string GetHostPort();
    std::string GetServerName();
    std::string GetPathErr(std::string code) const;
    std::map<std::string, Route> GetRoute();
};
