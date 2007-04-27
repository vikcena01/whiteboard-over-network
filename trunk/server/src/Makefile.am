## Process this file with automake to produce Makefile.in

## Created by Anjuta

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GLIB_CFLAGS) \
	$(GNUTLS_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = server

server_SOURCES = \
	main.c \
	netio.c

server_LDFLAGS = 

server_LDADD = $(GLIB_LIBS) \
               $(GNUTLS_LIBS)

