#include <string.h>

#include "room.h"
#include "user.h"

void rooms_init()
{
	int i;

	for (i = 0; i < MAX_USERS; i++) {
		rooms[i].users = NULL;
	}
}

int
room_create(char *name, int creator_sd)
{
	int i;

	/* Find an empty room. */
	for (i = 0; (rooms[i].users != NULL) || (i < MAX_ROOMS); i++);
	if (i == MAX_USERS)
		return -1; /* No room free. */
	rooms[i].creator = creator_sd;
	strncpy(rooms[i].name, name, 32);
    rooms[i].users = g_list_append (rooms[i].users, 
        GUINT_TO_POINTER(creator_sd));

	return 0;
}

int
room_destroy(char *name)
{
	int i = 0;

	for (; i < MAX_ROOMS; i++) {
		if (strcmp(rooms[i].name, name) == 0)
			break;
	}
	rooms[i].users = NULL;
	rooms[i].creator = -1;
    
    return 0;
}

int
room_distribute(int sd, void *data)
{
    return 0;
}

int
room_refresh()
{
    return 0;
}
