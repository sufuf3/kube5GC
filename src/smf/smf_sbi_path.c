#define TRACE_MODULE _smf_sbi_path

#include "core_network.h"

#include "smf_sbi_path.h"

#include <unistd.h>
#include <signal.h>

static int _smf_sbi_message_smf_smContextCreate(sock_id sock, void *data)
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

static int _smf_sbi_message_smf_smContextUpdate(sock_id sock, void *data)
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

static int _smf_sbi_message_smf_smContextRelease(sock_id sock, void *data)
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

static int _smf_sbi_message_smf_smContextRetrieve(sock_id sock, void *data)
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

void start_server()
{
    status_t rv;
    rv = fork();
    if (rv < 0)
    {
        d_fatal("Open server fail");
        return;
    }
    else if (rv == 0)
    {
        rv = setsid();
        if (rv == -1)
        {
            d_fatal("Server set session fail");
            exit(EXIT_FAILURE);
        } else
        {
            rv = execl("./http_server/smf_http_server", "smf_http_server", NULL);
            d_assert(rv != -1, return, "exec server error: %s", strerror(errno));
        }
    } else if (rv > 0)
    {
        smf_self()->server_pid = rv;
    }
}

status_t smf_sbi_server_open()
{
    status_t rv;
    sock_id new;
    c_sockaddr_t addr_smContextCreate, addr_smContextUpdate, addr_smContextRelease, addr_smContextRetrieve;

    // Create csock for smContextCreate
    memcpy(addr_smContextCreate.sun_path, "/tmp/smf_smContextCreate.csock", sizeof(addr_smContextCreate.sun_path));
    addr_smContextCreate.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&new, &addr_smContextCreate);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(new, _smf_sbi_message_smf_smContextCreate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    // Create csock for smContextUpdate
    memcpy(addr_smContextUpdate.sun_path, "/tmp/smf_smContextUpdate.csock", sizeof(addr_smContextUpdate.sun_path));
    addr_smContextUpdate.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&new, &addr_smContextUpdate);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(new, _smf_sbi_message_smf_smContextUpdate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    // Create csock for smContextRelease
    memcpy(addr_smContextRelease.sun_path, "/tmp/smf_smContextRelease.csock", sizeof(addr_smContextRelease.sun_path));
    addr_smContextRelease.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&new, &addr_smContextRelease);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(new, _smf_sbi_message_smf_smContextRelease, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    // Create csock for smContextRetrieve
    memcpy(addr_smContextRetrieve.sun_path, "/tmp/smf_smContextRetrieve.csock", sizeof(addr_smContextRetrieve.sun_path));
    addr_smContextRetrieve.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&new, &addr_smContextRetrieve);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(new, _smf_sbi_message_smf_smContextRetrieve, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    start_server();
    return CORE_OK;
}

status_t smf_sbi_server_close()
{
    kill(smf_self()->server_pid, SIGINT);
    return CORE_OK;
}
