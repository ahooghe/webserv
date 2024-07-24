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
    std::string line, filename, content, type;
    bool inFile = false;

    while (std::getline(body, line))
    {
        if (line == boundary)
        {
            if (inFile && !filename.empty() && !type.empty() && !content.empty())
            {
                if (_makeFile(filename, type, content) == 0)
                    return 500;
                filename.clear();
                type.clear();
                content.clear();
            }
            inFile = false;
        }
        else if (line.find("Content-Disposition:") != std::string::npos)
        {
            size_t pos = line.find("filename=");
            if (pos != std::string::npos)
            {
                filename = line.substr(pos + 10);
                if (filename[0] == '"' && filename[filename.size() - 1] == '"')
                    filename = filename.substr(1, filename.size() - 2);
            }
        }
        else if (line.find("Content-Type:") != std::string::npos)
        {
            type = line.substr(line.find("Content-Type: ") + 14);
        }
        else if (line.empty())
        {
            inFile = true;
        }
        else if (inFile)
        {
            content += line + "\n";
        }
    }

    if (!filename.empty() && !type.empty() && !content.empty())
    {
        if (_makeFile(filename, type, content) == 0)
            return 500;
    }
    else
    {
        return 400;
    }

    return 201;
}

int Response::_makeFile(const std::string &filename, const std::string &type, const std::string &content)
{
    std::ofstream file;
    if (type == "text/plain")
    {
        file.open(filename.c_str());
    }
    else
    {
        file.open(filename.c_str(), std::ios::binary);
    }

    if (!file.is_open())
        return 0;

    if (type == "text/plain")
    {
        file << content;
    }
    else
    {
        file.write(content.data(), content.size());
    }

    file.close();
    return 1;
}

std::string Response::_createPath()
{
	std::string host = this->_request.getHeaders()["Host"];
	if (host.empty())
		return "";
	int hostPort = this->_request.getConfig().getPortHost(host);
	if (hostPort == 0)
		return ("");
	Server server = this->_request.getConfig().getServer(hostPort);
	Location location;
	std::string uriPath = this->_request.getUri();
	while (!uriPath.empty())
	{
		std::string oldUri = uriPath;
		if (server.getLocation(uriPath).getRealLocation() == 0)
			uriPath = uriPath.substr(0, uriPath.find("/") + 1);
		else
		{
			location = server.getLocation(uriPath);
			break;
		}
	}
	if (location.getRealLocation() == false)
		return ("");
	std::string requestedFile;
	if (location.getAlias().empty())
		requestedFile = server.getRoot() + this->_request.getUri();
	else
		requestedFile = server.getRoot() + location.getAlias() + this->_request.getUri().substr(this->_request.getUri().find_last_of("/"));
	return (requestedFile);
}

int Response::_getRequestIndex()
{
	std::string path = _createPath();
	DIR *dir;
	struct dirent *entry;
	std::vector<std::string> entries;

	if ((dir = opendir(path.c_str())) == NULL)
		return 404;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name != "." && name != "..")
			entries.push_back(name);
	}
	closedir(dir);
	std::sort(entries.begin(), entries.end());
	std::stringstream html;
	html << "<html><head><title>Index of " << path << "</title></head><body>";
	html << "<h1>Index of " << path << "</h1><ul>";

	for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); it++)
		html << "<li><a href=\"" << *it << "\">" << *it << "</a></li>";
	html << "</ul></body></html>";

	this->_response = html.str();
	return 200;
}

Location Response::_getLocation()
{
	std::string host = this->_request.getHeaders()["Host"];
	if (host.empty())
		return "";
	int hostPort = this->_request.getConfig().getPortHost(host);
	if (hostPort == 0)
		return ("");
	Server server = this->_request.getConfig().getServer(hostPort);
	Location location;
	std::string uriPath = this->_request.getUri();
	while (!uriPath.empty())
	{
		std::string oldUri = uriPath;
		if (server.getLocation(uriPath).getRealLocation() == 0)
			uriPath = uriPath.substr(0, uriPath.find("/") + 1);
		else
		{
			location = server.getLocation(uriPath);
			break;
		}
	}
	return location;
}

int Response::_isFile(std::string path)
{
    char resolvedPath[PATH_MAX];
    if (realpath(path.c_str(), resolvedPath) == NULL)
        return 400;
	else if (access(path.c_str(), F_OK) != 0)
		return 404;
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) == 0)
	{
		if (S_ISDIR(path_stat.st_mode))
			return 1;
		else if (S_ISLNK(path_stat.st_mode))
			return 403;
		else if (S_ISSOCK(path_stat.st_mode))
			return 403;
		else if (S_ISFIFO(path_stat.st_mode))
			return 403;
		else if (S_ISCHR(path_stat.st_mode) || S_ISBLK(path_stat.st_mode))
			return 403;
	}

	std::ifstream file;
	file.open(path.c_str());
	if (!file.is_open())
		return 403;
	return 0;
}