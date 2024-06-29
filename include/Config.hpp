#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "Error.hpp"

class Location
{
private:
	int			_http;
	int			_https;
	bool		_autoindex;
	bool		_realLocation;
	std::string	_index;
	std::string	_alias;

public:
	Location();
	Location(bool real);
	~Location();

	void		initLocation(std::string locationBlock);

	int			getHttp() const;
	int			getHttps() const;
	bool		getAutoindex() const;
	bool		getRealLocation() const;
	std::string	getIndex() const;
	std::string	getAlias() const;

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

};

class Config
{
private:
	int							_clientMaxBodySize;
	int							_default;
	std::map<int, Server>		_server;
	std::map<int, std::string>	_errorPages;

public:
	Config();
	~Config();

	void						checkFormat(const char *path);
	void						errCheck();

	void						initConfig(const char *path);

	int							getClientMaxBodySize() const;
	int							getDefault() const;
	Server						getServer(int server) const;
	std::string					getErrorPage(int errorCode) const;

	
};