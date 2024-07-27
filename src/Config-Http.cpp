#include "../include/Config.hpp"

Config::Config()
{
	//1000000 bytes = 1MB
	_clientMaxBodySize = 1000000;
}

Config::~Config()
{
}

void Config::initConfig(const char *path)
{
	std::fstream file(path);
	bool default_server = false;

	if (!file.is_open())
		throw FileOpenException();

	std::string line;
	while (std::getline(file, line))
	{
		//Empty lines are ignored
		if (line.empty())
			continue;
		//Remove comments and check end line syntaxing
		size_t poundPos = line.find('#');
		if (poundPos != std::string::npos)
			line = line.substr(0, (poundPos));

		//Remove leading and trailing whitespaces
		line = line.substr(line.find_first_not_of(" \t"));
		line = line.substr(0, line.find_last_not_of(" \t") + 1);

		//Checks for different parts of file
		if (line.find("client_max_body_size") != std::string::npos)
		{
			size_t semiColonPos = line.find(';');
			if (semiColonPos != std::string::npos)
				line = line.substr(0, semiColonPos);
			
			/*
			Making a substring of the integer part in the config file
			and removing all the spaces in front and behind the value (just in case :p) 
			Then using a string stream conversion to make sure errors are handled properly.
			*/
			std::string value = line.substr(line.find("client_max_body_size") + 20);
			value = value.substr(value.find_first_not_of(" \t"));
			value = value.substr(0, value.find_last_not_of(" \t") + 1);

			std::istringstream iss(value);
			char extra;
			if (!(iss >> _clientMaxBodySize) || _clientMaxBodySize < 0 || iss >> extra)
				throw ClientMaxBodySizeException();
		}
		else if (line.find("error_page") != std::string::npos)
		{
			size_t semiColonPos = line.find(';');
			if (semiColonPos != std::string::npos)
				line = line.substr(0, semiColonPos);
			//Getting the error code and file path seperated
			std::string errorPage = line.substr(line.find("error_page") + 10);
			errorPage = errorPage.substr(errorPage.find_first_not_of(" \t"));
			std::string errorCode = errorPage.substr(0, errorPage.find_first_of(" \t"));
			errorPage = errorPage.substr(errorPage.find_first_of(" \t"));
			errorPage = errorPage.substr(errorPage.find_first_not_of(" \t"));
			std::string errorPath = errorPage.substr(0, errorPage.find_first_of(" \t"));

			//Same conversion method as above
			std::istringstream iss(errorCode);
			char extra;
			int code;
			if (!(iss >> code) || code < 0 || iss >> extra)
				throw ErrorCodeException();

			//Checking if the error code is already defined
			if (_errorPages.find(code) == _errorPages.end())
				_errorPages[code] = errorPath;
			else
				throw DoubleErrorException();
		}
		else if (line.find("server ") != std::string::npos)
		{
			line = line.substr(line.find("server") + 6);
			line = line.substr(line.find_first_not_of(" \t"));
			line = line.substr(0, line.find_last_not_of(" \t") + 1);
			std::istringstream ss(line);
			char extra;
			int port;
			if (!(ss >> port) || port < 0 || ss >> extra)
				throw PortException();

			std::string serverBlock;
			if (std::getline(file, line) && line != "{")
				throw OpeningBracketException();
			while (std::getline(file, line))
			{
				//If brackets is 0, we should be done with the current server block
				if (line.empty())
					continue;
				
				if (line.find("server_name") != std::string::npos)
				{
					std::string host = line.substr(line.find("server_name") + 12);
					host = host.substr(host.find_first_not_of(" \t"));
					host = host.substr(0, host.find_last_not_of(" \t"));
					this->_portHost[host] = port;
					std::cout << host << " = " << port << std::endl;
				}

				//Remove comments and check end line syntaxing
				size_t poundPos = line.find('#');
				if (poundPos != std::string::npos)
					line = line.substr(0, (poundPos));
				size_t semiColonPos = line.find(';');
				if (semiColonPos != std::string::npos)
					line = line.substr(0, semiColonPos);
				line = line.substr(line.find_first_not_of(" \t"));
				line = line.substr(0, line.find_last_not_of(" \t") + 1);

				if (line.find("{") != std::string::npos)
					throw ClosingBracketException();
				if (line.find("location") != std::string::npos)
				{
					serverBlock += line + "\n";
					while (std::getline(file, line))
					{
						if (line.empty())
							continue;
						if (line.find("}") != std::string::npos)
							break;
						if (line.find("location ") != std::string::npos)
							throw ClosingBracketException();
						serverBlock += line + "\n";
					}
				}
				else if (line.find("}") != std::string::npos)
					break;
				serverBlock += line + "\n";
			}
			if (line.find("}") == std::string::npos)
				throw ClosingBracketException();
			if (default_server == false)
			{
				_default  = port;
				default_server = true;
			}
			this->_server[port].initServer(serverBlock);
		}
		else
			throw FormatException();
	}
	file.close();
}

Config &Config::operator=(const Config &src)
{
	this->_server = src.getServers();
	this->_clientMaxBodySize = src.getClientMaxBodySize();
	this->_default = src.getDefault();
	this->_errorPages = src.getErrorPages();
	this->_portHost = src.getPortHosts();
	return (*this);
}


