#define TRACE_MODULE _pfcp_conv

#include "core_debug.h"
#include "core_network.h"

#include "pfcp_message.h"
#include "pfcp_types.h"
#include "pfcp_conv.h"



status_t pfcp_f_seid_to_sockaddr(
    pfcp_f_seid_t *f_seid, c_uint16_t port, c_sockaddr_t **list)
{
    c_sockaddr_t *addr = NULL, *addr6 = NULL;

    d_assert(f_seid, return CORE_ERROR,);
    d_assert(list, return CORE_ERROR,);

    addr = core_calloc(1, sizeof(c_sockaddr_t));
    d_assert(addr, return CORE_ERROR,);
    addr->c_sa_family = AF_INET;
    addr->c_sa_port = htons(port);

    addr6 = core_calloc(1, sizeof(c_sockaddr_t));
    d_assert(addr6, return CORE_ERROR,);
    addr6->c_sa_family = AF_INET6;
    addr6->c_sa_port = htons(port);

    if (f_seid->ipv4 && f_seid->ipv6)
    {
        addr->next = addr6;

        addr->sin.sin_addr.s_addr = f_seid->both.addr;
        memcpy(addr6->sin6.sin6_addr.s6_addr, f_seid->both.addr6, IPV6_LEN);

        *list = addr;
    }
    else if (f_seid->ipv4)
    {
        addr->sin.sin_addr.s_addr = f_seid->addr;
        CORE_FREE(addr6);

        *list = addr;
    }
    else if (f_seid->ipv6)
    {
        memcpy(addr6->sin6.sin6_addr.s6_addr, f_seid->addr6, IPV6_LEN);
        CORE_FREE(addr);

        *list = addr6;
    }
    else
    {
        CORE_FREE(addr);
        CORE_FREE(addr6);
        d_assert(0, return CORE_ERROR,);
    }

    return CORE_OK;
}

