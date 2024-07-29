#include "../include/Config.hpp"

Config::Config()
{
	_clientMaxBodySize = 1000000;
}

Config::~Config()
{
	if (this->_totalports)
		delete[] this->_ports;

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
		if (line.empty())
			continue;
		size_t poundPos = line.find('#');
		if (poundPos != std::string::npos)
			line = line.substr(0, (poundPos));

		line = line.substr(line.find_first_not_of(" \t"));
		line = line.substr(0, line.find_last_not_of(" \t") + 1);

		if (line.find("client_max_body_size") != std::string::npos)
		{
			size_t semiColonPos = line.find(';');
			if (semiColonPos != std::string::npos)
				line = line.substr(0, semiColonPos);
			
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
			std::string errorPage = line.substr(line.find("error_page") + 10);
			errorPage = errorPage.substr(errorPage.find_first_not_of(" \t"));
			std::string errorCode = errorPage.substr(0, errorPage.find_first_of(" \t"));
			errorPage = errorPage.substr(errorPage.find_first_of(" \t"));
			errorPage = errorPage.substr(errorPage.find_first_not_of(" \t"));
			std::string errorPath = errorPage.substr(0, errorPage.find_first_of(" \t"));

			std::istringstream iss(errorCode);
			char extra;
			int code;
			if (!(iss >> code) || code < 0 || iss >> extra)
				throw ErrorCodeException();

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
				if (line.empty())
					continue;
				
				if (line.find("server_name") != std::string::npos)
				{
					std::string host = line.substr(line.find("server_name") + 12);
					host = host.substr(host.find_first_not_of(" \t"));
					host = host.substr(0, host.find_last_not_of(" \t"));
					this->_portHost[host] = port;
				}

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

	this->_ports = new int[this->_portHost.size()];
	int i = 0;
	for (std::map<std::string, int>::iterator it = this->_portHost.begin(); it != this->_portHost.end(); ++it)
	{
		this->_ports[i] = it->second;
		i++;
	}
	this->_totalports = i;
}

Config &Config::operator=(const Config &src)
{
	this->_server = src.getServers();
	this->_clientMaxBodySize = src.getClientMaxBodySize();
	this->_default = src.getDefault();
	this->_errorPages = src.getErrorPages();
	this->_portHost = src.getPortHosts();
	this->_totalports = src.getTotalports();
	this->_ports = src.getPorts();
	return (*this);
}


