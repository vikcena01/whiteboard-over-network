#ifndef __ROOM_H_
#define __ROOM_H_

#include <gnutls/gnutls.h>
#include <glib-2.0/glib.h>
#include "netio.h"

#define MAX_ROOMS 100
struct room_s {
    char name[32];
    int creator;
    GList *users;
};

struct room_s rooms[MAX_ROOMS];
/*
 * Creates a new room with sd (socket descriptor) of the user as the admin.
 */
int
room_create(char *name, int creator_sd);

/* Destroys a room. */
int
room_destroy(char *name);

/* Distributes data to all users in the room. */
int
room_distribute(int sd, void *data);

/* Sends a list of available rooms. */
int
room_refresh();

#endif

