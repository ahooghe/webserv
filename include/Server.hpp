
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brmajor <brmajor@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 14:31:21 by brmajor           #+#    #+#             */
/*   Updated: 2024/03/29 13:14:33 by yaretel-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <strings.h>
#include <fcntl.h>
#include <vector>
#include <netinet/tcp.h>

#include "Request.hpp"

#define BUFSIZE     4096

class   Config;
class	Request;

class   Servers
{
	private:
		std::vector<int>	_serverPorts;
		std::vector<int>	_serverSockets;
		int					_total_ports;
		Config				_config;
		fd_set				_current_sockets, _ready_sockets;

		void				_closeAllSockets(std::vector<int>& sockets);
		std::string			_intToString(int value); 
	public:
		Servers(Config config);
		Servers();
		~Servers();
	
		void        createServerSocket();
		int			makeSocket();
		void        pingServer();
		int         acceptConnection(int serverSocket);
		void        receiveRequest(int connectionSocket);
		void        setNonBlocking(int sockfd);
		
};