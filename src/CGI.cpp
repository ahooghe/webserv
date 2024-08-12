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

void alarmHandler(int sig) {
    (void)sig;
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

		struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = alarmHandler;
        sigaction(SIGALRM, &sa, NULL);

        struct itimerval timer;
        timer.it_value.tv_sec = 5;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);

		int result = waitpid(pid, &status, 0);

		timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);

		if (result == -1)
		{
			close(pipefd[0]);
			return 500;
		}
		else if (result == 0)
		{
			kill(pid, SIGKILL);
			close(pipefd[0]);
			return 504;
		}
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
			this->_response += "\r\n";
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
