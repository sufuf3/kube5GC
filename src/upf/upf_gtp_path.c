#define TRACE_MODULE _upf_gtp_path
#include "core_debug.h"
#include "core_pkbuf.h"

#include "3gpp_types.h"
#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"

#include "app/context.h"
#include "upf_context.h"
#include "upf_event.h"
#include "upf_gtp_path.h"
#include "upf_ipfw.h"

#define UPF_GTP_HANDLED 1

c_uint16_t in_cksum(c_uint16_t *addr, int len);
static status_t upf_gtp_handle_multicast(pkbuf_t *recvbuf);
static status_t upf_gtp_handle_slaac(upf_sess_t *sess, pkbuf_t *recvbuf);
static status_t upf_gtp_send_to_pdr(upf_pdr_t *pdr, pkbuf_t *sendbuf);
//static status_t upf_gtp_send_router_advertisement(
//        upf_sess_t *sess, c_uint8_t *ip6_dst);

static int _gtpv1_tun_recv_cb(sock_id sock, void *data)
{
    pkbuf_t *recvbuf = NULL;
    int n;
    status_t rv;
    upf_pdr_t *pdr = NULL;

    recvbuf = pkbuf_alloc(GTPV1U_HEADER_LEN, MAX_SDU_LEN);
    d_assert(recvbuf, return -1, "pkbuf_alloc error");

    n = sock_read(sock, recvbuf->payload, recvbuf->len);
    if (n <= 0)
    {
        pkbuf_free(recvbuf);
        return -1;
    }

    recvbuf->len = n;

    d_trace(50, "[TUN] RECV : ");
    d_trace_hex(50, recvbuf->payload, recvbuf->len);

    /* Find the bearer by packet filter */
    pdr = upf_pdr_find_by_packet(recvbuf);
    if (pdr)
    {
        /* Unicast */
        rv = upf_gtp_send_to_pdr(pdr, recvbuf);
        d_assert(rv == CORE_OK, , "upf_gtp_send_to_pdr() failed");
    }
    else
    {
        if (context_self()->parameter.multicast)
        {
            rv = upf_gtp_handle_multicast(recvbuf);
            d_assert(rv != CORE_ERROR, , "upf_gtp_handle_multicast() failed");
        }
    }

    pkbuf_free(recvbuf);
    return 0;
}

