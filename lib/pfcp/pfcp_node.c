#define TRACE_MODULE _pfcp_node

#include "core_debug.h"

#include "pfcp_conv.h"
#include "pfcp_node.h"
#include "pfcp_xact.h"

#define MAX_PFCP_NODE_POOL_SIZE          512

pool_declare(pfcp_node_pool, pfcp_node_t, MAX_PFCP_NODE_POOL_SIZE);

status_t pfcp_node_init(void)
{
    pool_init(&pfcp_node_pool, MAX_PFCP_NODE_POOL_SIZE);

    return CORE_OK;
}
status_t pfcp_node_final(void)
{
    if (pool_used(&pfcp_node_pool))
        d_error("%d not freed in pfcp_node_pool[%d]",
            pool_used(&pfcp_node_pool), pool_size(&pfcp_node_pool));
    d_trace(9, "%d not freed in pfcp_node_pool[%d]\n",
            pool_used(&pfcp_node_pool), pool_size(&pfcp_node_pool));
    pool_final(&pfcp_node_pool);

    return CORE_OK;
}

status_t pfcp_add_node(list_t *list, pfcp_node_t **node,
        c_sockaddr_t *all_list, int no_ipv4, int no_ipv6, int prefer_ipv4)
{
    status_t rv;
    pfcp_node_t *new_node = NULL;
    c_sockaddr_t *preferred_list = NULL;

    d_assert(list, return CORE_ERROR,);
    d_assert(all_list, return CORE_ERROR,);

    rv = core_copyaddrinfo(&preferred_list, all_list);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    if (no_ipv4 == 1)
    {
        rv = core_filteraddrinfo(&preferred_list, AF_INET6);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }
    if (no_ipv6 == 1)
    {
        rv = core_filteraddrinfo(&preferred_list, AF_INET);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }
    if (prefer_ipv4 == 1)
    {
        rv = core_sortaddrinfo(&preferred_list, AF_INET);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }
    else
    {
        rv = core_sortaddrinfo(&preferred_list, AF_INET6);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }

    if (preferred_list)
    {
        pool_alloc_node(&pfcp_node_pool, &new_node);
        d_assert(new_node, return CORE_ERROR,);
        memset(new_node, 0, sizeof(pfcp_node_t));

        new_node->sa_list = preferred_list;

        list_init(&new_node->local_list);
        list_init(&new_node->remote_list);

        new_node->tm_heartbeat = 0;

        list_append(list, new_node);
        new_node->state = PFCP_NODE_ST_NULL;
    }

    *node = new_node;

    return CORE_OK;
}

pfcp_node_t *pfcp_add_node_with_seid(list_t *list, pfcp_f_seid_t *f_seid,
        c_uint16_t port, int no_ipv4, int no_ipv6, int prefer_ipv4)
{
    status_t rv;
    pfcp_node_t *node = NULL;
    c_sockaddr_t *sa_list = NULL;

    d_assert(list, return NULL,);
    d_assert(f_seid, return NULL,);
    d_assert(port, return NULL,);

    rv = pfcp_f_seid_to_sockaddr(f_seid, port, &sa_list);
    d_assert(rv == CORE_OK, return NULL,);

    rv = pfcp_add_node(list, &node, sa_list, no_ipv4, no_ipv6, prefer_ipv4);
    d_assert(rv == CORE_OK, return NULL,);
    d_assert(node, return NULL,);

    rv = pfcp_f_seid_to_ip(f_seid, &node->ip);
    d_assert(rv == CORE_OK, return NULL,);

    rv = sock_fill_scope_id_in_local(node->sa_list);
    d_assert(rv == CORE_OK, return NULL,);

    core_freeaddrinfo(sa_list);

    return node;
}

status_t pfcp_remove_node(list_t *list, pfcp_node_t *node)
{
    d_assert(node, return CORE_ERROR,);

    list_remove(list, node);

    if (node->sock)
        sock_delete(node->sock);

    if (node->tm_heartbeat)
        tm_delete(node->tm_heartbeat);

    pfcp_xact_delete_all(node);

    core_freeaddrinfo(node->sa_list);
    pool_free_node(&pfcp_node_pool, node);

    return CORE_OK;
}

status_t pfcp_remove_all_nodes(list_t *list)
{
    pfcp_node_t *node = NULL, *next_node = NULL;
    
    node = list_first(list);
    while(node)
    {
        next_node = list_next(node);

        pfcp_remove_node(list, node);

        node = next_node;
    }

    return CORE_OK;
}

pfcp_node_t* pfcp_find_node(list_t *list, pfcp_f_seid_t *f_seid)
{
    status_t rv;
    pfcp_node_t *node = NULL;
    ip_t ip;

    d_assert(list, return NULL,);
    d_assert(f_seid, return NULL,);

    rv = pfcp_f_seid_to_ip(f_seid, &ip);
    d_assert(rv == CORE_OK, return NULL,);

    node = list_first(list);
    while (node)
    {
        if (memcmp(&node->ip, &ip, ip.len) == 0)
            break;

        node = list_next(node);
    }

    return node;
}


pfcp_node_t* pfcp_find_node_sockaddr(list_t *list, c_sockaddr_t *from)
{
    //status_t rv;
    pfcp_node_t *node = NULL;
    //ip_t ip;
    c_sockaddr_t *addr;

    d_assert(list, return NULL,);
    d_assert(from, return NULL,);

    //rv = pfcp_f_seid_to_ip(f_seid, &ip);
    //d_assert(rv == CORE_OK, return NULL,);
    d_assert(from->sin.sin_family == AF_INET, return NULL,);

    node = list_first(list);
    while (node)
    {
#if 0
        //from->sin6.sin6_addr.s6_addr
        //$ fixme: only ipv4 now        
        if(node->ip.ipv4 && node->ip.addr == from->sin.sin_addr.s_addr)
        {
            break;
        }
#else
        for (addr = node->sa_list; addr; addr=addr->next)
        {
            if (addr->sin.sin_family == from->sin.sin_family)
            {
                if (addr->sin.sin_addr.s_addr == from->sin.sin_addr.s_addr)
                {
                    return node;
                    break;
                }
            }            
        }
#endif
        node = list_next(node);
    }

    return node;
}
