#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "netio.h"

struct security_s *
network_client_init(int security_policy)
{
	struct security_s *security = malloc(sizeof(*security));
	int kx_prio[2];

	gnutls_global_init ();

	if (security_policy == CRED_ANON) {
		gnutls_anon_allocate_client_credentials(&security->anoncred);
	}

    /* Initialize TLS session */
	gnutls_init(&security->session, GNUTLS_CLIENT);
    /* Use default priorities */
    gnutls_set_default_priority(security->session);
    gnutls_kx_set_priority (security->session, kx_prio);
	if (security_policy == CRED_ANON) {
		kx_prio[0] = GNUTLS_KX_ANON_DH;
		kx_prio[1] = 0;
		gnutls_kx_set_priority (security->session, kx_prio);
		/* Put the anonymous credentials to the current session */
		gnutls_credentials_set(security->session, GNUTLS_CRD_ANON,
							   security->anoncred);
	}
	return security;
}

int
network_connect(struct security_s *security,
                char *server)
{
    int s_fd;
    int err, ret;
    struct sockaddr_in sa;
    
    s_fd = socket (AF_INET, SOCK_STREAM, 0);

    memset(&sa, '\0', sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(WON_SERVER_PORT));
    inet_pton (AF_INET, server, &sa.sin_addr);

    err = connect(s_fd, (struct sockaddr *)&sa, sizeof(sa));
    if (err < 0) {
        printf("Connect error\n");
        return -1;
    }

    gnutls_transport_set_ptr(security->session, (gnutls_transport_ptr_t)s_fd);

    /* Perform the TLS handshake. */
    ret = gnutls_handshake(security->session);

    if (ret < 0) {
        printf("*** Handshake failed\n");
        gnutls_perror(ret);
    } else {
        printf("- Handshake was completed\n");
    }
    
    return s_fd;
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
