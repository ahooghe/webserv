#include "../include/Request.hpp"

void Request::_errorCheck()
{
	if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE")
		this->_status = 405;
	if (this->_httpVersion != "HTTP/1.1")
		throw std::exception();
	if (this->_uri.find("/") == std::string::npos)
		throw std::exception();
}

std::string Request::makeErrorFile(int code)
{
	std::string filePath = this->_config.getErrorPage(code);
	std::ifstream file;
	std::stringstream ret;
	file.open(filePath.c_str());
	if (file.is_open())
		ret << file.rdbuf();
	else
		return (backup500file());
	file.close();
	return ret.str();
}

Request &Request::operator=(const Request src)
{
	this->_config = src.getConfig();
	this->_method = src.getMethod();
	this->_uri = src.getUri();
	this->_httpVersion = src.getHttpVersion();
	this->_body = src.getBody();
	this->_headers = src.getHeaders();
	return *this;
}

std::string Request::backup500file()
{
	std::string backup = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"UTF-8\">\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>Error 500 - Internal Server Error</title>\n\t<style>\n\t\tbody {\n\t\t\tdisplay: flex;\n\t\t\tjustify-content: center;\n\t\t\talign-items: center;\n\t\t\theight: 100vh;\n\t\t\tfont-family: Arial, sans-serif;\n\t\t\tbackground-color: #f0f0f0;\n\t\t}\n\t\t.container {\n\t\t\ttext-align: center;\n\t\t}\n\t</style>\n</head>\n<body>\n\t<div class=\"container\">\n\t\t<h1>Error 500</h1>\n\t\t<p>Internal Server Error</p>\n\t\t<p>The server encountered an internal error and was unable to complete your request.</p>\n\t</div>\n</body>\n</html>";
	return backup;
}
