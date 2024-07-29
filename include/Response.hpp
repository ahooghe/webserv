#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/wait.h>
#include <dirent.h>
#include <vector>
#include <algorithm>

#include "CGI.hpp"

class Response
{
	private:
		Request _request;
		std::string _response;

		int			_getRequest();
		int			_deleteRequest();
		int			_pushRequest();
		std::string	_createPath();
		Location	_getLocation();
		int			_isFile(std::string path);
		int			_getRequestIndex();
		int			_makeFile(const std::string &filename, const std::string &type, const std::string &content);
		int			_handlePush(std::istringstream& body, std::string boundary);

		Response();
	public:
		Response(Request request);
		~Response();

		int			handleRequest();
		std::string	getResponse();
};

#endif