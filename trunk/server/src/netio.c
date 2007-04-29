#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "netio.h"
#include "room.h"
#include "user.h"

struct security_s *
network_server_init(int security_policy)
{
	security = malloc(sizeof(*security));

	gnutls_global_init();
    gnutls_anon_allocate_server_credentials (&security->anoncred);
	gnutls_dh_params_init(&dh_params);
	gnutls_dh_params_generate2(dh_params, DH_BITS);

//	if (security_policy == CRED_ANON)
		gnutls_anon_set_server_dh_params(security->anoncred, dh_params);
	//return security;
    return NULL;
}

int
network_interpret(int sd,
                  char *cmd)
{
    char *tmp = cmd;
    if (strstr(cmd, "login") == 0) {
        cmd += 5;
        user_login(sd, cmd);
    }
    else if (strstr(cmd, "logout") == 0)
        user_logout(sd);
    else if (strstr(cmd, "join") == 0) {
        tmp = cmd;
        cmd += 5;
        if (user_join(sd, cmd) != 0)
            printf("Could not join room %s\n", cmd);
    }
    //else if (strstr(cmd, "part") == 0)
       // room_part();
    else if (strstr(cmd, "create") == 0) {
        cmd += 7;
        if (room_create(cmd, sd) != 0)
            printf("Could not create room %s\n", cmd);
    }
    else if (strstr(cmd, "refresh") == 0)
        room_refresh();

    free(tmp);
    return 0;
}

gpointer
network_server_work(gpointer data)
{
    int sd = *((int *)data);
    int ret;
    char *packet_data;
    unsigned int level;
    gnutls_session_t session;
    
    pthread_detach (pthread_self());
    g_free(data);
    session = network_session();
    
    gnutls_transport_set_ptr (session, (gnutls_transport_ptr_t)sd);
    ret = gnutls_handshake(session);
    if (ret < 0) {
        close(sd);
        gnutls_deinit(session);
        printf("Handshake failed (%s)\n\n",
            gnutls_strerror(ret));
        return NULL;
    }
    printf("Handshake completed\n");
    
    while (1) { /* Do stuff. */
            if (network_recv(&level, (void **)&packet_data, session) < 1) {
                printf("Lost connection\n");
                break;
            }
            //printf("Got package(level %d): %s\n", level, data);
            network_interpret(sd, packet_data);
            free(packet_data);
    }
    /* End the gnutls session. */
    gnutls_bye(session, GNUTLS_SHUT_WR);
    close(sd);
    gnutls_deinit(session);
    return NULL;
}

int
network_server_loop()
{
    struct sockaddr_in sa, sa_cl;
    int *sd;
    int optval = 1;
    int err, client_len;
    char topbuf[512];
    
    security->listener = socket (AF_INET, SOCK_STREAM, 0);
    if (security->listener < 0)
        printf("socket() error.\n");
    memset (&sa, '\0', sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(WON_SERVER_PORT);

    setsockopt(security->listener, SOL_SOCKET,
               SO_REUSEADDR, &optval, sizeof(int));

    err = bind(security->listener, (struct sockaddr *)&sa, sizeof(sa));
    if (err < 0)
        printf("bind() error.\n");
    err = listen(security->listener, 1024);
    if (err < 0)
        printf("listen() error\n");
    client_len = sizeof(sa_cl);
    
    for (;;) {
        sd = g_malloc(sizeof(int));
        *sd = accept(security->listener, (struct sockaddr *)&sa_cl, (socklen_t *)&client_len);
        g_thread_create(network_server_work, sd, FALSE, NULL);
        printf ("- connection from %s, port %d\n", 
                inet_ntop(AF_INET, &sa_cl.sin_addr, topbuf,
                sizeof (topbuf)), ntohs(sa_cl.sin_port));
    }
    return 0;
}

gnutls_session_t 
network_session()
{
    gnutls_session_t session;
    int kx_prio[2];

    gnutls_init(&session, GNUTLS_SERVER);
    gnutls_set_default_priority(session);

    kx_prio[0] = GNUTLS_KX_ANON_DH;
    kx_prio[1] = 0;

	gnutls_kx_set_priority(session, kx_prio);
	gnutls_credentials_set(session, GNUTLS_CRD_ANON, security->anoncred);
	gnutls_dh_set_prime_bits(session, DH_BITS);

    return session;
}

long long
network_send(unsigned int level,
             void *data,
             long long len,
             gnutls_session_t session)
{
	size_t s_len;
	packet_t packet;

	/* Prepare and send metainformation. */
	packet.magic = PACKET_MAGIC;
	packet.level = level;
	packet.len = len;
	s_len = gnutls_record_send(session, &packet, sizeof(packet_t));
	if (s_len < sizeof(packet_t))
		return -1;

	s_len = gnutls_record_send(session, data, len);
	if (s_len < len)
		return -2;

	return s_len;
}

long long
network_recv(unsigned int *level,
             void **data,
             gnutls_session_t session)
{
	size_t r_len;
	packet_t packet;
	
	/* Get the metainformation packet. */
	r_len = gnutls_record_recv(session, &packet, sizeof(packet_t));
	if (r_len < sizeof(packet_t))
		return -1;
	if (packet.magic != PACKET_MAGIC) /* Corrupted package. */
		return -2;
	
	*data = malloc(packet.len);
	r_len = gnutls_record_recv(session, *data, packet.len);
	if (r_len < packet.len)
		return -3;
	*level = packet.level;
	return r_len;
}
