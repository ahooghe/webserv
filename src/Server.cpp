/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahooghe <ahooghe@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 11:42:06 by brmajor           #+#    #+#             */
/*   Updated: 2024/08/15 22:53:10 by ahooghe          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Response.hpp"
#include "../include/Request.hpp"
#include "../include/Config.hpp"
#include <sstream>
#include <iostream>

Servers::Servers(Config config)
{
	this->_config = config;
	this->_total_ports = _config.getTotalports();
	std::map<std::string, int> portHosts = this->_config.getPortHosts();

    for (std::map<std::string, int>::iterator it = portHosts.begin(); it != portHosts.end(); ++it)
    {
        this->_serverPorts.push_back(it->second);
	}
	
	FD_ZERO(&this->_current_sockets);
	createServerSocket();
}

Servers::Servers()
{   
}

Servers::~Servers()
{   
}

void Servers::_closeAllSockets(std::vector<int>& sockets) 
{
	for (std::vector<int>::iterator sock = sockets.begin(); sock != sockets.end(); ++sock)
		close(*sock);
	sockets.clear();
}

std::string Servers::_intToString(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

void	Servers::createServerSocket()
{
	std::vector<int> serverSockets;
	for (int i = 0; i < this->_total_ports; i++)
	{
		int port = this->_serverPorts[i];
		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0)
		{
			_closeAllSockets(serverSockets);
			throw std::runtime_error("Error creating socket for port " + _intToString(port));
		}

		int sockOpt = 1;
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt)) < 0)
		{
			close(serverSocket);
			_closeAllSockets(serverSockets);
			throw std::runtime_error("Error setting socket options for port " + _intToString(port));
		}

		struct sockaddr_in serverAddr;
		bzero(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(port);

		if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			close(serverSocket);
			_closeAllSockets(serverSockets);
			throw std::runtime_error("Error binding socket to port " + _intToString(port));
		}

		if (listen(serverSocket, SOMAXCONN) < 0)
		{
			close(serverSocket);
			_closeAllSockets(serverSockets);
			throw std::runtime_error("Error listening on port " + _intToString(port));
		}

		setNonBlocking(serverSocket);
		serverSockets.push_back(serverSocket);
		FD_SET(serverSocket, &this->_current_sockets);
	}
	this->_serverSockets = serverSockets;
}

int	Servers::makeSocket()
{
	return -1;
}

void Servers::pingServer()
{
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	FD_ZERO(&this->_ready_sockets);
	this->_ready_sockets = _current_sockets;

	int activity = select(FD_SETSIZE, &this->_ready_sockets, NULL, NULL, &timeout);
	if (activity < 0)
	{
		perror("select");
		return;
	}
	
	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (FD_ISSET(i, &this->_ready_sockets))
		{
			if (std::find(this->_serverSockets.begin(), this->_serverSockets.end(), i) != this->_serverSockets.end())
			{
				acceptConnection(i);
			}
			else
			{
				receiveRequest(i);
			}
		}
	}
}

int		Servers::acceptConnection(int serverSocket)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int connectionSocket  =accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
	if (connectionSocket < 0)
	{
		perror ("accept error");
		return -1;
	}

	setNonBlocking(connectionSocket);
	
	FD_SET(connectionSocket, &this->_current_sockets);
	return connectionSocket;
}

void	Servers::receiveRequest(int connectionSocket)
{
	char buffer[BUFSIZE];
	std::string requestStr;
	
	while (true)
	{
		int bytesReceived = recv(connectionSocket, buffer, BUFSIZE - 1, 0);
		if (bytesReceived < 0)
		{
			perror("recv error");
			close(connectionSocket);
			FD_CLR(connectionSocket, &this->_current_sockets);
			return;
		}
		buffer[bytesReceived] = '\0';
		requestStr.append(buffer);
		if (bytesReceived == 0 || requestStr.find("\r\n\r\n") != std::string::npos)
			break;
	}

	if (requestStr.empty())
	{
		close(connectionSocket);
		FD_CLR(connectionSocket, &this->_current_sockets);
		return;
	}
	
	Request request(this->_config, requestStr);
	request.execute();
	std::string response = request.getResponse();
	int bytesSent = send(connectionSocket, response.c_str(), response.length(), 0);
	
	if (bytesSent < 0)
	{
		perror("send error");
		close(connectionSocket);
		FD_CLR(connectionSocket, &this->_current_sockets);
		return;
	}
	if (requestStr.find("keep-alive") == std::string::npos)
	{
		if (shutdown(connectionSocket, SHUT_WR) < 0)
		{
			perror("shutdown error");
			close(connectionSocket);
			FD_CLR(connectionSocket, &this->_current_sockets);
		}
	}
	else
	{
		close(connectionSocket);
		FD_CLR(connectionSocket, &this->_current_sockets);
	}
	
}

void    Servers::setNonBlocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        close(sockfd);
        throw std::runtime_error("Error getting socket flags");
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        close(sockfd);
        throw std::runtime_error("Error setting socket to non-blocking");
    }
}