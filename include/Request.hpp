#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Config.hpp"
#include <string>
#include <map>
#include <exception>


class Request
{
	private:
		std::string	_request;
		Config		_config;
		int			_status;

		std::string	_method;
		std::string	_uri;
		std::string	_httpVersion;
		std::string _body;
		std::map<std::string, std::string> _headers;

		void		_processRequest();
		void		_errorCheck();
	public:
		Request();
		Request(Config config, std::string request);
		~Request();

		void		execute();
		std::string	getResponse();

		std::string getMethod() const;
		std::string getUri() const;
		std::string getHttpVersion() const;
		std::string getBody() const;
		std::map<std::string, std::string> getHeaders() const;
		Config getConfig() const;

		Request &operator=(const Request src);
};

#endif

// Example of a GET request (no body)
		
//    GET /index.html HTTP/1.1
//    Host: www.example.com
//    User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)
//    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//    Accept-Language: en-US,en;q=0.5
//    Accept-Encoding: gzip, deflate
//    Connection: keep-alive



// Example of a POST request (with body)

//    POST /submit-form HTTP/1.1
//    Host: www.example.com
//    User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)
//    Content-Type: application/x-www-form-urlencoded
//    Content-Length: 27
//    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//    Accept-Language: en-US,en;q=0.5
//    Accept-Encoding: gzip, deflate
//    Connection: keep-alive

//    name=John+Doe&age=23



// Example of a DELETE request (typically no body)

//    DELETE /resource/123 HTTP/1.1
//    Host: www.example.com
//    User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)
//    Accept: */*
//    Accept-Language: en-US,en;q=0.5
//    Accept-Encoding: gzip, deflate
//    Connection: keep-alive
		