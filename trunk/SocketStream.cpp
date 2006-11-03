/*
 *  Whiteboard Over Network 2
 *  Copyright (C) 2006  Luka Napotnik
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <iostream>
#include <string>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <strings.h>
#include <errno.h>

#include "Packet.h"
#include "SocketStream.h"

SocketStream::SocketStream() { }
SocketStream::SocketStream(string address, int host_port)
{
	try {
		connect(address, host_port);
	} catch (string str) {
				throw str;
	}
}

int SocketStream::close() { }

int SocketStream::connect(string address, int host_port)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
	addr.sin_port = htons(host_port);
	port = host_port;

	if (::connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) != 0)
		throw string("unable to connect to " + address);
}

