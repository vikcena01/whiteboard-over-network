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
#include "WONConnection.h"

using namespace std;

WONConnection::WONConnection(int n_users)
{
	max_users = n_users;

	users = new user[n_users];
	rooms = new room[n_users];

	for (int i = 0; i < n_users; i++) {
		users[i].fd = -1;
		users[i].name = "";
		users[i].flags = 0;
		users[i].room_id = -1; /* In no room. */

		rooms[i].name = "";
		rooms[i].users = NULL;
		rooms[i].flags = 0;
	}
}

int WONConnection::sendData(string data, int target_fd)
{
	int conn_fd = fd;

	if (target_fd > -1)
		conn_fd = target_fd;
	long n;
       int level = PACKET_DATA, flags = 0;
	if ((n = writeall(conn_fd, (void *)data.c_str(), data.length(), level, flags)) == -1)
		throw string("sending data failed with errors");
	return n;
}

string WONConnection::recvData()
{
	char *buf;
	long n;
       int level, flags;
	string str;
	if ((buf = readall(&n, &level, &flags)) == NULL)
		throw string("reading data failed with errors");
        str = buf;
       delete buf;
	return str;
}

int WONConnection::sendMsg() {}
int WONConnection::recvMsg() {}
void WONConnection::ping() {}

int WONConnection::start()
{
	int yes = 1;
	backlog = 10;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	listener = socket(PF_INET, SOCK_STREAM, 0);

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(WON2_SERVER_PORT);

        if (setsockopt(listener ,SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw string("Starting the server failed becose setsockopt() failed");
	if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw string("Starting the server failed becose bind() failed");
	if (listen(listener, backlog) == -1)
	throw string("Starting the server failed becose listen() failed");

	FD_SET(listener, &master);
	fd_max = listener;

	while (1) {
		read_fds = master;
		if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1)
		throw string("error while select()");
		for (i = 0; i <= fd_max; i++) {
			if (FD_ISSET(i, &read_fds)) { /* New connection. */
				if (i == listener) {
					client_len = sizeof(addr);
					if ((fd = accept(listener, (struct sockaddr *)&addr, &client_len)) == -1)
				cout << "Warning: could not accept client.\n";
					else {
						FD_SET(fd, &master);
						if (fd > fd_max)
							fd_max = fd;
					}
				} else { /* Recieve data. */
					string str;
					try {
						string str;
						interpret(i);
						if (interpret(i) == -1) {
							::close(i);
							FD_CLR(i, &master);
						}
					} catch (string str) {
						cout << "Connection hung up.\n";
						::close(i);
						FD_CLR(i, &master);
					}
				}
			}
		}
	}
	return 0;
}

int WONConnection::interpret(int cur_fd)
{
	char *message;
	int level, flags;
	long len;
	try {
		message = readall(&len, &level, &flags);
		char *message_begin = message;
		if (message == NULL)
			return -1;
		if (level == PACKET_SYS) {
			if (strncmp(message, "login", 11) == 0) { /* User is loging in. */
				message += 6;
				if (userLogin(cur_fd, message) != 0)
					cout << "Error: while loging in the user" << message;
			}
			else if (strncmp(message, "room_create", 11) == 0) { /* Create a new room. */
				message += 12;
				if (roomCreate(cur_fd, message, 0) == -1)
					cout << "Error: room number limit exceeded. \n";
			}
			else if (strncmp(message, "room_join", 11) == 0) { /* Join a room. */
				int status;
				int room_id;
				
				message += 10;
				room_id = atoi(message);
				status = roomJoin(cur_fd, room_id);
				switch (status) {
					case -1: cout << "Error: invalid room id\n"; break;
					case -2: cout << "Error: user is already in a room.\n"; break;
					case -3: cout << "Error: the room does not exist.\n"; break;
				}
			}
			else if (strncmp(message, "room_leave", 11) == 0) { /* Leave a room. */
				if (roomLeave(cur_fd) != 0)
					cout << "Error: user could not leave the room.\n";
			}
			else if (strncmp(message, "rooms_refresh", 11) == 0) { /* Refresh room list. */
				roomRefresh(cur_fd);
			}
			else if (strncmp(message, "logout", 11) == 0) { /* Log out from the server. */
			}
		} else
			distributeData(cur_fd, message);
		delete message_begin;
	} catch (string str) {
		cout << "Error while recieving message for the interpreter: " << str << ".\n";
	}
	return 0;
}

int WONConnection::distributeData(int cur_fd, char *message)
{
	room *the_room;
	user *the_user;

	if (users[cur_fd].room_id < 0)
		return 0; /* User is not in the room. */

	the_room = &rooms[users[cur_fd].room_id];
	the_user = the_room->users;

	for (; the_user; the_user = the_user->prev) {
		if (the_user->fd == cur_fd) /* Skip the sender. */
			continue;
		/* Send data to all other users in the room. */
		sendData(string(message), the_user->fd);
	}

	return 0;
}

int WONConnection::roomCreate(int user_fd, string name, int flags)
{
	int i;
	for (i = 0; i < max_users; i++) { /* Find space for a room. */
		if (rooms[i].users == NULL) 
			break;
	}

	if (i == max_users)
		return -1; /* Room number limit exceeded. */


	rooms[i].users = &users[user_fd]; /* Put the creator in the room. */
	rooms[i].name = name;
	rooms[i].flags = flags;
	users[user_fd].room_id = i;
	users[user_fd].next = users[user_fd].prev = NULL;

	return 0;
}

int WONConnection::roomJoin(int user_fd, int room_id)
{
	if ((room_id < 0) || (room_id > (max_users - 1))) /* Invalid room id. */
		return -1;
	if (users[user_fd].room_id > -1) /* User is already in a room. */
		return -2;
	if (rooms[room_id].users == NULL) /* The room does not exist. */
		return -3;

	/* Add the user to the room. */
	users[user_fd].prev = rooms[room_id].users;
	rooms[room_id].users = &users[user_fd];

	return 0;
}

int WONConnection::roomLeave(int user_fd)
{
	int i;
	user *room_user;
	int room_id = users[user_fd].room_id;

	if (room_id < 0) /* User is not in any room. */
		return -1; 

	room_user = rooms[room_id].users;

	while (room_user) {
		if (room_user->name == users[user_fd].name)
			break;
	}

	if (!room_user)
		return -2; /* User not found in the room. */

	rooms[room_id].users = room_user->prev;
	rooms[room_id].users->next = room_user->next;

	room_user->room_id = -1;

	return 0;
}

int WONConnection::roomRefresh(int user_fd)
{
	string room_list = "";
	
	for (int i = 0; i < max_users; i++) {
		if (rooms[i].users)
			room_list += rooms[i].name + ":" + (char)(48 + i) + "\n";
	}
	sendData(room_list, user_fd);
	
	return 0;
}

int WONConnection::userLogin(int user_fd, string name)
{
	users[user_fd].fd = user_fd;
	users[user_fd].name = name;

	return 0;
}

