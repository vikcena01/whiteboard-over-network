/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Luka Napotnik 2007 <luka.napotnik@gmail.com>
 * 
 * main.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * main.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with main.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include <glib-2.0/glib.h>


#include "netio.h"
#include "../config.h"

int main(int argc, char **argv)
{
	int port;

	if (argc > 1) {
		if (strcmp(argv[1], "-p") == 0)
			port = atoi(argv[2]);
		else if (strcmp(argv[1], "-v") == 0)
			printf(PACKAGE_VERSION"\n");
		else if (strcmp(argv[1], "--help") == 0)
			printf("\t-p : specify port\n\n");
		
	}
	g_thread_init(NULL);
	
	network_server_init(CRED_ANON);
	network_server_loop();
	
	return 0;
}
