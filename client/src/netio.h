#ifndef __NETIO_H_
#define __NETIO_H_


#include <gnutls/gnutls.h>
#include <glib-2.0/glib.h>

/* Got that from Linux kernel ;) */
#define PACKET_MAGIC 0xDEADBEEF
#define WON_SERVER_PORT 5252

/* Possible credentials. */
#define CRED_ANON 0x1
#define CRED_CERT 0x2
#define CRED_PASS 0x3
#define CRED_OPGP 0x4

static gnutls_dh_params_t dh_params;

struct security_s {
	gnutls_session_t session;
	/* Possible credentials. */
	gnutls_anon_client_credentials_t anoncred;
};

/* Packet header information. */
typedef struct {
	long magic;
	int reserved;
	unsigned int level;
	long long len;
} packet_t;

/*
 * Initializes GnuTLS session and sets credentials.
 */
struct security_s *network_client_init(int security_policy);

/*
 * Connects to a host and thy to handshake. 
 * Returns the socket descriptor.
 */
int
network_connect(struct security_s *security,
                char *server);

/*
 * Sends package.
 */
long long
network_send(unsigned int level,
             void *data,
             long long len,
             gnutls_session_t session);

/*
 * Recieves package.
 */
long long
network_recv(unsigned int *level,
             void **data,
             gnutls_session_t session);

#endif
