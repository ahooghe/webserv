#include "../include/Config.hpp"

Location &Location::operator=(const Location &src)
{
	this->_http = src.getHttp();
	this->_https = src.getHttps();
	this->_autoindex = src.getAutoindex();
	this->_realLocation = src.getRealLocation();
	this->_index = src.getIndex();
	this->_alias = src.getAlias();
	this->_get = src.getGet();
	this->_delete = src.getDelete();
	this->_post = src.getPost();
	return (*this);
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

Config &Config::operator=(const Config &src)
{
	if (this != &src)
	{
		if (this->_ports != NULL)
		{
			delete[] this->_ports;
			this->_ports = NULL;
		}

		this->_server = src.getServers();
		this->_clientMaxBodySize = src.getClientMaxBodySize();
		this->_default = src.getDefault();
		this->_errorPages = src.getErrorPages();
		this->_portHost = src.getPortHosts();
		this->_totalports = src.getTotalports();
		this->_ports = new int[src.getTotalports()];
		for (int i = 0; i < src.getTotalports(); i++)
			this->_ports[i] = src.getPorts()[i];
	}
	return (*this);
}
