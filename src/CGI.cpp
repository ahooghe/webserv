#include "../include/CGI.hpp"
#include <iostream>

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
	std::string uri = this->_request.getUri();
	size_t dotIndex = uri.find_last_of(".");
	if (dotIndex == std::string::npos)
		return 500;
	std::string fileSuffix = uri.substr(dotIndex);
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
	char **env = _buildEnv(filePath);
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
		close(pipefd[0]);

		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
		    close(pipefd[1]);
		    _exit(500);
		}
		close(pipefd[1]);
		char absPath[PATH_MAX];
		if (realpath(cgiPath.c_str(), absPath) == NULL)
			_exit(500);
		char *const execArgs[] = {absPath, NULL};
		execve(absPath, execArgs, env);
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
	if (close(pipefd[0]) == 0 || close(pipefd[1]) == 0)
		return 500;
	
	for (int i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
	env = NULL;

	return 200;
}