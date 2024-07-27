#include "../include/CGI.hpp"

CGI::CGI()
{
}

CGI::~CGI()
{
}

CGI::CGI(Request request)
{
    this->_request = request;
}

int CGI::execute()
{
    std::string fileSuffix = this->_request.getUri().substr(this->_request.getUri().find_last_of("."));
    int portServer = this->_request.getConfig().getPortHost(this->_request.getHeaders()["Host"]);
    if (portServer == 0)
        return 400;
    Server server = this->_request.getConfig().getServer(portServer);
    std::string cgiPath = server.getCgiPath(fileSuffix);
    std::string filePath = server.getRoot() + this->_request.getUri();
    if (cgiPath.empty())
        return 400;

    char cwd[100];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 500;

    char **env = _buildEnv(cgiPath, filePath);
    if (env == NULL)
        return 500;
    
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return 500;
    
    pid_t pid = fork();

    if (pid == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return 500;
    }
    else if (pid == 0)
    {
        // Child process
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            close(pipefd[1]);
            _exit(500);
        }
        close(pipefd[1]);

        char *const execArgs[] = {NULL};
        execve(cgiPath.c_str(), execArgs, env);
        _exit(500);
    }
    else
    {
        close(pipefd[1]);
        int status;
        waitpid(pid, &status, 0);

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            close(pipefd[0]);
            return 500;
        }

        char buff[CGI_BUFF];
        ssize_t readBytes;
        while ((readBytes = read(pipefd[0], buff, CGI_BUFF - 1)) > 0)
        {
            buff[readBytes] = '\0';
            this->_response += buff;
        }
        close(pipefd[0]);
    }

    if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1)
        return 500;
    
    for (int i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
    env = NULL;

    return 200;
}

std::string CGI::getResponse()
{
    return this->_response;
}

char **CGI::_buildEnv(std::string filePath, std::string cgiPath)
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

        char **env = new char *[envVars.size() + 1];
        env[envVars.size()] = NULL;

        int i = 0;
        for (std::map<std::string, std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it)
        {
            std::string tmp = it->first + "=" + it->second;
            env[i] = new char[tmp.size() + 1];
            std::strcpy(env[i], tmp.c_str());
            i++;
        }
        return env;
    }
    catch (const std::exception &e)
    {
        return NULL;
    }
}