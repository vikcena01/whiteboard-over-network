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

Packet::Packet() { }
Packet::Packet(string dest, int port) { }
Packet::Packet(int ffd) { }

char *Packet::readall(long *n, int *level, int *flags)
{
	long n_left, n_read;
	char *pptr, *pptr_begin;
	packet new_packet;
	int i;
	
	if ((i = read(fd, &new_packet, sizeof(packet))) != sizeof(packet)) {
		if (i == 0) {
			cout << "Client disconnected.\n";
			return NULL;
		}
		throw string("invalid packet received");
	}
	if (strncmp(new_packet.signature, WON2_PACKET_SIGNATURE, strlen(WON2_PACKET_SIGNATURE)) != 0)
		throw string("invalid packet signature");
	n_left = new_packet.len;
	pptr = new char(new_packet.len);
	pptr_begin = pptr;
	while (n_left > 0) {
		if ((n_read = read(fd, pptr, n_left)) < 0) {
			if (errno == EINTR)
				n_read = 0;
			else
				return NULL;
		}
		else if (n_read == 0)
			break; 
		n_left -= n_read;
		pptr += n_read;
	}
	*n = new_packet.len - n_left;
	*level = new_packet.level;
	*flags = new_packet.flags;
	return pptr_begin;
}

long Packet::writeall(int target_fd, void *ptr, long n, int level, int flags)
{
	long n_left = n, n_written;
	char *pptr = (char *)ptr;
	packet new_packet;
	new_packet.level = level;
	new_packet.flags = flags;
	new_packet.len = n;
	strncpy(new_packet.signature, WON2_PACKET_SIGNATURE, 21);
	if (write(fd, &new_packet, sizeof(packet)) != sizeof(packet))
		throw string("sending packet failed");
		while (n_left > 0) {
			if ((n_written = write(target_fd, pptr, n_left)) <= 0) {
				if ((n_written < 0) && (errno == EINTR))
				n_written = 0;
			else
				return -1;
		}
		n_left -= n_written;
		pptr += n_written;
	}
		return n;
}

