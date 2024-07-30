#include "../include/Config.hpp"

void Config::errCheck()
{
	//If 0, there's no server block in the config
	if (_default == 0)
		throw NoServerException();

	//Checking whether all the optional variables in the config file were setup. If not, applying default value
	if (_clientMaxBodySize == 0)
		_clientMaxBodySize = 1000000;
	if (_errorPages.find(400) == _errorPages.end())
		_errorPages[400] = "default_files/err_pages/400.html";
	if (_errorPages.find(401) == _errorPages.end())
		_errorPages[401] = "default_files/err_pages/401.html";
	if (_errorPages.find(403) == _errorPages.end())
		_errorPages[403] = "default_files/err_pages/403.html";
	if (_errorPages.find(404) == _errorPages.end())
		_errorPages[404] = "default_files/err_pages/404.html";
	if (_errorPages.find(405) == _errorPages.end())
		_errorPages[405] = "default_files/err_pages/405.html";
	if (_errorPages.find(413) == _errorPages.end())
		_errorPages[413] = "default_files/err_pages/413.html";
	if (_errorPages.find(500) == _errorPages.end())
		_errorPages[500] = "default_files/err_pages/500.html";
	if (_errorPages.find(501) == _errorPages.end())
		_errorPages[501] = "default_files/err_pages/500.html";
	if (_errorPages.find(502) == _errorPages.end())
		_errorPages[502] = "default_files/err_pages/502.html";
	if (_errorPages.find(503) == _errorPages.end())
		_errorPages[503] = "default_files/err_pages/503.html";
	if (_errorPages.find(504) == _errorPages.end())
		_errorPages[504] = "default_files/err_pages/504.html";
}

void Config::checkFormat(const char *path)
{
	std::ifstream file(path);
	if (!file.is_open())
		throw FileOpenException();
	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty())
			continue;
		if (line.find_first_not_of(" \t") == std::string::npos || line.find_last_not_of(" \t") == std::string::npos)
			continue;
		line = line.substr(line.find_first_not_of(" \t"));
		line = line.substr(0, line.find_last_not_of(" \t") + 1);
		if (line.empty())
			continue;
		size_t semiColonPos = line.find(';');
		if (line.find("server ") != std::string::npos || line.find("location ") != std::string::npos)
			continue;
		if (line.find("{") != std::string::npos)
		{
			if (line != "{")
				throw BracketException();
		}
		else if (line.find("}") != std::string::npos)
		{
			if (line != "}")
				throw BracketException();
		}
		else if (semiColonPos == std::string::npos || semiColonPos == 0)
			throw SemiColonException();
	}
	file.close();
}

void Config::_checkErrorPages()
{
	for (std::map<int, std::string>::iterator it = this->_errorPages.begin(); it != this->_errorPages.end(); ++it)
	{
		std::string path = it->second;
		char resolvedPath[PATH_MAX];
		if (realpath(path.c_str(), resolvedPath) == NULL)
			throw ErrorFileException();
		else if (access(path.c_str(), F_OK) != 0)
			throw ErrorFileException();
		struct stat path_stat;
		if (stat(path.c_str(), &path_stat) == 0)
		{
			if (S_ISDIR(path_stat.st_mode))
				throw ErrorFileException();
			else if (S_ISLNK(path_stat.st_mode))
				throw ErrorFileException();
			else if (S_ISSOCK(path_stat.st_mode))
				throw ErrorFileException();
			else if (S_ISFIFO(path_stat.st_mode))
				throw ErrorFileException();
			else if (S_ISCHR(path_stat.st_mode) || S_ISBLK(path_stat.st_mode))
				throw ErrorFileException();
		}

		std::ifstream file;
		file.open(path.c_str());
		if (!file.is_open())
			throw ErrorFileException();
		file.close();
	}
}