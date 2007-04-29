#ifndef __USER_H_
#define __USER_H_

#include <gnutls/gnutls.h>
#include <glib-2.0/glib.h>
#include "netio.h"

#define MAX_USERS 100

struct user_s {
	char name[32];
	int room_id;
    gnutls_session_t session;
};

struct user_s users[MAX_USERS];

void
users_init();

/*
 * Logs in the user.  
 */
int
user_login(int sd,
           char *inf);

/*
 * Logs out hte user.  
 */
int
user_logout(int sd);

int
user_join(int sd,
          char *room);
#endif
