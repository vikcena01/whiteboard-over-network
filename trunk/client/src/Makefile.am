## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/whiteboard-over-network/glade
glade_DATA = whiteboard-over-network.glade
	

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(WHITEBOARD_OVER_NETWORK_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g \
	 -rdynamic

bin_PROGRAMS = whiteboard-over-network

whiteboard_over_network_SOURCES = \
	main.c \
	netio.c\
	netio.h 

whiteboard_over_network_LDFLAGS = 

whiteboard_over_network_LDADD = $(WHITEBOARD_OVER_NETWORK_LIBS)

EXTRA_DIST = $(glade_DATA)


