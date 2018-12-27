#define TRACE_MODULE _core_unixsock
#include "core_network.h"
#include "core_debug.h"

#include "3gpp_types.h"

#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

status_t unixgram_socket(sock_id* new)
{
    status_t rv;

    rv = sock_socket(new, AF_UNIX, SOCK_DGRAM, PF_UNIX);
    d_assert(rv == CORE_OK && new, return CORE_ERROR,);

    d_trace(1, "unix domain socket\n");

    return CORE_OK;
}

status_t unixgram_server(sock_id *new, c_sockaddr_t *l_addr)
{
    status_t rv;

    d_assert(new, return CORE_ERROR,);
    d_assert(l_addr, return CORE_ERROR,);

    rv = unixgram_socket(new);
    if (rv == CORE_OK)
    {
        unlink(l_addr->sun_path);
        d_assert(sock_setsockopt(*new, SOCK_O_REUSEADDR, 1) == CORE_OK,
            return CORE_ERROR,
                "setsockopt [%s] failed(%d:%s)",
                l_addr->sun_path,
                errno, strerror(errno));

        if (sock_bind(*new, l_addr) == CORE_OK)
        {
            d_trace(1, "unixgram_server() [%s]\n",
                    l_addr);
            return CORE_OK;
        }

        rv = sock_delete(*new);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }
    return CORE_OK;
}

status_t unixgram_recvfrom(sock_id sock, pkbuf_t **pkbuf, c_sockaddr_t *from)
{
    ssize_t size;

    d_assert(sock, return CORE_ERROR,);
    d_assert(from, return CORE_ERROR,);

    *pkbuf = pkbuf_alloc(0, MAX_SBI_MESSAGE_LEN);
    if ((*pkbuf) == NULL)
    {
        char tmp_buf[MAX_SBI_MESSAGE_LEN];

        d_fatal("Can't allocate pkbuf");

        /* Read data from socket to exit from select */
        core_recv(sock, tmp_buf, MAX_SBI_MESSAGE_LEN, 0);

        return CORE_ERROR;
    }
    size = core_recvfrom(sock, (*pkbuf)->payload, (*pkbuf)->len, 0, from);
    if (size <= 0)
    {
        pkbuf_free((*pkbuf));

        if (errno != EAGAIN)
        {
            d_warn("net_read failed(%d:%s)", errno, strerror(errno));
        }

        return CORE_ERROR;
    }
    else
    {
        (*pkbuf)->len = size;

        return CORE_OK;;
    }
    return CORE_OK;
}

status_t unixgram_sendto(sock_id sock, pkbuf_t *pkbuf, c_sockaddr_t *to)
{
    ssize_t sent;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    d_assert(sock, return CORE_ERROR, "Null param");

    sent = core_sendto(sock, pkbuf->payload, pkbuf->len, 0, to);
    if (sent < 0 || sent != pkbuf->len)
    {
        d_error("core_sendto [%s] failed(%d:%s)",
            to->sun_path, errno, strerror(errno));
        return CORE_ERROR;
    }

    return CORE_OK;
}
