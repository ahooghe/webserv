#include "Request.hpp"
#include <string>
#include <cstring>
#include <sys/wait.h>

#define CGI_BUFF 4096

class CGI
{
	private:
		Request _request;
		std::string _response;
		
		CGI();
		char **_buildEnv(std::string filePath, std::string cgiPath);
	
	public:
		CGI(Request request);
		~CGI();

		int				execute();
		std::string		getResponse();
};