#include "../include/Config.hpp"
#include <iostream>

/************/
/*  CONFIG  */
/************/
int Config::getPortHost(std::string host) const
{
	if (this->_portHost.find(host) != this->_portHost.end())
		return (this->_portHost.at(host));
	return (0);
}

std::map<std::string, int> Config::getPortHosts() const
{
	return this->_portHost;
}

int Config::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

int Config::getDefault() const
{
	return _default;
}

Server Config::getServer(int serverPort) const
{
	if (_server.find(serverPort) != _server.end())
		return _server.at(serverPort);
	return _server.at(_default);
}

std::string Config::getErrorPage(int errorCode) const
{
	if (_errorPages.find(errorCode) != _errorPages.end())
		return _errorPages.at(errorCode);
	return "";
}

std::map<int, Server> Config::getServers() const
{
	return this->_server;
}

std::map<int, std::string> Config::getErrorPages() const
{
	return this->_errorPages;
}

int *Config::getPorts() const
{
	return this->_ports;
}

int Config::getTotalports() const
{
	return this->_totalports;
}

/************/
/*  SERVER  */
/************/
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

std::map<std::string, std::string> Server::getCgiPaths() const
{
	return this->_cgiPath;
}

std::map<std::string, Location> Server::getLocations() const
{
	return this->_location;
}

/**************/
/*  LOCATION  */
/**************/

int Location::getHttp() const
{
	return _http;
}

int Location::getHttps() const
{
	return _https;
}

bool Location::getAutoindex() const
{
	return _autoindex;
}

bool Location::getRealLocation() const
{
	return _realLocation;
}

bool Location::getGet() const
{
	return _get;
}

bool Location::getDelete() const
{
	return _delete;
}

bool Location::getPost() const
{
	return _post;
}

std::string Location::getIndex() const
{
	return _index;
}

std::string Location::getAlias() const
{
	return _alias;
}
