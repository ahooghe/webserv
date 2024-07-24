#include "Config.hpp"
#include "Request.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <string>

class Response
{
	private:
		Request _request;
		std::string _response;

		int			_getRequest();
		int			_deleteRequest();
		int			_pushRequest();
		std::string	_createPath();
		Location	_getLocation();
		int			_isFile(std::string path);
		int			_getRequestIndex();
		int			_makeFile(const std::string &filename, const std::string &type, const std::string &content);

		Response();
	public:
		Response(Request request);
		~Response();

		int			handleRequest();
		std::string	getResponse();
};