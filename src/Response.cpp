#include "../include/Response.hpp"
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
	else if (this->_request.getMethod() == "POST")
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
#include <iostream>
int Response::_getRequest()
{
	std::cout << "Meow" << std::endl;
	std::ifstream file;
	std::stringstream ret;
	Location location = _getLocation();
	std::string path = _createPath();
	std::cout << path << std::endl;
	if (location.getRealLocation() == false)
		return 400;
	if (location.getGet() == false)
	{
		if (this->_request.getUri().find(".") != std::string::npos)
		{
			Location locExt = _getExtensionLocation();
			if (locExt.getRealLocation() == false)
				return 405;
			if (locExt.getGet() == false)
			return 405;
		}
		else
			return 405;
	}
	std::string filesuffix = "";
	if (path.find_last_of(".") != std::string::npos && path.find_last_of(".") > path.find_last_of("/"))
		filesuffix = path.substr(path.find_last_of("."));
	Server server = this->_request.getConfig().getServer(this->_request.getConfig().getPortHost(this->_request.getHeaders()["Host"]));
	if (!filesuffix.empty() && !server.getCgiPath(filesuffix).empty())
	{
		CGI cgi(this->_request);
		int status = cgi.execute();
		this->_response = cgi.getResponse();
		return status;
	}
	if (_isFile(path) == 1)
		return (_getRequestIndex());
	else if (_isFile(path) != 0)
		return _isFile(path);
	file.open(path.c_str());
	if (file.is_open())
		ret << file.rdbuf();
	else 
		return 404;
	file.close();
	this->_response = ret.str();
	return 200;
}

