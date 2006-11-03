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

#ifndef __SOCKETSTREAM_H_
#define __SOCKETSTREAM_H_

#include <iostream>
#include <string>
#include <cstring>

#include "Packet.h"

using namespace std;

class SocketStream : public Packet
{
protected:
	struct sockaddr_in addr;
	int port;
public:
	SocketStream();
	SocketStream(string address, int host_port);
	int connect(string address, int host_port);
	int close();
};

#endif
