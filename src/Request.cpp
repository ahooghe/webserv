#include "../include/Request.hpp"
#include "../include/Response.hpp"

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(Config config, std::string request)
{
	this->_config = config;
	this->_request = request;
	this->_status = 0;
}

void Request::execute()
{
	_processRequest();

	_errorCheck();
}

std::string Request::getResponse()
{
	std::string response = this->_httpVersion;
	if (this->_status == 0)
	{
		Response resp(*this);
		this->_status = resp.handleRequest();
		switch(this->_status)
		{
			case 200:
				response += " 200 OK\r\n\r\n" + resp.getResponse();
				break;
			case 201:
				response += " 201 Created\r\n\r\n";
				break;
			case 204:
				response += " 204 No Content\r\n\r\n";
				break;
			case 400:
				response += " 400 Bad Request\r\n\r\n";
				break;
			case 403:
				response += " 403 Forbidden\r\n\r\n";
				break;
			case 404:
				response += " 404 Not Found\r\n\r\n";
				break;
			case 405:
				response += " 405 Method Not Allowed\r\n\r\n";
				break;
			case 500:
				response += " 500 Internal Server Error\r\n\r\n";
				break;
			default:
				response += " 501 Not Implemented\r\n\r\n";
				break;
		}
	}
	if (this->_status == 400)
		response += this->_status + "Bad Request\r\n\r\n";
	return (response);
}

void Request::_processRequest()
{
	try
	{
		std::string line;
		std::istringstream iss(this->_request);
		if (std::getline(iss, line))
		{
			size_t firstSpace = line.find(' ');
			if (firstSpace != std::string::npos)
			{
				this->_method = line.substr(0, firstSpace);
				size_t secondSpace = line.find(' ', firstSpace + 1);
				if (secondSpace != std::string::npos)
				{
					this->_uri = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
					size_t crPos = line.find("\r", secondSpace + 1);
					if (crPos != std::string::npos)
					{
						this->_httpVersion = line.substr(secondSpace + 1, crPos - secondSpace - 1);
					}
					else
						throw std::exception();
				}
				else
					throw std::exception();
			}
			else
				throw std::exception();
		}
		else
			throw std::exception();

		while (std::getline(iss, line))
		{
			if (line.empty()) // Correctly checks for the end of the header section
				break;

			size_t colonPos = line.find_first_of(":");
			if (colonPos == std::string::npos || colonPos == 0 || colonPos == line.length() - 1)
				throw std::exception();

			std::string headerName = line.substr(0, colonPos);
			std::string headerValue = line.substr(colonPos + 1);
			size_t start = headerValue.find_first_not_of(" \t");
			if (start != std::string::npos)
				headerValue = headerValue.substr(start);
			else
				headerValue.clear();

			size_t end = headerValue.find_last_not_of(" \t\r");
			if (end != std::string::npos)
				headerValue = headerValue.substr(0, end + 1);

			if (headerName.empty() || headerValue.empty())
				throw std::exception();

			if (this->_headers.find(headerName) == this->_headers.end())
				this->_headers[headerName] = headerValue;
			else
				throw std::exception();
		}
		while (std::getline(iss, line))
		{
			this->_body += line + "\n";
		}
		if (!this->_body.empty())
			this->_body.erase(this->_body.length() - 1);
	}
	catch (const std::exception &e)
	{
		this->_status = 400;
	}
}

void Request::_errorCheck()
{
	if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE")
		throw std::exception();
	if (this->_httpVersion != "HTTP/1.1")
		throw std::exception();
	if (this->_uri.find("/") == std::string::npos)
		throw std::exception();
}

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