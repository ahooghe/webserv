#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>

#include "Config.hpp"

class WebServer
{
private:
    Config config;

public:
    WebServer();
    ~WebServer();

    void init(const char *filePath);
    void start();

    void test();
};

#endif