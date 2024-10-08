#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "Error.hpp"

class Location
{
private:
	bool		_http;
	bool		_https;
	bool		_autoindex;
	bool		_realLocation;
	bool		_get;
	bool		_delete;
	bool		_post;
	std::string	_index;
	std::string	_alias;
	int			_httpcounter;

public:
	Location();
	Location(bool real);
	~Location();

	void		initLocation(std::string locationBlock);

	int			getHttp() const;
	int			getHttps() const;
	bool		getAutoindex() const;
	bool		getRealLocation() const;
	bool		getGet() const;
	bool		getDelete() const;
	bool		getPost() const;
	std::string	getIndex() const;
	std::string	getAlias() const;

	Location &operator=(const Location &src);
};

class Server
{
private:
	std::string							_serverName;
	std::string							_root;
	std::string							_index;
	std::map<std::string, std::string>	_cgiPath;
	std::map<std::string, Location>		_location;

public:
	Server();
	~Server();

	void								initServer(std::string serverBlock);
	
	std::string							getServerName() const;
	std::string							getRoot() const;
	std::string							getIndex() const;
	std::string							getCgiPath(std::string extension) const;
	Location							getLocation(std::string path) const;
	std::map<std::string, std::string>	getCgiPaths() const;
	std::map<std::string, Location>		getLocations() const;

	Server &operator=(const Server &src);
};

class Config
{
private:
	int							_clientMaxBodySize;
	int							_default;
	int							*_ports;
	int							_totalports;
	std::map<int, Server>		_server;
	std::map<int, std::string>	_errorPages;
	std::map<std::string, int>	_portHost;

	void						_checkErrorPages();

public:
	Config();
	~Config();
	Config(const Config &src);

	void						checkFormat(const char *path);
	void						initConfig(const char *path);
	void						errCheck();

	int							getClientMaxBodySize() const;
	int							getDefault() const;
	Server						getServer(int server) const;
	std::string					getErrorPage(int errorCode) const;
	std::map<int, Server>		getServers() const;
	std::map<int, std::string>	getErrorPages() const;
	std::map<std::string, int>	getPortHosts() const;
	int							getPortHost(std::string host) const;
	int							*getPorts() const;
	int							getTotalports() const;

	Config &operator=(const Config &src);
};

#endif