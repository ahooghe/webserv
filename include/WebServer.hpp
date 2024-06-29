#include "Config.hpp"
#include <iostream>

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