#include "../include/WebServer.hpp"

WebServer::WebServer()
{
}

WebServer::~WebServer()
{
}

void WebServer::init(const char *filePath)
{
    try
    {
        config.checkFormat(filePath);
        config.initConfig(filePath);
        config.errCheck();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

void WebServer::start()
{
    std::cout << "Starting server..." << std::endl;
    
    //test();
    std::string temp = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive";
    Request request(this->config, temp);
    request.execute();
    std::cout << request.getResponse() << std::endl;
}

//Testing all getters in Config.hpp
void WebServer::test()
{
    std::cout << "Test print:" << std::endl;
    std::cout << "Data in config:" << std::endl;
    std::cout << "Client max body size: " << config.getClientMaxBodySize() << std::endl;
    std::cout << "Default server port: " << config.getDefault() << std::endl;
    std::cout << "Error Page: 404: " << config.getErrorPage(404) << std::endl;
    std::cout << "Error Page: 506: " << config.getErrorPage(506) << std::endl;
    
    std::cout << "Default server name: " << config.getServer(config.getDefault()).getServerName() << std::endl;
    std::cout << "Default server root: " << config.getServer(config.getDefault()).getRoot() << std::endl;
    std::cout << "Default server index: " << config.getServer(config.getDefault()).getIndex() << std::endl;
    std::cout << "Default server cgi path: .php: " << config.getServer(config.getDefault()).getCgiPath(".php") << std::endl;
    std::cout << "Default server cgi path: .py: " << config.getServer(config.getDefault()).getCgiPath(".py") << std::endl;
    
    std::cout << "Default server / location: exists?: " << config.getServer(config.getDefault()).getLocation("/").getRealLocation() << std::endl;
    std::cout << "Default server / location: autoindex?: " << config.getServer(config.getDefault()).getLocation("/").getAutoindex() << std::endl;
    std::cout << "Default server / location: index: " << config.getServer(config.getDefault()).getLocation("/").getIndex() << std::endl;
    std::cout << "Default server / location: alias: " << config.getServer(config.getDefault()).getLocation("/").getAlias() << std::endl;
    std::cout << "Default server / location: http port: " << config.getServer(config.getDefault()).getLocation("/").getHttp() << std::endl;
    std::cout << "Default server / location: https port: " << config.getServer(config.getDefault()).getLocation("/").getHttps() << std::endl;

    std::cout << "Default server /test location: exists?: " << config.getServer(config.getDefault()).getLocation("/test").getRealLocation() << std::endl;
}