static int _gtpv1_u_recv_cb(sock_id sock, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_uint32_t size = GTPV1U_HEADER_LEN;
    gtp_header_t *gtp_h = NULL;
    struct ip *ip_h = NULL;

    char buf[CORE_ADDRSTRLEN];
    c_sockaddr_t from;

    c_uint32_t teid;
    upf_pdr_t *pdr = NULL;
    upf_sess_t *sess = NULL;
    upf_subnet_t *subnet = NULL;
    upf_dev_t *dev = NULL;

    d_assert(sock, return -1, "Null param");

    rv = gtp_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK)
    {
        if (errno == EAGAIN)
            return 0;

        return -1;
    }

    d_assert(pkbuf, return 0, );
    d_assert(pkbuf->payload, goto cleanup, );
    d_trace(50, "[UPF] RECV : ");
    d_trace_hex(50, pkbuf->payload, pkbuf->len);

    gtp_h = pkbuf->payload;
    if (gtp_h->flags & GTPU_FLAGS_S)
        size += 4;

    if (gtp_h->type == GTPU_MSGTYPE_ECHO_REQ)
    {
        pkbuf_t *echo_rsp;

        d_trace(3, "[UPF] RECV Echo Request from [%s]\n",
                CORE_ADDR(&from, buf));
        echo_rsp = gtp_handle_echo_req(pkbuf);
        if (echo_rsp)
        {
            ssize_t sent;

            /* Echo reply */
            d_trace(3, "[UPF] SEND Echo Response to [%s]\n",
                    CORE_ADDR(&from, buf));

            sent = core_sendto(sock,
                               echo_rsp->payload, echo_rsp->len, 0, &from);
            if (sent < 0 || sent != echo_rsp->len)
            {
                d_error("core_sendto failed(%d:%s)", errno, strerror(errno));
            }
            pkbuf_free(echo_rsp);
        }
    }
    else if (gtp_h->type == GTPU_MSGTYPE_GPDU ||
             gtp_h->type == GTPU_MSGTYPE_END_MARKER)
    {
        teid = ntohl(gtp_h->teid);

        d_trace(3, "[UPF] RECV GPU-U from ENB : TEID[0x%x]\n", teid);

        /* Remove GTP header and send packets to TUN interface */
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, goto cleanup, );

        ip_h = pkbuf->payload;
        d_assert(ip_h, goto cleanup, );

        pdr = upf_pdr_find_by_upf_s1u_teid(teid);
        d_assert(pdr, goto cleanup, );
        sess = pdr->sess;
        d_assert(sess, goto cleanup, );

        if (ip_h->ip_v == 4 && sess->ipv4)
            subnet = sess->ipv4->subnet;
        else if (ip_h->ip_v == 6 && sess->ipv6)
            subnet = sess->ipv6->subnet;

        if (!subnet)
        {
            d_trace_hex(9, pkbuf->payload, pkbuf->len);
            d_trace(9, "[DROP] Cannot find subnet V:%d, IPv4:%p, IPv6:%p\n",
                    ip_h->ip_v, sess->ipv4, sess->ipv6);
            goto cleanup;
        }

        /* Check IPv6 */
        if (context_self()->parameter.no_slaac == 0 && ip_h->ip_v == 6)
        {
            rv = upf_gtp_handle_slaac(sess, pkbuf);
            if (rv == UPF_GTP_HANDLED)
            {
                pkbuf_free(pkbuf);
                return 0;
            }
            d_assert(rv == CORE_OK, , "upf_gtp_handle_slaac() failed");
        }

        dev = subnet->dev;
        d_assert(dev, goto cleanup, );
        if (sock_write(dev->sock, pkbuf->payload, pkbuf->len) <= 0)
            d_error("sock_write() failed");
    }

cleanup:
    pkbuf_free(pkbuf);
    return 0;
}

status_t upf_gtp_open()
{
    status_t rv;
    upf_dev_t *dev = NULL;
    upf_subnet_t *subnet = NULL;
    int rc;

    rv = gtp_server_list(&upf_self()->gtpu_list, _gtpv1_u_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR, );
    rv = gtp_server_list(&upf_self()->gtpu_list6, _gtpv1_u_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR, );

    upf_self()->gtpu_sock = gtp_local_sock_first(&upf_self()->gtpu_list);
    upf_self()->gtpu_sock6 = gtp_local_sock_first(&upf_self()->gtpu_list6);
    upf_self()->gtpu_addr = gtp_local_addr_first(&upf_self()->gtpu_list);
    upf_self()->gtpu_addr6 = gtp_local_addr_first(&upf_self()->gtpu_list6);

    d_assert(upf_self()->gtpu_addr || upf_self()->gtpu_addr6,
             return CORE_ERROR, "No GTP Server");

    /* NOTE : tun device can be created via following command.
     *
     * $ sudo ip tuntap add name uptun mode tun
     *
     * Also, before running upf, assign the one IP from IP pool of UE 
     * to uptun. The IP should not be assigned to UE
     *
     * $ sudo ifconfig uptun 45.45.0.1/16 up
     *
     */

    /* Open Tun interface */
    for (dev = upf_dev_first(); dev; dev = upf_dev_next(dev))
    {
        rc = tun_open(&dev->sock, (char *)dev->ifname, 0);
        if (rc != 0)
        {
            d_error("tun_open(dev:%s) failed", dev->ifname);
            return CORE_ERROR;
        }

        rc = sock_register(dev->sock, _gtpv1_tun_recv_cb, NULL);
        if (rc != 0)
        {
            d_error("sock_register(dev:%s) failed", dev->ifname);
            sock_delete(dev->sock);
            return CORE_ERROR;
        }
    }

    /* 
     * On Linux, it is possible to create a persistent tun/tap 
     * interface which will continue to exist even if free5gc quit, 
     * although this is normally not required. 
     * It can be useful to set up a tun/tap interface owned 
     * by a non-root user, so free5gc can be started without 
     * needing any root privileges at all.
     */

    /* Set P-to-P IP address with Netmask
     * Note that Linux will skip this configuration */
    for (subnet = upf_subnet_first(); subnet; subnet = upf_subnet_next(subnet))
    {
        d_assert(subnet->dev, return CORE_ERROR, );
        rc = tun_set_ip(subnet->dev->sock, &subnet->gw, &subnet->sub);
        if (rc != 0)
        {
            d_error("tun_set_ip(dev:%s) failed", subnet->dev->ifname);
            return CORE_ERROR;
        }
    }

    /* Link-Local Address for UPF_TUN */
    for (dev = upf_dev_first(); dev; dev = upf_dev_next(dev))
        dev->link_local_addr = core_link_local_addr_by_dev(dev->ifname);

    return CORE_OK;
}

