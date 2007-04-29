#include <glib-2.0/glib.h>
#include "netio.h"

#define MAX_USERS 100

struct user_s {
	char name[32];
	int room_id;
};

struct user_s users[MAX_USERS];


