#include "../include/Response.hpp"

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
		{
			if (uriPath.find("/") != std::string::npos)
				uriPath = uriPath.substr(0, uriPath.find("/") + 1);
		}
		else
		{
			location = server.getLocation(uriPath);
			break;
		}
	}
	return location;
}