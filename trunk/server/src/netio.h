#ifndef __NETIO_H_
#define __NETIO_H_


#include <gnutls/gnutls.h>
#include <glib-2.0/glib.h>

/* Got that from Linux kernel ;) */
#define PACKET_MAGIC 0xDEADBEEF
#define WON_SERVER_PORT 5252
#define DH_BITS 1024

/* Possible credentials. */
#define CRED_ANON 0x1
#define CRED_CERT 0x2
#define CRED_PASS 0x3
#define CRED_OPGP 0x4

struct security_s {
	gnutls_session_t session;
	/* Possible credentials. */
	gnutls_anon_server_credentials_t anoncred;
    int listener;
};

/* Packet header information. */
typedef struct {
	long magic;
	int reserved;
	unsigned int level;
	long long len;
} packet_t;

typedef struct {
    GList *client_fds;
} won_server_t;

static gnutls_dh_params_t dh_params;
gnutls_anon_server_credentials_t anoncred;
won_server_t *won_server;

/*
 * Initializes GnuTLS session and sets credentials.
 */
struct security_s *network_server_init(int security_policy);

/* The worker thread for connections. */
gpointer
network_server_work(gpointer data);

/*
 * Get the server into a loop. 
 */
int
network_server_loop(struct security_s *security);

/*
 * Creates a new TLS session.
 */
gnutls_session_t 
network_session();

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