status_t upf_gtp_close()
{
    upf_dev_t *dev = NULL;

    sock_delete_list(&upf_self()->gtpu_list);
    sock_delete_list(&upf_self()->gtpu_list6);

    for (dev = upf_dev_first(); dev; dev = upf_dev_next(dev))
        sock_delete(dev->sock);

    return CORE_OK;
}

static status_t upf_gtp_handle_multicast(pkbuf_t *recvbuf)
{
#if 0
    status_t rv;
    struct ip *ip_h =  NULL;
    struct ip6_hdr *ip6_h =  NULL;

    ip_h = (struct ip *)recvbuf->payload;
    if (ip_h->ip_v == 6)
    {
#if COMPILE_ERROR_IN_MAC_OS_X /* Compiler error in Mac OS X platform */
        ip6_h = (struct ip6_hdr *)recvbuf->payload;
        if (IN6_IS_ADDR_MULTICAST(&ip6_h->ip6_dst))
#else
        struct in6_addr ip6_dst;
        ip6_h = (struct ip6_hdr *)recvbuf->payload;
        memcpy(&ip6_dst, &ip6_h->ip6_dst, sizeof(struct in6_addr));
        if (IN6_IS_ADDR_MULTICAST(&ip6_dst))
#endif
        {
            hash_index_t *hi = NULL;

            /* IPv6 Multicast */
            for (hi = upf_sess_first(); hi; hi = upf_sess_next(hi))
            {
                upf_sess_t *sess = upf_sess_this(hi);
                d_assert(sess, return CORE_ERROR,);
                if (sess->ipv6)
                {
                    /* PDN IPv6 is avaiable */
                    upf_bearer_t *bearer = upf_default_bearer_in_sess(sess);
                    d_assert(bearer, return CORE_ERROR,);

                    //rv = upf_gtp_send_to_bearer(bearer, recvbuf);
                    d_assert(rv == CORE_OK,,
                            "upf_gtp_send_to_bearer failed");

                    return UPF_GTP_HANDLED;
                }
            }
        }
    }
#endif
    return CORE_OK;
}

static status_t upf_gtp_handle_slaac(upf_sess_t *sess, pkbuf_t *recvbuf)
{
#if 0
    // status_t rv;
    // struct ip *ip_h = NULL;
    
    d_assert(sess, return CORE_ERROR,);
    d_assert(recvbuf, return CORE_ERROR,);
    d_assert(recvbuf->payload, return CORE_ERROR,);
    
    ip_h = (struct ip *)recvbuf->payload;
    if (ip_h->ip_v == 6)
    {
        struct ip6_hdr *ip6_h = (struct ip6_hdr *)recvbuf->payload;
        if (ip6_h->ip6_nxt == IPPROTO_ICMPV6)
        {
            struct icmp6_hdr *icmp_h =
                (struct icmp6_hdr *)(recvbuf->payload + sizeof(struct ip6_hdr));
            if (icmp_h->icmp6_type == ND_ROUTER_SOLICIT)
            {
                d_trace(5, "[UPF]      Router Solict\n");
                if (sess->ipv6)
                {
                    rv = upf_gtp_send_router_advertisement(
                            sess, ip6_h->ip6_src.s6_addr);
                    d_assert(rv == CORE_OK,,"send router advertisement failed");
                }
                return UPF_GTP_HANDLED;
            }
        }
    }
#endif
    return CORE_OK;
}

