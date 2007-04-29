#include <string.h>

#include "user.h"
#include "room.h"

void
users_init()
{
    int i;
    
    for(i = 0; i < MAX_USERS; i++) {
        users[i].session = NULL;
        users[i].room_id = -1;
    }
}

int
user_login(int sd,
           char *inf)
{
    strncpy(users[sd].name, inf, 32);
    return 0;
}

int
user_logout(int sd)
{
    users[sd].room_id = -1;
    users[sd].session = NULL;
    
    return 0;
}

int
user_join(int sd,
          char *room)
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++) {
        if (strcmp(rooms[i].name, room) == 0)
            break;
    }
    if (i == MAX_ROOMS)
        return -1; /* Room not found. */
        
    users[sd].room_id = i;
    rooms[i].users = g_list_append(rooms[i].users, GUINT_TO_POINTER(sd));
    
    return 0;
}
