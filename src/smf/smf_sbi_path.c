#define TRACE_MODULE _smf_sbi_path

#include "core_network.h"

#include "smf_sbi_path.h"

static int _smf_sbi_message(sock_id sock, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;
    d_info("Recv: %s, from: %s", pkbuf->payload, from.sun_path);

    rv = unixgram_sendto(sock, pkbuf, &from);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    pkbuf_free(pkbuf);

    return CORE_OK;
}

status_t smf_sbi_server_open()
{
    status_t rv;
    sock_id new;
    c_sockaddr_t addr;
    memcpy(addr.sun_path, "/tmp/smf_smContextCreate.csock", sizeof(addr.sun_path));
    addr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&new, &addr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(new, _smf_sbi_message, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    return CORE_OK;
}

status_t smf_sbi_server_close()
{
    return CORE_OK;
}
