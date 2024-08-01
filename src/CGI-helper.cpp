#include "../include/CGI.hpp"

std::string CGI::getResponse()
{
    return this->_response;
}

char **CGI::_buildEnv(std::string filePath)
{
    try
    {
        std::map<std::string, std::string> envVars;

        envVars["REQUEST_METHOD"] = this->_request.getMethod();
        envVars["CONTENT_TYPE"] = this->_request.getHeaders()["Content-Type"];
        envVars["CONTENT_LENGTH"] = this->_request.getHeaders()["Content-Length"];
        envVars["SCRIPT_NAME"] = filePath;
        envVars["SCRIPT_FILENAME"] = filePath;
        envVars["REQUEST_URI"] = this->_request.getUri();
        envVars["HTTP_COOKIE"] = this->_request.getHeaders()["Cookie"];
        envVars["GATEWAY_INTERFACE"] = "CGI/1.1";
        envVars["SERVER_PROTOCOL"] = "HTTP/1.1";

        envVars["SERVER_NAME"] = this->_request.getHeaders()["Host"];
        envVars["SERVER_PORT"] = this->_request.getConfig().getPortHost(envVars["SERVER_NAME"]);

        // Optional variables
        envVars["PATH_INFO"] = this->_request.getUri();
        envVars["REDIRECT_STATUS"] = "200";

	int amountofX_POST = 0;
	std::string x_POST;
	std::map<int, std::string> x_post;
	if (this->_request.getMethod() == "POST")
	{
		x_POST = this->_request.getHeaders()["X_POST"];
		while (!x_POST.empty())
		{
			if (x_POST.find_first_of("+") != std::string::npos)
			{
				x_post[amountofX_POST] = x_POST.substr(0, x_POST.find_first_of("+"));
				x_POST = x_POST.substr(x_POST.find_first_of("+"));
				amountofX_POST++;
			}
			else
			{
				x_post[amountofX_POST] = x_POST.substr(0, x_POST.find_first_of("+"));
				x_POST = x_POST.substr(x_POST.find_first_of("+"));
				amountofX_POST++;
			}
		}
	}

        char **env = new char *[envVars.size() + amountofX_POST + 1];
        env[envVars.size()] = NULL;

        int i = 0;
        for (std::map<std::string, std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it)
        {
            std::string tmp = it->first + "=" + it->second;
            env[i] = new char[tmp.size() + 1];
            std::strcpy(env[i], tmp.c_str());
            i++;
        }
	for (int j = 0; j < amountofX_POST; j++)
	{
		std::string tmp = "X_POST=" + x_post[j];
		env[i + j] = new char[tmp.size() + 1];
		std::strcpy(env[i + j], tmp.c_str());
		j++;
	}
        return env;
    }
    catch (const std::exception &e)
    {
        return NULL;
    }
}
