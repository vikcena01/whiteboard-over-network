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

#include <signal.h>

using namespace std;

#include "WONConnection.h"

#define APP_MSG \
	"Whiteboard Over Network 2 - server\n" \
	"Copyright (C) 2006, Luka Napotnik <luka.napotnik@gmail.com>\n\n" \
	"Available parameters:\n" \
	"--help\t: This message\n" \
	"-v\t: print version\n" \
	"-d\t: daemon mode\n\n"

#define WON2_SERVER_VERSION "0.0.1-alpha1"

void sig_segfault(int sig)
{
	cout << "The program has crashed\n";
}

int main(int argc, char **argv)
{
	WONConnection *conn;

	signal(SIGSEGV, sig_segfault);

	if (argc > 1) {
		if (strcmp(argv[1], "--help") == 0)
			cout << APP_MSG;
		else if (strcmp(argv[1], "-v") == 0)
			cout << "Whiteboard Over Network 2 - server\nVersion " << WON2_SERVER_VERSION << "\n";
			return 1;
	}

	conn = new WONConnection(10);
	try {
		//conn->connect("localhost", 4242);
        conn->start();

	    conn->ping();
	    conn->sendData("aloooo\n", -1);
	    conn->close();
	} catch (string str) {
			cout << "Connection error: " << str << ".\n";
	}

	return 0;
}

