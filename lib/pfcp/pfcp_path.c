#define TRACE_MODULE _pfcp_path

#include "core_debug.h"
#include "core_pkbuf.h"

#include "3gpp_types.h"
#include "pfcp_message.h"
#include "pfcp_conv.h"
#include "pfcp_node.h"

#include "pfcp_path.h"

status_t pfcp_server(sock_node_t *snode, sock_handler handler)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];

    d_assert(snode, return CORE_ERROR,);

    rv = udp_server(&snode->sock, snode->list);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    rv = sock_register(snode->sock, handler, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    d_trace(1, "pfcp_server() [%s]:%d\n",
            CORE_ADDR(snode->list, buf), CORE_PORT(snode->list));

    return CORE_OK;
}

status_t pfcp_client(pfcp_node_t *gnode)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];

    d_assert(gnode, return CORE_ERROR,);

    rv = udp_client(&gnode->sock, gnode->sa_list);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    d_trace(1, "pfcp_client() [%s]:%d\n",
            CORE_ADDR(gnode->sa_list, buf), CORE_PORT(gnode->sa_list));

    return CORE_OK;
}

status_t pfcp_connect(sock_id ipv4, sock_id ipv6, pfcp_node_t *gnode)
{
    c_sockaddr_t *addr;
    char buf[CORE_ADDRSTRLEN];

    d_assert(ipv4 || ipv6, return CORE_ERROR,);
    d_assert(gnode, return CORE_ERROR,);
    d_assert(gnode->sa_list, return CORE_ERROR,);

    addr = gnode->sa_list;
    while(addr)
    {
        sock_id id;

        if (addr->c_sa_family == AF_INET) id = ipv4;
        else if (addr->c_sa_family == AF_INET6) id = ipv6;
        else
            d_assert(0, return CORE_ERROR,);

        if (id)
        {
            if (sock_connect(id, addr) == CORE_OK)
            {
                d_trace(1, "pfcp_connect() [%s]:%d\n",
                        CORE_ADDR(addr, buf), CORE_PORT(addr));

                gnode->sock = id;
                break;
            }
        }

        addr = addr->next;
    }

    if (addr == NULL)
    {
        d_warn("pfcp_connect() [%s]:%d failed(%d:%s)",
                CORE_ADDR(gnode->sa_list, buf), CORE_PORT(gnode->sa_list),
                errno, strerror(errno));
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t pfcp_server_list(list_t *list, sock_handler handler)
{
    status_t rv;
    sock_node_t *snode = NULL;

    d_assert(list, return CORE_ERROR,);
    d_assert(handler, return CORE_ERROR,);

    for (snode = list_first(list); snode; snode = list_next(snode))
    {
        rv = pfcp_server(snode, handler);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }

    return CORE_OK;
}

sock_id pfcp_local_sock_first(list_t *list)
{
    sock_node_t *snode = NULL;
    sock_id sock = 0;

    d_assert(list, return 0,);

    for (snode = list_first(list); snode; snode = list_next(snode))
    {
        sock = snode->sock;
        if (sock) return sock;
    }

    return 0;
}

c_sockaddr_t *pfcp_local_addr_first(list_t *list)
{
    sock_node_t *snode = NULL;
    c_sockaddr_t *addr = NULL;

    d_assert(list, return NULL,);

    for (snode = list_first(list); snode; snode = list_next(snode))
    {
        addr = sock_local_addr(snode->sock);
        if (addr) return addr;
    }

    return NULL;
}

status_t pfcp_recv(sock_id sock, pkbuf_t **pkbuf)
{
    ssize_t size;

    d_assert(sock, return CORE_ERROR,);

    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if ((*pkbuf) == NULL)
    {
        char tmp_buf[MAX_SDU_LEN];

        d_fatal("Can't allocate pkbuf");

        /* Read data from socket to exit from select */
        core_recv(sock, tmp_buf, MAX_SDU_LEN, 0);

        return CORE_ERROR;
    }

    size = core_recv(sock, (*pkbuf)->payload, (*pkbuf)->len, 0);
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
}

status_t pfcp_recvfrom(sock_id sock, pkbuf_t **pkbuf, c_sockaddr_t *from)
{
    ssize_t size;

    d_assert(sock, return CORE_ERROR,);
    d_assert(from, return CORE_ERROR,);

    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if ((*pkbuf) == NULL)
    {
        char tmp_buf[MAX_SDU_LEN];

        d_fatal("Can't allocate pkbuf");

        /* Read data from socket to exit from select */
        core_recv(sock, tmp_buf, MAX_SDU_LEN, 0);

        return CORE_ERROR;
    }

    size = core_recvfrom(sock, (*pkbuf)->payload, (*pkbuf)->len, 0, from);
    if (size <= 0)
    {
        pkbuf_free((*pkbuf));

        if (errno != EAGAIN)
        {
            d_warn("core_recv failed(%d:%s)", errno, strerror(errno));
        }

        return CORE_ERROR;
    }
    else
    {
        (*pkbuf)->len = size;

        return CORE_OK;;
    }
}

status_t pfcp_send(pfcp_node_t *gnode, pkbuf_t *pkbuf)
{
    char buf[CORE_ADDRSTRLEN];
    ssize_t sent;
    sock_id sock = 0;
    c_sockaddr_t *addr = NULL;

    d_assert(gnode, return CORE_ERROR, "Null param");
    d_assert(pkbuf, return CORE_ERROR, "Null param");
    sock = gnode->sock;
    d_assert(sock, return CORE_ERROR, "Null param");

    /* New interface */
    sock = gnode->sock;
    d_assert(sock, return CORE_ERROR,);
    addr = sock_remote_addr(sock);
    d_assert(addr, return CORE_ERROR,);

    //sent = core_send(sock, pkbuf->payload, pkbuf->len, 0);
    sent = core_sendto(sock, pkbuf->payload, pkbuf->len, 0, gnode->sa_list);
    if (sent < 0 || sent != pkbuf->len)
    {
        d_error("core_send [%s]:%d failed(%d:%s)",
            CORE_ADDR(addr, buf), CORE_PORT(addr), errno, strerror(errno));
        return CORE_ERROR;
    }

    return CORE_OK;
}

pkbuf_t *pfcp_handle_heartbeat_req(pkbuf_t *pkb, c_uint32_t recovery_time)
{
    pfcp_header_t *pfcph = NULL;
    pkbuf_t *pkb_resp = NULL;
    pfcp_header_t *pfcph_resp = NULL;
    c_uint16_t length;
    int idx;

    d_assert(pkb, return NULL, "pkt is NULL");

    pfcph = (pfcp_header_t *)pkb->payload;
    /* Check PFCP version. Now only support PFCPv1(version = 1) */
    if ((pfcph->flags >> 5) != 1)
    {
        return NULL;
    }

    if (pfcph->type != PFCP_HEARTBEAT_REQUEST_TYPE)
    {
        return NULL;
    }


    pkb_resp = pkbuf_alloc(0, 100 /* enough for HEARTBEAT_RSP; use smaller buffer */);
    d_assert(pkb_resp, return NULL, "Can't allocate pkbuf");
    pfcph_resp = (pfcp_header_t *)pkb_resp->payload;

    /* reply back immediately */
    pfcph_resp->flags = (PFCP_VERSION << 5); /* set version */
    //pfcph_resp->flags |= (1 << 4); /* set PT */
    pfcph_resp->type = PFCP_HEARTBEAT_RESPONSE_TYPE;
    length = 0;     /* length of Recovery IE */
    pfcph_resp->length = htons(length); /* to be overwriten */
    pfcph_resp->sqn_only = pfcph->sqn_only;
    idx = 8;
#if 0
    if (pfcph->flags & (GTPU_FLAGS_PN | GTPU_FLAGS_S))
    {
        length += 4;
        if (pfcph->flags & GTPU_FLAGS_S)
        {
            /* sequence exists */
            pfcph_resp->flags |= GTPU_FLAGS_S;
            *((c_uint8_t *)pkb_resp->payload + idx) = *((c_uint8_t *)pkb->payload + idx);
            *((c_uint8_t *)pkb_resp->payload + idx + 1) = *((c_uint8_t *)pkb->payload + idx + 1);
        }
        else
        {
            *((c_uint8_t *)pkb_resp->payload + idx) = 0;
            *((c_uint8_t *)pkb_resp->payload + idx + 1) = 0;
        }
        idx += 2;
        if (pfcph->flags & GTPU_FLAGS_PN)
        {
            /* sequence exists */
            pfcph_resp->flags |= GTPU_FLAGS_PN;
            *((c_uint8_t *)pkb_resp->payload + idx) = *((c_uint8_t *)pkb->payload + idx);
        }
        else
        {
            *((c_uint8_t *)pkb_resp->payload + idx) = 0;
        }
        idx++;
        *((c_uint8_t *)pkb_resp->payload + idx) = 0; /* next-extension header */
        idx++;
    }
#endif    
    /* fill Recovery Time Stamp IE */
    //length += 8;
    *((c_uint8_t *)pkb_resp->payload + idx) = 0; idx++; /* type */
    *((c_uint8_t *)pkb_resp->payload + idx) = 96; idx++; /* type */
    *((c_uint8_t *)pkb_resp->payload + idx) = 0; idx++; /* length */
    *((c_uint8_t *)pkb_resp->payload + idx) = 4; idx++; /* length */
    *((c_uint8_t *)pkb_resp->payload + idx) = recovery_time & 0xFF; idx++; /* Recovery Time Stamp value */
    *((c_uint8_t *)pkb_resp->payload + idx) = (recovery_time >> 8) & 0xFF; idx++; /* Recovery Time Stamp value */
    *((c_uint8_t *)pkb_resp->payload + idx) = (recovery_time >> 16) & 0xFF; idx++; /* Recovery Time Stamp value */
    *((c_uint8_t *)pkb_resp->payload + idx) = (recovery_time >> 24) & 0xFF; idx++; /* Recovery Time Stamp value */

    length = idx - 4;
    pfcph_resp->length = htons(length);
    pkb_resp->len = idx;                /* buffer length */

    return pkb_resp;
}

#include "pfcp_xact.h"

status_t pfcp_start_heartbeat(pfcp_node_t *gnode, c_uint32_t recovery_time)
{
    pkbuf_t *pkb_req = NULL;
    //pfcp_header_t *pfcph_reqp = NULL;
    //c_uint16_t length;
    int idx;
    status_t rt;

    //pkb_req = pkbuf_alloc(0, 100 /* enough for HEARTBEAT_REQ; use smaller buffer */);
    pkb_req = pkbuf_alloc(32, 4 /* enough for HEARTBEAT_REQ; use smaller buffer */);
    d_assert(pkb_req, return CORE_ERROR, "Can't allocate pkbuf");
    //pfcph_reqp = (pfcp_header_t *)pkb_req->payload;

    #if 0
    /* reply back immediately */
    pfcph_reqp->flags = (PFCP_VERSION << 5); /* set version */
    //pfcph_resp->flags |= (1 << 4); /* set PT */
    pfcph_reqp->type = PFCP_HEARTBEAT_REQUEST_TYPE;
    length = 0;     /* length of Recovery IE */
    pfcph_reqp->length = htons(length); /* to be overwriten */
    pfcph_reqp->sqn_only = 0;
    idx = 8;
    #else
    idx = 0;
    #endif
    

    /* fill Recovery Time Stamp IE */
    //length += 4;
    *((c_uint8_t *)pkb_req->payload + idx) = 0; idx++; /* type */
    *((c_uint8_t *)pkb_req->payload + idx) = 96; idx++; /* type */
    *((c_uint8_t *)pkb_req->payload + idx) = 0; idx++; /* length */
    *((c_uint8_t *)pkb_req->payload + idx) = 4; idx++; /* length */
    *((c_uint8_t *)pkb_req->payload + idx) = recovery_time & 0xFF; idx++; /* Recovery Time Stamp value */
    *((c_uint8_t *)pkb_req->payload + idx) = (recovery_time >> 8) & 0xFF; idx++; /* Recovery Time Stamp value */
    *((c_uint8_t *)pkb_req->payload + idx) = (recovery_time >> 16) & 0xFF; idx++; /* Recovery Time Stamp value */
    *((c_uint8_t *)pkb_req->payload + idx) = (recovery_time >> 24) & 0xFF; idx++; /* Recovery Time Stamp value */

    //length = idx - 4;
    //pfcph_reqp->length = htons(length);
    pkb_req->len = idx;                /* buffer length */
    
    
    pfcp_xact_t *n4_xact;
    pfcp_header_t h;
    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_HEARTBEAT_REQUEST_TYPE;
    h.seid = 0; //sess->upf_teid;
    
    n4_xact = pfcp_xact_local_create(gnode, &h, pkb_req);
    d_assert(n4_xact, return CORE_ERROR, "Null param");
    
    rt = pfcp_xact_commit(n4_xact);
    d_assert(rt == CORE_OK, return rt, "xact_commit error");
    //rt = pfcp_send(gnode, pkb_req);
    //pkbuf_free(pkb_req);
    return rt;
}