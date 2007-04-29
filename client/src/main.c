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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <gnutls/gnutls.h>

#include "netio.h"

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

/* For testing propose use the local (not installed) glade file */
/* #define GLADE_FILE PACKAGE_DATA_DIR"/whiteboard-over-network/glade/whiteboard-over-network.glade" */
#define GLADE_FILE "whiteboard-over-network.glade"
GladeXML *gxml;
gboolean
execute_command(gpointer data);

void button_connect_clicked(GtkButton *button, gpointer data)
{
	char login_msg[64];
	GtkWidget *entry_server, *label_status, *entry_user;
	unsigned int level = PACKET_SYS;
	entry_server = glade_xml_get_widget(gxml, "combo_server");	
	label_status = glade_xml_get_widget(gxml, "label_login_status");
	entry_user = glade_xml_get_widget(gxml, "entry_user");
	
	network_client_init(CRED_ANON);
	if (network_connect(gtk_combo_box_get_active_text(entry_server)) != 0)
		gtk_label_set_markup(label_status, "<span size=\"small\">Status: <i>error connecting</i></span>");
	/* Create a thread that will listen for incomming data from server. */
	g_thread_create(network_reciever, NULL, NULL, NULL);
	g_timeout_add(100, execute_command, NULL);

	sprintf(login_msg, "login %s", gtk_entry_get_text(entry_user));
	printf("msg: \'%s\'(size %d)\n", login_msg, strlen(login_msg));
	network_send(&level, login_msg, strlen(login_msg), security->session);
}

void button_cancel_clicked(GtkButton *button, gpointer data)
{
	gtk_main_quit();
}

gboolean
execute_command(gpointer data)
{
	char *command;

	command = (char *)g_async_queue_try_pop(sys_queue);
	if (!command)
		return TRUE;
	printf("Got system command: %s\n", command);
	free(command);
	return TRUE;
}

GtkWidget*
create_window_login (void)
{
	GtkWidget *window, *window_rooms;
	
	gxml = glade_xml_new (GLADE_FILE, NULL, NULL);
	
	glade_xml_signal_autoconnect (gxml);
	window = glade_xml_get_widget (gxml, "window_login");
	return window;
}

int
won_client_connect()
{
}

int
main (int argc, char *argv[])
{
 	GtkWidget *window_login;
	struct security_s *security;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	#endif

	
	gtk_set_locale ();
	g_thread_init(NULL);
	gtk_init (&argc, &argv);
	
	window_login = create_window_login();
	gtk_widget_show (window_login);
	gtk_main ();

	return 0;
}
