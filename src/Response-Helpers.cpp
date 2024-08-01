#include "../include/Response.hpp"

int Response::_makeFile(const std::string &filename, const std::string &type, const std::string &content)
{
	std::ofstream file;
	if (type.find("text") != std::string::npos)
	{
		file.open(filename.c_str());
	}
	else
	{
		file.open(filename.c_str(), std::ios::binary);
	}

	if (!file.is_open())
		return 0;

	if (type.find("text") != std::string::npos)
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
		if (server.getLocation(uriPath).getRealLocation() == false)
		{
			size_t slashPos = uriPath.find_last_of("/");
			if (slashPos == 0)
				break;
			if (slashPos != std::string::npos)
				uriPath = uriPath.substr(0, slashPos);
		}
		else
		{
			location = server.getLocation(uriPath);
			break;
		}
	}
	std::string requestedFile = server.getRoot();
	std::string uriWithoutLoc = this->_request.getUri().substr(uriPath.length());
	if (!uriWithoutLoc.empty())
	{
		if (uriWithoutLoc.find(".") != std::string::npos)
		{
			if (location.getAlias().empty())
				requestedFile += uriPath + uriWithoutLoc;
			else
				requestedFile += location.getAlias() + "/" + uriWithoutLoc;
		}
		else
		{
			if (!location.getAlias().empty())
				uriPath = location.getAlias();
			int isExt = -1;
			requestedFile += uriPath + uriWithoutLoc;
			if (requestedFile.find_last_of(".") != std::string::npos && requestedFile.find_last_of("/") != std::string::npos)
				isExt = requestedFile.find_last_of(".") - requestedFile.find_last_of("/");
			if (location.getIndex().empty() && isExt < 0)
				requestedFile += "/" + server.getIndex();
			else if (!location.getIndex().empty() && isExt < 0)
				requestedFile += "/" + location.getIndex();
		}
	}
	else if (location.getAutoindex() == true)
	{
		requestedFile += uriPath;
	}
	else
	{
		if (!location.getAlias().empty())
			uriPath = location.getAlias();
		int isExt = -1;
		requestedFile += uriPath;
		if (requestedFile.find_last_of(".") != std::string::npos && requestedFile.find_last_of("/") != std::string::npos)
			isExt = requestedFile.find_last_of(".") - requestedFile.find_last_of("/");
		if (location.getIndex().empty() && isExt < 0)
			requestedFile += uriPath + "/" + server.getIndex();
		else if (!location.getIndex().empty() && isExt < 0)
			requestedFile += uriPath + "/" + location.getIndex();
	}
	return (requestedFile);
}

int Response::_isFile(std::string path)
{
	
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
	if (access(path.c_str(), F_OK) != 0)
		return 404;

	char resolvedPath[PATH_MAX];
	if (realpath(path.c_str(), resolvedPath) == NULL)
	{
		return 400;
	}
	
	std::ifstream file;
	file.open(path.c_str());
	if (!file.is_open())
		return 403;
	file.close();
	return 0;
}

int Response::_handlePush(std::istringstream& body, std::string boundary)
{
	bool inFile = false;
	std::string line, filename, content, type;
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
				filename = line.substr(pos + 9);
				if (filename[0] == '"' && filename[filename.size() - 1] == '"')
					filename = filename.substr(1, filename.size() - 2);
			}
		}
		else if (line.find("Content-Type: ") != std::string::npos)
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
		return 400;
	return 201;
}