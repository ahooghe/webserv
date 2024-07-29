#include "../include/Response.hpp"
#include <iomanip>

Response::Response()
{
}

Response::~Response()
{
}

Response::Response(Request request)
{
	this->_request = request;
}

int Response::handleRequest()
{
	if (this->_request.getMethod() == "GET")
	{
		return _getRequest();
	}
	else if (this->_request.getMethod() == "DELETE")
	{
		return _deleteRequest();
	}
	else if (this->_request.getMethod() == "PUSH")
	{
		return _pushRequest();
	}
	else
		return 405;
}

std::string Response::getResponse()
{
	return this->_response;
}

int Response::_getRequest()
{
	std::ifstream file;
	std::stringstream ret;
	Location location = _getLocation();
	std::string path = _createPath();
	int filetype = _isFile(path);
	std::string filesuffix = this->_request.getUri().substr(this->_request.getUri().find_last_of("."));
	if (filesuffix[0] != '.')
		return 400;
	if (filesuffix != ".html")
	{
		CGI cgi(this->_request);
		int status = cgi.execute();
		this->_response = cgi.getResponse();
		return status;
	}
	if (location.getRealLocation() == false || path.empty())
		return 400;
	if (location.getAutoindex() == true && filetype == 1)
		return (_getRequestIndex());
	if (filetype != 0)
		return filetype;
	file.open(path.c_str());
	if (file.is_open())
		ret << file.rdbuf();
	else
		return 404;
	file.close();
	if (ret.str().empty())
		return 404;
	this->_response = ret.str();
	return 200;
}

int Response::_deleteRequest()
{
	std::string path = _createPath();
	if (path.empty())
		return 400;
	pid_t pid = fork();
	if (pid == -1)
		return 500;
	else if (pid > 0)
	{
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
			return 204;
		else
			return 404;
	}
	else
	{
		char *args[] = {(char *)"/bin/rm", (char *)path.c_str(), NULL};
		execve(args[0], args, NULL);
	}
	return 500;
}

int Response::_pushRequest()
{
	std::map<std::string, std::string> headers = this->_request.getHeaders();
	if (headers.find("Content-Type") == headers.end() || headers["Content-Type"].find("multipart/form-data") == std::string::npos)
		return 400;

	std::string contentType = headers["Content-Type"];
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos)
		return 400;

	std::string boundary = "--" + contentType.substr(boundaryPos + 9);
	if (this->_request.getBody().empty())
		return 400;
	std::istringstream body(this->_request.getBody());

	int statusPush = _handlePush(body, boundary);

	return statusPush;
}
