
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

#include "Request.hpp"

#define BUFSIZE     4096

class   Config;
class	Request;

class   Servers
{
	private:
		std::vector<int>	_serverSockets;
		int					_total_ports;
		Config				_config;
		fd_set				_current_sockets, _ready_sockets;

	 public:
		Servers(Config config);
		Servers();
		~Servers();
	
		void        createServerSocket();
		int			makeSocket(int port, sockaddr_in addr);
		void        pingServer();
		int         acceptConnection();
		void        receiveRequest(int connectionSocket);
		void        setNonBlocking(int sockfd);
		
};