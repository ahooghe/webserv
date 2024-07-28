#ifndef CGI_HPP
#define CGI_HPP

#define CGI_BUFF 4096

#include <sys/wait.h>
#include <cstring>

#include "Request.hpp"

class CGI
{
	private:
		Request _request;
		std::string _response;
		
		CGI();
		char **_buildEnv(std::string filePath);
	
	public:
		CGI(Request request);
		~CGI();

		int				execute();
		std::string		getResponse();
};

#endif