#include "../include/Config.hpp"

Server::Server()
{
}

Server::~Server()
{
}

void Server::initServer(std::string serverBlock)
{
	std::string line;
	std::istringstream server(serverBlock);
	bool atLeastOneLocation = false;
	while(std::getline(server, line))
	{
		if (line.find("location ") != std::string::npos)
		{
			atLeastOneLocation = true;
			std::string locationBlock = "";
			std::string locID = line.substr(line.find("location ") + 9);
			locID = locID.substr(locID.find_first_not_of(" \t"));
			locID = locID.substr(0, locID.find_first_of(" \t"));
			if (std::getline(server, line) && line == "{")
				throw OpeningBracketException();
			while (std::getline(server, line))
			{
				if (line.empty())
					continue;
				if (line.find("}") != std::string::npos)
					break;
				locationBlock += line + "\n";
			}
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line != "}")
				throw ClosingBracketException();
			_location[locID].initLocation(locationBlock);
		}
		else if (line.find("server_name ") != std::string::npos)
		{
			line = line.substr(line.find("server_name ") + 12);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_serverName = line;
		}
		else if (line.find("root ") != std::string::npos)
		{
			line = line.substr(line.find("root ") + 5);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_root = line;
		}
		else if (line.find("index ") != std::string::npos)
		{
			line = line.substr(line.find("index ") + 6);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_index = line;
		}
		else if (line.find("cgi_path ") != std::string::npos)
		{
			line = line.substr(line.find("cgi_path ") + 9);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			std::string extension = line.substr(0, line.find_first_of(" \t"));
			std::string path = line.substr(line.find_first_of(" \t") + 1);
			if (path.find(" ") != std::string::npos || path.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_cgiPath[extension] = path;
		}
		else
			throw BadServerFormatException();
	}
	if (!atLeastOneLocation)
		throw NoLocationException();
}

std::string Server::getServerName() const
{
	return _serverName;
}

std::string Server::getRoot() const
{
	return _root;
}

std::string Server::getIndex() const
{
	return _index;
}

std::string Server::getCgiPath(std::string extension) const
{
	if (_cgiPath.find(extension) != _cgiPath.end())
		return _cgiPath.at(extension);
	return "";
}

Location Server::getLocation(std::string path) const
{
	if (_location.find(path) != _location.end())
		return _location.at(path);
	return Location(false);
}