status_t pfcp_sockaddr_to_f_seid(
    c_sockaddr_t *addr, c_sockaddr_t *addr6, pfcp_f_seid_t *f_seid, int *len)
{
    d_assert(f_seid, return CORE_ERROR,);

    if (addr && addr6)
    {
        f_seid->ipv4 = 1;
        f_seid->both.addr = addr->sin.sin_addr.s_addr;
        f_seid->ipv6 = 1;
        memcpy(f_seid->both.addr6, addr6->sin6.sin6_addr.s6_addr, IPV6_LEN);
        *len = PFCP_F_SEID_IPV4V6_LEN;
    }
    else if (addr)
    {
        f_seid->ipv4 = 1;
        f_seid->ipv6 = 0;
        f_seid->addr = addr->sin.sin_addr.s_addr;
        *len = PFCP_F_SEID_IPV4_LEN;
    }
    else if (addr6)
    {
        f_seid->ipv4 = 0;
        f_seid->ipv6 = 1;
        memcpy(f_seid->addr6, addr6->sin6.sin6_addr.s6_addr, IPV6_LEN);
        *len = PFCP_F_SEID_IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t pfcp_f_seid_to_ip(pfcp_f_seid_t *f_seid, ip_t *ip)
{
    d_assert(ip, return CORE_ERROR,);
    d_assert(f_seid, return CORE_ERROR,);

    memset(ip, 0, sizeof(ip_t));

    ip->ipv4 = f_seid->ipv4;
    ip->ipv6 = f_seid->ipv6;

    if (ip->ipv4 && ip->ipv6)
    {
        ip->both.addr = f_seid->both.addr;
        memcpy(ip->both.addr6, f_seid->both.addr6, IPV6_LEN);
        ip->len = IPV4V6_LEN;
    }
    else if (ip->ipv4)
    {
        ip->addr = f_seid->addr;
        ip->len = IPV4_LEN;
    }
    else if (ip->ipv6)
    {
        memcpy(ip->addr6, f_seid->addr6, IPV6_LEN);
        ip->len = IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t pfcp_ip_to_f_seid(ip_t *ip, pfcp_f_seid_t *f_seid, int *len)
{
    d_assert(ip, return CORE_ERROR,);
    d_assert(f_seid, return CORE_ERROR,);

    f_seid->ipv4 = ip->ipv4;
    f_seid->ipv6 = ip->ipv6;

    if (f_seid->ipv4 && f_seid->ipv6)
    {
        f_seid->both.addr = ip->both.addr;
        memcpy(f_seid->both.addr6, ip->both.addr6, IPV6_LEN);
        *len = PFCP_F_SEID_IPV4V6_LEN;
    }
    else if (f_seid->ipv4)
    {
        f_seid->addr = ip->addr;
        *len = PFCP_F_SEID_IPV4_LEN;
    }
    else if (f_seid->ipv6)
    {
        memcpy(f_seid->addr6, ip->addr6, IPV6_LEN);
        *len = PFCP_F_SEID_IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t pfcp_ip_to_f_teid(ip_t *ip, pfcp_f_teid_t *f_teid, int *len)
{
    d_assert(ip, return CORE_ERROR,);
    d_assert(f_teid, return CORE_ERROR,);

    f_teid->ipv4 = ip->ipv4;
    f_teid->ipv6 = ip->ipv6;

    if (f_teid->ipv4 && f_teid->ipv6)
    {
        f_teid->both.addr = ip->both.addr;
        memcpy(f_teid->both.addr6, ip->both.addr6, IPV6_LEN);
        *len = PFCP_F_TEID_IPV4V6_LEN;
    }
    else if (f_teid->ipv4)
    {
        f_teid->addr = ip->addr;
        *len = PFCP_F_TEID_IPV4_LEN;
    }
    else if (f_teid->ipv6)
    {
        memcpy(f_teid->addr6, ip->addr6, IPV6_LEN);
        *len = PFCP_F_TEID_IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t pfcp_ip_to_ue_ip_addr(ip_t *ip, pfcp_ue_ip_addr_t *ue_ip, int *len)
{
    d_assert(ip, return CORE_ERROR,);
    d_assert(ue_ip, return CORE_ERROR,);

    ue_ip->ipv4 = ip->ipv4;
    ue_ip->ipv6 = ip->ipv6;

    if (ue_ip->ipv4 && ue_ip->ipv6)
    {
        ue_ip->both.addr = ip->both.addr;
        memcpy(ue_ip->both.addr6, ip->both.addr6, IPV6_LEN);
        *len = PFCP_UE_IP_ADDR_IPV4V6_LEN;
    }
    else if (ue_ip->ipv4)
    {
        ue_ip->addr = ip->addr;
        *len = PFCP_UE_IP_ADDR_IPV4_LEN;
    }
    else if (ue_ip->ipv6)
    {
        memcpy(ue_ip->addr6, ip->addr6, IPV6_LEN);
        *len = PFCP_UE_IP_ADDR_IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t pfcp_outer_hdr_to_ip(pfcp_outer_hdr_t *outer_hdr, ip_t *ip)
{
    d_assert(ip, return CORE_ERROR,);
    d_assert(outer_hdr, return CORE_ERROR,);

    memset(ip, 0, sizeof(ip_t));
    
    ip->ipv4 = outer_hdr->gtpu_ipv4;
    ip->ipv6 = outer_hdr->gtpu_ipv6;

    if (outer_hdr->gtpu_ipv4 && outer_hdr->gtpu_ipv6)
    {
        ip->both.addr = outer_hdr->both.addr;
        memcpy(ip->both.addr6, outer_hdr->both.addr6, IPV6_LEN);
        ip->len = IPV4V6_LEN;
    }
    else if (outer_hdr->gtpu_ipv4)
    {
        ip->addr = outer_hdr->addr;
        ip->len = IPV4_LEN;
    }
    else if (outer_hdr->gtpu_ipv6)
    {
        memcpy(ip->addr6, outer_hdr->addr6, IPV6_LEN);
        ip->len = IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t pfcp_sockaddr_to_f_teid(
    c_sockaddr_t *addr, c_sockaddr_t *addr6, pfcp_f_teid_t *f_teid, int *len)
{
    d_assert(f_teid, return CORE_ERROR,);

    if (addr && addr6)
    {
        f_teid->ipv4 = 1;
        f_teid->both.addr = addr->sin.sin_addr.s_addr;
        f_teid->ipv6 = 1;
        memcpy(f_teid->both.addr6, addr6->sin6.sin6_addr.s6_addr, IPV6_LEN);
        *len = PFCP_F_TEID_IPV4V6_LEN;
    }
    else if (addr)
    {
        f_teid->ipv4 = 1;
        f_teid->ipv6 = 0;
        f_teid->addr = addr->sin.sin_addr.s_addr;
        *len = PFCP_F_TEID_IPV4_LEN;
    }
    else if (addr6)
    {
        f_teid->ipv4 = 0;
        f_teid->ipv6 = 1;
        memcpy(f_teid->addr6, addr6->sin6.sin6_addr.s6_addr, IPV6_LEN);
        *len = PFCP_F_TEID_IPV6_LEN;
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}


