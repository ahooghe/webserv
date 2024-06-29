#include "../include/Config.hpp"

Location::Location()
{
	_http = 0;
	_https = 0;
	_autoindex = false;
	_realLocation = true;
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
		std::cout << line << std::endl;
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
			line = line.substr(line.find("listen_http ") + 12);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			std::istringstream ss(line);
			char extra;
			if (!(ss >> _http) || _http < 0 || ss >> extra)
				throw FormatException();
		}
		else if (line.find("listen_https") != std::string::npos)
		{
			line = line.substr(line.find("listen_https ") + 13);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			std::istringstream ss(line);
			char extra;
			if (!(ss >> _https) || _https < 0 || ss >> extra)
				throw FormatException();
		}
		else if (line.find("auto_index ") != std::string::npos)
		{
			line = line.substr(line.find("auto_index ") + 11);
			line = line.substr(line.find_first_not_of(" \t"));
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
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionLocException();
			_index = line;
		}
		else if (line.find("alias ") != std::string::npos)
		{
			line = line.substr(line.find("alias ") + 6);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			if (line.find(" ") != std::string::npos || line.find("\t") != std::string::npos)
				throw MultipleDefinitionLocException();
			_alias = line;
		}
		else
			throw FormatException();
	}
}

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

std::string Location::getIndex() const
{
	return _index;
}

std::string Location::getAlias() const
{
	return _alias;
}