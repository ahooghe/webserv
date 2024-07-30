#include "../include/Config.hpp"
#include <iostream>

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
		if (line.find_first_not_of(" \t") == std::string::npos)
			continue;
		if (line.find("location ") != std::string::npos)
		{
			atLeastOneLocation = true;
			std::string locationBlock = "";
			std::string locID = line.substr(line.find("location ") + 9);
			if (locID.find_first_not_of(" \t") != std::string::npos)
				locID = locID.substr(locID.find_first_not_of(" \t"));
			if (locID.find_first_of(" \t") != std::string::npos)
				locID = locID.substr(0, locID.find_first_of(" \t"));
			if (locID.length() > 1 && locID[locID.length() - 1] == '/')
				locID = locID.substr(0, locID.length() - 1);
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
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line != "}")
				throw ClosingBracketException();
			_location[locID].initLocation(locationBlock);
		}
		else if (line.find("server_name ") != std::string::npos)
		{
			line = line.substr(line.find("server_name ") + 12);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_serverName = line;
		}
		else if (line.find("root ") != std::string::npos)
		{
			line = line.substr(line.find("root ") + 5);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_root = line;
		}
		else if (line.find("index ") != std::string::npos)
		{
			line = line.substr(line.find("index ") + 6);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionSerException();
			_index = line;
		}
		else if (line.find("cgi_path ") != std::string::npos)
		{
			line = line.substr(line.find("cgi_path ") + 9);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			std::string extension = "";
			std::string path = "";
			size_t extensionPos = line.find_first_of(" \t");
			if (extensionPos != std::string::npos)
			{
				extension = line.substr(0, extensionPos);
				path = line.substr(extensionPos + 1);
			}
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

Server &Server::operator=(const Server &src)
{
	this->_serverName = src.getServerName();
	this->_root = src.getRoot();
	this->_index = src.getIndex();
	this->_cgiPath = src.getCgiPaths();
	this->_location = src.getLocations();
	return (*this);
}