#include "room.h"

void rooms_init()
{
	int i;

	for (i = 0; i < MAX_USERS; i++) {
		users[i].name = NULL;
		users[i].room_id = -1;
	}
}

int
room_create(char *name, int creator_sd);
{
	int i;

	/* Find an empty room. */
	for (i = 0; (rooms[i].users != NULL) || (i < MAX_ROOMS); i++);
	if (i == MAX_USERS)
		return -1; /* No room free. */
	rooms[i].creator = creator_sd;
	strncpy(name, rooms[i].name, 32);
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
	rooms[i].name = NULL;
	rooms[i].creator = -1;
}

int
room_distribute(int sd, void *data)
