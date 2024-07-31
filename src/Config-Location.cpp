#include "../include/Config.hpp"

Location::Location()
{
	_http = true;
	_https = false;
	_autoindex = false;
	_realLocation = true;
	_httpcounter = 0;
	_get = true;
	_post = true;
	_delete = true;
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
		else if (line.find("limit_except ") != std::string::npos)
		{
			line = line.substr(13);
			std::istringstream iss(line);
			std::string var1, var2, var3;
			iss >> var1 >> var2 >> var3;
			this->_get = false;
			if (var1 == "GET" || var2 == "GET" || var3 == "GET")
				this->_get = true;
			this->_delete = false;
			if (var1 == "DELETE" || var2 == "DELETE" || var3 == "DELETE")
				this->_delete = true;
			this->_post = false;
			if (var1 == "POST" || var2 == "POST" || var3 == "POST")
				this->_post = true;
		}
		else
			throw FormatException();
	}
	if (this->_httpcounter > 1)
		throw FormatException();
}