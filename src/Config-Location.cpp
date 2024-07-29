#include "../include/Config.hpp"

Location::Location()
{
	_http = true;
	_https = false;
	_autoindex = false;
	_realLocation = true;
	_httpcounter = 0;
}

Location::Location(bool fake)
{
	_realLocation = fake;
}

Location::~Location()
{
}

void Location::initLocation(std::string locationBlock)
{
	std::istringstream location(locationBlock);
	std::string line;

	while(std::getline(location, line))
	{
		size_t poundPos = line.find('#');
		if (poundPos != std::string::npos)
			line = line.substr(0, (poundPos));
		size_t semiColonPos = line.find(';');
		if (semiColonPos != std::string::npos)
			line = line.substr(0, semiColonPos);
		if (line.find_first_not_of(" \t") == std::string::npos || line.find_last_not_of(" \t") == std::string::npos)
			continue;
		line = line.substr(line.find_first_not_of(" \t"));
		line = line.substr(0, line.find_last_not_of(" \t") + 1);

		if (line.find("listen_http ") != std::string::npos)
		{
			_httpcounter++;
		}
		else if (line.find("listen_https") != std::string::npos)
		{
			this->_https = true;
			this->_http = false;
			_httpcounter++;
		}
		else if (line.find("auto_index ") != std::string::npos)
		{
			line = line.substr(line.find("auto_index ") + 11);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line == "on")
				_autoindex = true;
			else if (line == "off")
				_autoindex = false;
			else
				throw BadInputException();
		}
		else if (line.find("index ") != std::string::npos)
		{
			line = line.substr(line.find("index ") + 6);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionLocException();
			_index = line;
		}
		else if (line.find("alias ") != std::string::npos)
		{
			line = line.substr(line.find("alias ") + 6);
			if (line.find_first_not_of(" \t") != std::string::npos)
				line = line.substr(line.find_first_not_of(" \t"));
			if (line.find_last_not_of(" \t") != std::string::npos)
				line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionLocException();
			_alias = line;
		}
		else
			throw FormatException();
	}
	if (this->_httpcounter > 1)
		throw FormatException();
}
Location &Location::operator=(const Location &src)
{
	this->_http = src.getHttp();
	this->_https = src.getHttps();
	this->_autoindex = src.getAutoindex();
	this->_realLocation = src.getRealLocation();
	this->_index = src.getIndex();
	this->_alias = src.getAlias();
	return (*this);
}