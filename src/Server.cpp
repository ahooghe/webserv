/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahooghe <ahooghe@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 11:42:06 by brmajor           #+#    #+#             */
/*   Updated: 2024/07/29 23:23:41 by ahooghe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Response.hpp"
#include "../include/Request.hpp"
#include "../include/Config.hpp"

Servers::Servers(Config config)
{
	this->_config = config;
	createServerSocket();


}

Servers::Servers()
{   
}

Servers::~Servers()
{   
}

void	Servers::createServerSocket()
{
	sockaddr_in		addr;
	_total_ports = _config.getTotalports();
	int		*ports = _config.getPorts();
	int		reuse[_total_ports];

	for (int i = 0; i < _total_ports; ++i)
	{
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(ports[i]);
	
		_serverSockets.push_back(makeSocket());
		reuse[i] = 1;
		if (setsockopt(_serverSockets[i], SOL_SOCKET, SO_REUSEADDR, &reuse[i], sizeof(reuse[i])) < 0)
		{
			perror("error: setsockopt");
		}
		if ((bind(_serverSockets[i], (struct sockaddr*)&addr, sizeof(sockaddr))) == -1)
		{
			perror("error: bind");
		}
		if ((listen(_serverSockets[i], 10)) == -1)
		{
			perror("error: listen");
		}
		FD_ZERO(&_current_sockets);
		FD_SET(_serverSockets[i], &_current_sockets);
	}
}

int	Servers::makeSocket()
{
	int	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		perror("error: socket");
	}
	return (serverSocket);
}

void Servers::pingServer()
{
	_ready_sockets = _current_sockets;
		
	if (select(FD_SETSIZE, &_ready_sockets, NULL, NULL, NULL) < 0)
	{
		perror("error: select");
	}
	for (int j = 0; j < _total_ports; j++)
	{
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &_ready_sockets))
			{
				//new connection
				if (i == _serverSockets[j])
				{
					int	connectionSocket = acceptConnection();
					FD_SET(connectionSocket, &_current_sockets);
				}
				//handle the connection
				else
				{
					receiveRequest(i);
					FD_CLR(i, &_current_sockets);
				}
			}
		}
	}
}

int		Servers::acceptConnection()
{
	int	connectionSocket;

	sockaddr	socketAddress;
	socklen_t	socketAdressLen = sizeof(socketAddress);
	
	for (int i = 0; i < _total_ports; i++)
	{
		if ((connectionSocket = accept(_serverSockets[i], &socketAddress, &socketAdressLen)) != -1)
		{
			sockaddr_in* addr_in = (sockaddr_in *)&socketAddress;
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
			std::cout << "IPv4 Address: " << ip << ", Port: " << ntohs(addr_in->sin_port) << std::endl;
			return (connectionSocket);
		}
	}
	perror("error: accept");
	return (-1);
}

void	Servers::receiveRequest(int connectionSocket)
{	
	char                 buffer[BUFSIZE];
	ssize_t              bytesRead;

	bzero(buffer, BUFSIZE);
	setNonBlocking(connectionSocket);
	while ((bytesRead = recv(connectionSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[bytesRead] = '\0';
		if (bytesRead == -1)
		{
			perror("read error");
		}
		else if (bytesRead == 0)
		{
			perror("nothing to read");
		}
	}
	std::string request_string(buffer);
	Request request(this->_config, request_string);
    std::cerr << "Request received, it was:\n" << buffer << std::endl;
	request.execute();
	std::string response = request.getResponse();

	send(connectionSocket, response.c_str(), response.size(), 0);
	close(connectionSocket);
}

void    Servers::setNonBlocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl failed");
		return ;
	}

	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl failed");
		return ;
	}
	return ;
}