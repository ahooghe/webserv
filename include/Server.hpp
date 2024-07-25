
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
#include <fstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>
#include <fcntl.h>
#include "Config.hpp"
#include "Error.hpp"
#include "Request.hpp"

//#define SERVER_PORT 8000
#define BUFSIZE     4096

class   Config;
class	Request;

class   Server
{
	private:
		in_port_t	_port;
		int			_serverSocket;
		Port		_config;
		fd_set		_current_sockets, _ready_sockets;

	 public:
		Server(in_port_t port, Port config);
		Server();
		~Server();
	
		void        createServerSocket(in_port_t port);
		void        pingServer();
		int         acceptConnection();
		void        receiveRequest(int connectionSocket);
		void        setNonBlocking(int sockfd);
		
};