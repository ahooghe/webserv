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
	
	//test();
	//std::string temp = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive";
	//Request request(this->config, temp);
	//request.execute();
	//std::cout << request.getResponse() << std::endl;
	Servers  server(config);

	server.pingServer();

}