#include "../include/WebServer.hpp"
#include "../include/Server.hpp"

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
	
	try 
	{
		Servers  server(config);
		while (1)
			server.pingServer();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

}