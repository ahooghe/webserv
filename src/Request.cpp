#include "../include/Request.hpp"
#include "../include/Response.hpp"

Request::Request()
{
	this->_status = 0;
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
	try
	{
		_errorCheck();
	}
	catch(const std::exception& e)
	{
		this->_status = 400;
	}
	
}

std::string Request::getResponse()
{
	std::string response = "HTTP/1.1";
	if (this->_status == 400)
		response += " 400 Bad Request\r\n\r\n" + makeErrorFile(this->_status);
	else if (this->_status == 405)
		response += " 405 Method Not Allowed\r\n\r\n" + makeErrorFile(this->_status);
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
				response += " 400 Bad Request\r\n\r\n" + makeErrorFile(this->_status);
				break;
			case 403:
				response += " 403 Forbidden\r\n\r\n" + makeErrorFile(this->_status);
				break;
			case 404:
				response += " 404 Not Found\r\n\r\n" + makeErrorFile(this->_status);
				break;
			case 405:
				response += " 405 Method Not Allowed\r\n\r\n" + makeErrorFile(this->_status);
				break;
			case 413:
				response += " 413 Payload Too Large\r\n\r\n" + makeErrorFile(this->_status);
				break;
			case 415:
				response += " 415 Unsupported Media Type\r\n\r\n" + makeErrorFile(this->_status);
				break;
			case 500:
				response += " 500 Internal Server Error\r\n\r\n" + makeErrorFile(this->_status);
				break;
			default:
				response += " 501 Not Implemented\r\n\r\n" + makeErrorFile(this->_status);
				break;
		}
	}
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
			std::istringstream firstline(line);
			firstline >> _method >> _uri >> _httpVersion;
			if (_method.empty() || _uri.empty() || _httpVersion.empty())
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
				continue;
	

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
			{
				this->_headers[headerName] += "+" + headerValue;
			}
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
