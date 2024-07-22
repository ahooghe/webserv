#include "../include/Request.hpp"

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
	/*if (this->_status == 0)
	{
		Response resp(this);
		resp.handleRequest();
	}*/
	if (this->_status == 400)
		response += this->_status + "Bad Request";
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

			size_t end = headerValue.find_last_not_of(" \t");
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