static status_t upf_gtp_send_to_pdr(upf_pdr_t *pdr, pkbuf_t *sendbuf)
{
    char buf[CORE_ADDRSTRLEN];
    status_t rv;
    gtp_header_t *gtp_h = NULL;
    upf_far_t *far;

    d_assert(pdr, pkbuf_free(sendbuf); return CORE_ERROR, );
    d_assert(pdr->far, pkbuf_free(sendbuf); return CORE_ERROR, );
    far = pdr->far;
    d_assert(far->gnode, pkbuf_free(sendbuf); return CORE_ERROR, );
    d_assert(far->gnode->sock, pkbuf_free(sendbuf); return CORE_ERROR, );

    if (far->apply_action == PFCP_FAR_APPLY_ACTION_DROP)
    {
        return CORE_OK;
    }

    if (!far->upf_n3_teid)
    {
        return CORE_ERROR;
    }

    /* Add GTP-U header */
    rv = pkbuf_header(sendbuf, GTPV1U_HEADER_LEN);
    if (rv != CORE_OK)
    {
        d_error("pkbuf_header error");
        pkbuf_free(sendbuf);
        return CORE_ERROR;
    }

    gtp_h = (gtp_header_t *)sendbuf->payload;
    /* Bits    8  7  6  5  4  3  2  1
     *        +--+--+--+--+--+--+--+--+
     *        |version |PT| 1| E| S|PN|
     *        +--+--+--+--+--+--+--+--+
     *         0  0  1   1  0  0  0  0
     */
    gtp_h->flags = 0x30;
    gtp_h->type = GTPU_MSGTYPE_GPDU;
    gtp_h->length = htons(sendbuf->len - GTPV1U_HEADER_LEN);
    gtp_h->teid = htonl(far->upf_n3_teid);

    d_trace(50, "[UPF] SEND : ");
    d_trace_hex(50, sendbuf->payload, sendbuf->len);

    d_trace(3, "[UPF] SEND GPU-U to ENB[%s] : TEID[0x%x]\n",
            CORE_ADDR(sock_remote_addr(far->gnode->sock), buf),
            far->upf_n3_teid);
    rv = gtp_send(far->gnode, sendbuf);

    return rv;
}

