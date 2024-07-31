#include "../include/Request.hpp"

std::string Request::getMethod() const
{
	return this->_method;
}

std::string Request::getUri() const
{
	return this->_uri;
}

std::string Request::getHttpVersion() const
{
	return this->_httpVersion;
}

std::string Request::getBody() const
{
	return this->_body;
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return this->_headers;
}

Config Request::getConfig() const
{
	return this->_config;
}
