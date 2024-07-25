/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brmajor <brmajor@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 11:42:06 by brmajor           #+#    #+#             */
/*   Updated: 2024/07/25 15:44:54 by brmajor          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Response.hpp"
#include "../include/Request.hpp"
#include "../include/Cgi.hpp"

Server::Server(in_port_t port, Port config)
{
	this->_config = config;
	this->_port = port;
	createServerSocket(port);

	FD_ZERO(&_current_sockets);
	FD_SET(_serverSocket, &_current_sockets);
}

Server::Server()
{   
}

Server::~Server()
{   
}

void	Server::createServerSocket(in_port_t port)
{
	sockaddr_in		addr;
	
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
	{
		perror("error: socket");
	}
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	int	reuse = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("error: setsockopt");
	}
	if ((bind(_serverSocket, (struct sockaddr*)&addr, sizeof(sockaddr))) == -1)
	{
		perror("error: bind");
	}
	if ((listen(_serverSocket, 10)) == -1)
	{
		perror("error: listen");
	}
}

void Server::pingServer()
{
	_ready_sockets = _current_sockets;
		
	if (select(FD_SETSIZE, &_ready_sockets, NULL, NULL, NULL) < 0)
	{
		perror("error: select");
	}
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (FD_ISSET(i, &_ready_sockets))
		{
			//new connection
			if (i == _serverSocket)
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

int		Server::acceptConnection()
{
	int	connectionSocket;

	sockaddr	socketAddress;
	socklen_t	socketAdressLen = sizeof(socketAddress);
	
	if ((connectionSocket = accept(_serverSocket, &socketAddress, &socketAdressLen)) == -1)
	{
		perror("error: accept");
	}
	sockaddr_in* addr_in = (sockaddr_in *)&socketAddress;
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
	std::cout << "IPv4 Address: " << ip << ", Port: " << ntohs(addr_in->sin_port) << std::endl;
	
	return (connectionSocket);
}

void	Server::receiveRequest(int connectionSocket)
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
	send(connectionSocket, response, response.size(), 0);
}

void    Server::setNonBlocking(int sockfd)
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