int Response::_deleteRequest()
{
	std::string path = _createPath();
	Location location = _getLocation();
	if (location.getRealLocation() == false)
		return 400;
	if (location.getDelete() == false)
	{
		if (this->_request.getUri().find(".") != std::string::npos)
		{
			Location locExt = _getExtensionLocation();
			if (locExt.getRealLocation() == false)
				return 405;
			if (locExt.getDelete() == false)
			return 405;
		}
		else
			return 405;
	}
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

// Function to generate a unique file name
std::string generateUniqueFileName(const std::string& extension) {
    std::ostringstream oss;
    std::time_t t = std::time(NULL);
    oss << t << "_" << rand() << extension;
    return oss.str();
}

int Response::_pushRequest()
{
	Location location = _getLocation();
	if (location.getRealLocation() == false)
		return 400;
	if (location.getPost() == false)
	{
		if (this->_request.getUri().find(".") != std::string::npos)
		{
			Location locExt = _getExtensionLocation();
			if (locExt.getRealLocation() == false)
				return 405;
			if (locExt.getPost() == false)
			return 405;
		}
		else
			return 405;
	}
	std::string filesuffix;
	if (this->_request.getUri().find_last_of(".") > this->_request.getUri().find_last_of("/"))
		filesuffix = this->_request.getUri().substr(this->_request.getUri().find_last_of("."));

	std::string host = this->_request.getHeaders()["Host"];
	int port = this->_request.getConfig().getPortHost(host);
	Server server = this->_request.getConfig().getServer(port);
	if (!filesuffix.empty() && !server.getCgiPath(filesuffix).empty())
	{
		if ((int)this->_request.getBody().size() > this->_request.getConfig().getClientMaxBodySize())
			return 413;
		CGI cgi(this->_request);
		int status = cgi.execute();
		if (status != 200)
			return status;
		std::stringstream iss(cgi.getResponse());
		if ((int)cgi.getResponse().size() > this->_request.getConfig().getClientMaxBodySize())
			return 413;
		int statusCgi, length;
		std::string line, httpVersion, responseFlag, contentType, contentLength, body, contentDisposition, filename;
		if (std::getline(iss, line))
		{
			std::stringstream firstLine(line);
			firstLine >> httpVersion >> statusCgi >> responseFlag;
			if (statusCgi != 200)
				return statusCgi;
		}
		if (std::getline(iss, line) && (line.find("Content-Type:") != std::string::npos))
			contentType = line.substr(line.find("Content-Type:") + 13);
		if (std::getline(iss, line) && (line.find("Content-Length:") != std::string::npos))
		{
			contentLength = line.substr(line.find("Content-Length:") + 15);
			std::stringstream content(contentLength);
			content >> length;
		}
		if (length == 0 || length > this->_request.getConfig().getClientMaxBodySize())
			return 413;
		if (std::getline(iss, line) && (line.find("Content-Disposition:") + 20) != std::string::npos)
		{
			contentDisposition = line.substr(line.find("Content-Disposition") + 20);
			if (contentDisposition.find("filename") != std::string::npos)
				filename = contentDisposition.substr(contentDisposition.find("filename") + 9);
			if (filename[0] == '"' && filename[filename.size() - 1] == '"')
					filename = filename.substr(1, filename.size() - 2);
		}
		while (std::getline(iss, line))
			body += line += "\r\n";
		
		if ((int)body.size() > this->_request.getConfig().getClientMaxBodySize())
			return 413;
		if (contentType.find("html") != std::string::npos)
		{
			this->_response = body;
			return 200;
		}
		else if (contentType.find("text") != std::string::npos)
		{
			if (_makeFile(filename, "text", body) == 0)
				return 500;
		}
		else if (contentType.find("image") != std::string::npos || contentType.find("application") != std::string::npos)
		{
			if (_makeFile(filename, "binary", body) == 0)
				return 500;
		}
		else
			return 405;
	}
	std::map<std::string, std::string> headers = this->_request.getHeaders();
	if (headers.find("Content-Type") == headers.end() || headers["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		std::string contentType = headers["Content-Type"];
		size_t boundaryPos = contentType.find("boundary=");
		if (boundaryPos == std::string::npos)
			return 400;
		std::string boundary = "--" + contentType.substr(boundaryPos + 9);
		if (this->_request.getBody().empty())
			return 400;
		if ((int)this->_request.getBody().size() > this->_request.getConfig().getClientMaxBodySize())
			return 413;
		std::istringstream body(this->_request.getBody());
		return (_handlePush(body, boundary));
	}
	else
	{
		if (headers.find("Content-Type") == headers.end() || this->_request.getBody().empty())
			return 400;
		std::string contentType = headers["Content-Type"];
		std::string body = this->_request.getBody();

		std::string filename;
		if (contentType.find("text") != std::string::npos)
			filename = generateUniqueFileName(".txt");
		else if (contentType.find("image") != std::string::npos)
			filename = generateUniqueFileName(".img");
		else if (contentType.find("application") != std::string::npos)
			filename = generateUniqueFileName(".bin");
		else
			return 415;
		
		std::string fullpath = server.getRoot();
		if (location.getAlias().empty())
			fullpath += this->_request.getUri().substr(0, this->_request.getUri().find_last_of("/"));
		else
			fullpath += location.getAlias();
		
		fullpath += filename;
		
		char absPath[PATH_MAX];
		realpath(fullpath.c_str(), absPath);
		if (absPath == NULL)
			return 500;

		if ((int)body.size() > this->_request.getConfig().getClientMaxBodySize())
			return 413;
		if (_makeFile(absPath, contentType, body) == 0)
			return 500;
		return 201;
	}
}

Location Response::_getExtensionLocation()
{
	std::string fileExtension = this->_request.getUri();
	if (fileExtension.find_last_of(".") != std::string::npos)
		fileExtension = fileExtension.substr(fileExtension.find_last_of("."));
	else
	{
		Location location;
		return location;
	}
	Config config = this->_request.getConfig();
	int port = config.getPortHost(this->_request.getHeaders()["Host"]);
	Server server = config.getServer(port);
	Location location = server.getLocation(fileExtension);
	return location;
}
