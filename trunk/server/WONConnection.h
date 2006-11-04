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

#ifndef __WONCONNECTION_H_
#define __WONCONNECTION_H_

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

#include "SocketStream.h"

using namespace std;

class WONConnection : public SocketStream
{
/* --- Server stuff --------- */
	struct sockaddr_in server_addr;
	fd_set master, read_fds;
	int fd_max, listener, n_bytes, i, j;
	int n_ready, backlog;
    socklen_t client_len;
/* -------------------------- */

	struct user {
		int fd;
		string name;
		int flags;
		int room_id;
		user *next, *prev;
	};

	struct room {
		user *users;
		int flags;
		string name;
	};

	int max_users; /* Maximal number of users/rooms on the server. */
	user *users; /* An array of users loged in. */
	room *rooms; /* An array of rooms. */

public:
	WONConnection(int n_users);

	/* Send data. */
	int sendData(string data, int target_fd);

	/* Recieve data.  IN QUESTION? */
	string recvData();

	/* Send a system message. */
	int sendMsg();
	/* Recieve a system message. */
	int recvMsg();
	void ping();

	/* Start listening for connections. */
	int start();
	int interpret(int cur_fd);
	int distributeData(int cur_fd, char *message);

	int roomCreate(int user_fd, string name, int flags);
	int roomJoin(int user_fd, int room_id);
	int roomLeave(int user_fd);
	int roomRefresh(int user_fd);
	int userLogin(int user_fd, string name);
};

#endif
