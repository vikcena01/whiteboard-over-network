#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "netio.h"

struct security_s *
network_server_init(int security_policy)
{
	struct security_s *security = malloc(sizeof(*security));
	int kx_prio[2];

	gnutls_global_init ();

	if (security_policy == CRED_ANON) {
		gnutls_anon_allocate_server_credentials(&security->anoncred);
	}

    gnutls_dh_params_init (&dh_params);
    gnutls_dh_params_generate2 (dh_params, DH_BITS);
    gnutls_anon_set_server_dh_params (security->anoncred, dh_params);
    
	return security;
}

int
network_server_loop(struct security_s *security)
{
    struct sockaddr_in sa, sa_cl;
    int sd;
    gnutls_session_t session;
    int optval = 1;
    int err, client_len, ret;
    char topbuf[512];
    
    security->listener = socket (AF_INET, SOCK_STREAM, 0);

    memset (&sa, '\0', sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(WON_SERVER_PORT);

    setsockopt(security->listener, SOL_SOCKET,
               SO_REUSEADDR, &optval, sizeof(int));

    err = bind(security->listener, (struct sockaddr *)&sa, sizeof(sa));
    err = listen(security->listener, 1024);
    client_len = sizeof(sa_cl);
    
    for (;;) {
        session = network_session();

        sd = accept(security->listener, (struct sockaddr *)&sa_cl, &client_len);

        printf ("- connection from %s, port %d\n", 
                inet_ntop(AF_INET, &sa_cl.sin_addr, topbuf,
                sizeof (topbuf)), ntohs(sa_cl.sin_port));

        gnutls_transport_set_ptr (session, (gnutls_transport_ptr_t)sd);
        ret = gnutls_handshake(session);
        if (ret < 0) {
            close(sd);
            gnutls_deinit(session);
            printf("Handshake failed (%s)\n\n",
                   gnutls_strerror(ret));
            continue;
        }
        printf("Handshake completed\n");
    }
    return 0;
}

gnutls_session_t 
network_session()
{
    int s_fd;
    int err, ret;
    struct sockaddr_in sa;
    gnutls_session_t session;
    const int kx_prio[] = { GNUTLS_KX_ANON_DH, 0 };

    gnutls_init (&session, GNUTLS_SERVER);
    gnutls_set_default_priority (session);
    gnutls_kx_set_priority (session, kx_prio);

    gnutls_credentials_set (session, GNUTLS_CRD_ANON, anoncred);

    gnutls_dh_set_prime_bits (session, DH_BITS);

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
	
	return r_len;
}
