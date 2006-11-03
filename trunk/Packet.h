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

#ifndef __PACKET_H_
#define __PACKET_H_

#include <iostream>
#include <string>
#include <cstring>



using namespace std;

#define WON2_SERVER_PORT 41321
#define WON2_PACKET_SIGNATURE "WON2_NET_PACKET_V001"
#define PACKET_SYS 1
#define PACKET_DATA 2

struct won_packet {
    char signature[21];
    int level;
    int flags;
    long len;
    char data[0];
};

typedef struct won_packet packet;
class Packet
{
protected:
	int fd;
public:
	Packet();
	Packet(string dest, int port);
	Packet(int ffd);

	/* Recieve the whole data from connection. */
	char *readall(long *n, int *level, int *flags);
	
	/* Send the whole data to connection. */
	long writeall(int target_fd, void *ptr, long n, int level, int flags);
};

#endif