#if 0
static status_t upf_gtp_send_router_advertisement(
        upf_sess_t *sess, c_uint8_t *ip6_dst)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;

    upf_bearer_t *bearer = NULL;
    upf_ue_ip_t *ue_ip = NULL;
    upf_subnet_t *subnet = NULL;
    upf_dev_t *dev = NULL;

    ipsubnet_t src_ipsub;
    c_uint16_t plen = 0;
    c_uint8_t nxt = 0;
    c_uint8_t *p = NULL;
    struct ip6_hdr *ip6_h =  NULL;
    struct nd_router_advert *advert_h = NULL;
    struct nd_opt_prefix_info *prefix = NULL;

    d_assert(sess, return CORE_ERROR,);
    bearer = upf_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR,);
    ue_ip = sess->ipv6;
    d_assert(ue_ip, return CORE_ERROR,);
    subnet = ue_ip->subnet;
    d_assert(subnet, return CORE_ERROR,);
    dev = subnet->dev;
    d_assert(dev, return CORE_ERROR,);

    pkbuf = pkbuf_alloc(GTPV1U_HEADER_LEN, 200);
    d_assert(pkbuf, return CORE_ERROR,);
    pkbuf->len = sizeof *ip6_h + sizeof *advert_h + sizeof *prefix;
    memset(pkbuf->payload, 0, pkbuf->len);

    p = (c_uint8_t *)pkbuf->payload;
    ip6_h = (struct ip6_hdr *)p;
    advert_h = (struct nd_router_advert *)((c_uint8_t *)ip6_h + sizeof *ip6_h);
    prefix = (struct nd_opt_prefix_info *)
        ((c_uint8_t*)advert_h + sizeof *advert_h);

    rv = core_ipsubnet(&src_ipsub, "fe80::1", NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    if (dev->link_local_addr)
        memcpy(src_ipsub.sub, dev->link_local_addr->sin6.sin6_addr.s6_addr,
                sizeof src_ipsub.sub);

    advert_h->nd_ra_type = ND_ROUTER_ADVERT;
    advert_h->nd_ra_code = 0;
    advert_h->nd_ra_curhoplimit = 64;
    advert_h->nd_ra_flags_reserved = 0;
    advert_h->nd_ra_router_lifetime = htons(64800);  /* 64800s */
    advert_h->nd_ra_reachable = 0;
    advert_h->nd_ra_retransmit = 0;

    prefix->nd_opt_pi_type = ND_OPT_PREFIX_INFORMATION;
    prefix->nd_opt_pi_len = 4; /* 32bytes */
    prefix->nd_opt_pi_prefix_len = subnet->prefixlen;
    prefix->nd_opt_pi_flags_reserved =
        ND_OPT_PI_FLAG_ONLINK|ND_OPT_PI_FLAG_AUTO;
    prefix->nd_opt_pi_valid_time = htonl(0xffffffff); /* Infinite */
    prefix->nd_opt_pi_preferred_time = htonl(0xffffffff); /* Infinite */
    memcpy(prefix->nd_opt_pi_prefix.s6_addr,
            subnet->sub.sub, sizeof prefix->nd_opt_pi_prefix.s6_addr);

    /* For IPv6 Pseudo-Header */
    plen = htons(sizeof *advert_h + sizeof *prefix);
    nxt = IPPROTO_ICMPV6;

    memcpy(p, src_ipsub.sub, sizeof src_ipsub.sub);
    p += sizeof src_ipsub.sub;
    memcpy(p, ip6_dst, IPV6_LEN);
    p += IPV6_LEN;
    p += 2; memcpy(p, &plen, 2); p += 2;
    p += 3; *p = nxt; p += 1;
    advert_h->nd_ra_cksum = in_cksum((c_uint16_t *)pkbuf->payload, pkbuf->len);

    ip6_h->ip6_flow = htonl(0x60000001);
    ip6_h->ip6_plen = plen;
    ip6_h->ip6_nxt = nxt;  /* ICMPv6 */
    ip6_h->ip6_hlim = 0xff;
    memcpy(ip6_h->ip6_src.s6_addr, src_ipsub.sub, sizeof src_ipsub.sub);
    memcpy(ip6_h->ip6_dst.s6_addr, ip6_dst, IPV6_LEN);
    
    //rv = upf_gtp_send_to_bearer(bearer, pkbuf);
    d_assert(rv == CORE_OK,, "upf_gtp_send_to_bearer() faild");

    d_trace(5, "[UPF]      Router Advertisement\n");

    pkbuf_free(pkbuf);
    return rv;
}
#endif

c_uint16_t in_cksum(c_uint16_t *addr, int len)
{
    int nleft = len;
    c_uint32_t sum = 0;
    c_uint16_t *w = addr;
    c_uint16_t answer = 0;

    // Adding 16 bits sequentially in sum
    while (nleft > 1)
    {
        sum += *w;
        nleft -= 2;
        w++;
    }

    // If an odd byte is left
    if (nleft == 1)
    {
        *(c_uint8_t *)(&answer) = *(c_uint8_t *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}
