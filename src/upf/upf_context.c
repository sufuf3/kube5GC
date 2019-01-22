#define TRACE_MODULE _upf_context
#include "core_debug.h"
#include "core_pool.h"
#include "core_index.h"
#include "core_lib.h"
#include "core_network.h"

#include <mongoc.h>
#include <yaml.h>
#include "app/yaml_helper.h"

#include "gtp/gtp_types.h"
#include "gtp/gtp_conv.h"
#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"
#include "gtp/gtp_xact.h"

//#include "fd/fd_lib.h"

#include "app/context.h"
#include "upf_context.h"

#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_conv.h"
#include "pfcp/pfcp_node.h"
#include "pfcp/pfcp_path.h"
#include "pfcp/pfcp_xact.h"

upf_pdr_t *upf_pdr_add(upf_sess_t *sess);
upf_far_t *upf_far_add(void);

static upf_context_t self;
//static fd_config_t g_fd_conf;

pool_declare(upf_dev_pool, upf_dev_t, MAX_NUM_OF_DEV);
pool_declare(upf_subnet_pool, upf_subnet_t, MAX_NUM_OF_SUBNET);

index_declare(upf_sess_pool, upf_sess_t, MAX_POOL_OF_SESS);
#define MAX_POOL_OF_PDR (MAX_POOL_OF_BEARER * 2)
#define MAX_POOL_OF_FAR (MAX_POOL_OF_SESS * 2)
#define MAX_POOL_OF_URR (MAX_POOL_OF_UE)
#define MAX_POOL_OF_QER (MAX_POOL_OF_SESS * 2)
#define MAX_POOL_OF_BAR (MAX_POOL_OF_UE)

index_declare(upf_pdr_pool, upf_pdr_t, MAX_POOL_OF_PDR);
index_declare(upf_far_pool, upf_far_t, MAX_POOL_OF_FAR);
index_declare(upf_urr_pool, upf_urr_t, MAX_POOL_OF_URR);
index_declare(upf_qer_pool, upf_qer_t, MAX_POOL_OF_QER);
index_declare(upf_bar_pool, upf_bar_t, MAX_POOL_OF_BAR);
pool_declare(upf_pf_pool, upf_pf_t, MAX_POOL_OF_PF);

static int context_initiaized = 0;

status_t upf_context_init()
{
    d_assert(context_initiaized == 0, return CORE_ERROR,
             "UPF context already has been initialized");

    /* Initialize UPF context */
    memset(&self, 0, sizeof(upf_context_t));

    list_init(&self.gtpu_list);
    list_init(&self.gtpu_list6);
    list_init(&self.pfcp_list);
    list_init(&self.pfcp_list6);
    {
        //struct  timeval  tv;
        //gettimeofday(&tv,NULL);
        //self.recovery_time = htonl(tv.tv_sec);
        self.recovery_time = htonl(time((time_t *)NULL));
    }

    gtp_node_init();
    list_init(&self.enb_s1u_list);
    pfcp_node_init();
    list_init(&self.upf_n4_list);

    list_init(&self.dev_list);
    pool_init(&upf_dev_pool, MAX_NUM_OF_DEV);
    list_init(&self.subnet_list);
    pool_init(&upf_subnet_pool, MAX_NUM_OF_SUBNET);

    index_init(&upf_sess_pool, MAX_POOL_OF_SESS);
    index_init(&upf_pdr_pool, MAX_POOL_OF_PDR);
    index_init(&upf_far_pool, MAX_POOL_OF_FAR);
    index_init(&upf_urr_pool, MAX_POOL_OF_URR);
    index_init(&upf_qer_pool, MAX_POOL_OF_QER);
    index_init(&upf_bar_pool, MAX_POOL_OF_BAR);
    pool_init(&upf_pf_pool, MAX_POOL_OF_PF);

    self.sess_hash = hash_make();

    context_initiaized = 1;

    return CORE_OK;
}

status_t upf_context_final()
{
    d_assert(context_initiaized == 1, return CORE_ERROR,
             "UPF context already has been finalized");

    upf_sess_remove_all();

    d_assert(self.sess_hash, , "Null param");
    hash_destroy(self.sess_hash);

    if (index_used(&upf_sess_pool))
        d_error("%d not freed in upf_sess_pool[%d] in UPF-Context",
                index_used(&upf_sess_pool), index_size(&upf_sess_pool));
    d_trace(9, "%d not freed in upf_sess_pool[%d] in UPF-Context\n",
            index_used(&upf_sess_pool), index_size(&upf_sess_pool));

    upf_dev_remove_all();
    upf_subnet_remove_all();

    index_final(&upf_sess_pool);
    pool_final(&upf_pf_pool);
    pool_final(&upf_pdr_pool);
    pool_final(&upf_far_pool);
    pool_final(&upf_qer_pool);
    pool_final(&upf_urr_pool);
    pool_final(&upf_bar_pool);
    pfcp_remove_all_nodes(&self.upf_n4_list);
    sock_remove_all_nodes(&self.pfcp_list);
    sock_remove_all_nodes(&self.pfcp_list6);
    pfcp_node_final();
    pool_final(&upf_dev_pool);
    pool_final(&upf_subnet_pool);

    gtp_remove_all_nodes(&self.enb_s1u_list);
    gtp_node_final();

    sock_remove_all_nodes(&self.gtpu_list);
    sock_remove_all_nodes(&self.gtpu_list6);

    context_initiaized = 0;

    return CORE_OK;
}

upf_context_t *upf_self()
{
    return &self;
}

static status_t upf_context_prepare()
{
    self.gtpu_port = GTPV1_U_UDP_PORT;
    self.pfcp_port = PFCP_UDP_PORT;
    self.tun_ifname = "uptun";

    return CORE_OK;
}

static status_t upf_context_validation()
{
    if (list_first(&self.gtpu_list) == NULL &&
        list_first(&self.gtpu_list6) == NULL)
    {
        d_error("No upf.gtpu in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    if (self.dns[0] == NULL && self.dns6[0] == NULL)
    {
        d_error("No upf.dns in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t upf_context_parse_config()
{
    status_t rv;
    config_t *config = &context_self()->config;
    yaml_document_t *document = NULL;
    yaml_iter_t root_iter;

    d_assert(config, return CORE_ERROR, );
    document = config->document;
    d_assert(document, return CORE_ERROR, );

    rv = upf_context_prepare();
    if (rv != CORE_OK)
        return rv;

    yaml_iter_init(&root_iter, document);
    while (yaml_iter_next(&root_iter))
    {
        const char *root_key = yaml_iter_key(&root_iter);
        d_assert(root_key, return CORE_ERROR, );
        if (!strcmp(root_key, "upf"))
        {
            yaml_iter_t upf_iter;
            yaml_iter_recurse(&root_iter, &upf_iter);
            while (yaml_iter_next(&upf_iter))
            {
                const char *upf_key = yaml_iter_key(&upf_iter);
                d_assert(upf_key, return CORE_ERROR, );
                if (!strcmp(upf_key, "gtpu"))
                {
                    yaml_iter_t gtpu_array, gtpu_iter;
                    yaml_iter_recurse(&upf_iter, &gtpu_array);
                    do
                    {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.gtpu_port;
                        const char *dev = NULL;
                        c_sockaddr_t *list = NULL;
                        sock_node_t *node = NULL;

                        if (yaml_iter_type(&gtpu_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&gtpu_iter, &gtpu_array,
                                   sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&gtpu_array) ==
                                 YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&gtpu_array))
                                break;
                            yaml_iter_recurse(&gtpu_array, &gtpu_iter);
                        }
                        else if (yaml_iter_type(&gtpu_array) ==
                                 YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR, );

                        while (yaml_iter_next(&gtpu_iter))
                        {
                            const char *gtpu_key =
                                yaml_iter_key(&gtpu_iter);
                            d_assert(gtpu_key,
                                     return CORE_ERROR, );
                            if (!strcmp(gtpu_key, "family"))
                            {
                                const char *v = yaml_iter_value(&gtpu_iter);
                                if (v)
                                    family = atoi(v);
                                if (family != AF_UNSPEC &&
                                    family != AF_INET && family != AF_INET6)
                                {
                                    d_warn("Ignore family(%d) : AF_UNSPEC(%d), "
                                           "AF_INET(%d), AF_INET6(%d) ",
                                           family, AF_UNSPEC, AF_INET, AF_INET6);
                                    family = AF_UNSPEC;
                                }
                            }
                            else if (!strcmp(gtpu_key, "addr") ||
                                     !strcmp(gtpu_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&gtpu_iter, &hostname_iter);
                                d_assert(yaml_iter_type(&hostname_iter) !=
                                             YAML_MAPPING_NODE,
                                         return CORE_ERROR, );

                                do
                                {
                                    if (yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    d_assert(num <= MAX_NUM_OF_HOSTNAME,
                                             return CORE_ERROR, );
                                    hostname[num++] =
                                        yaml_iter_value(&hostname_iter);
                                } while (
                                    yaml_iter_type(&hostname_iter) ==
                                    YAML_SEQUENCE_NODE);
                            }
                            else if (!strcmp(gtpu_key, "port"))
                            {
                                const char *v = yaml_iter_value(&gtpu_iter);
                                if (v)
                                {
                                    port = atoi(v);
                                    self.gtpu_port = port;
                                }
                            }
                            else if (!strcmp(gtpu_key, "dev"))
                            {
                                dev = yaml_iter_value(&gtpu_iter);
                            }
                            else
                                d_warn("unknown key `%s`", gtpu_key);
                        }

                        list = NULL;
                        for (i = 0; i < num; i++)
                        {
                            rv = core_addaddrinfo(&list,
                                                  family, hostname[i], port, 0);
                            d_assert(rv == CORE_OK, return CORE_ERROR, );
                        }

                        if (list)
                        {
                            if (context_self()->parameter.no_ipv4 == 0)
                            {
                                rv = sock_add_node(&self.gtpu_list,
                                                   &node, list, AF_INET);
                                d_assert(rv == CORE_OK, return CORE_ERROR, );
                            }

                            if (context_self()->parameter.no_ipv6 == 0)
                            {
                                rv = sock_add_node(&self.gtpu_list6,
                                                   &node, list, AF_INET6);
                                d_assert(rv == CORE_OK, return CORE_ERROR, );
                            }

                            core_freeaddrinfo(list);
                        }

                        if (dev)
                        {
                            rv = sock_probe_node(
                                context_self()->parameter.no_ipv4 ? NULL : &self.gtpu_list,
                                context_self()->parameter.no_ipv6 ? NULL : &self.gtpu_list6,
                                dev, self.gtpu_port);
                            d_assert(rv == CORE_OK, return CORE_ERROR, );
                        }

                    } while (yaml_iter_type(&gtpu_array) == YAML_SEQUENCE_NODE);

                    if (list_first(&self.gtpu_list) == NULL &&
                        list_first(&self.gtpu_list6) == NULL)
                    {
                        rv = sock_probe_node(
                            context_self()->parameter.no_ipv4 ? NULL : &self.gtpu_list,
                            context_self()->parameter.no_ipv6 ? NULL : &self.gtpu_list6,
                            NULL, self.gtpu_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR, );
                    }
                }
                else if (!strcmp(upf_key, "ue_pool"))
                {
                    yaml_iter_t ue_pool_array, ue_pool_iter;
                    yaml_iter_recurse(&upf_iter, &ue_pool_array);
                    do
                    {
                        upf_subnet_t *subnet = NULL;
                        const char *ipstr = NULL;
                        const char *mask_or_numbits = NULL;
                        const char *apn = NULL;
                        const char *dev = self.tun_ifname;

                        if (yaml_iter_type(&ue_pool_array) ==
                            YAML_MAPPING_NODE)
                        {
                            memcpy(&ue_pool_iter, &ue_pool_array,
                                   sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&ue_pool_array) ==
                                 YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&ue_pool_array))
                                break;
                            yaml_iter_recurse(&ue_pool_array,
                                              &ue_pool_iter);
                        }
                        else if (yaml_iter_type(&ue_pool_array) ==
                                 YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR, );

                        while (yaml_iter_next(&ue_pool_iter))
                        {
                            const char *ue_pool_key =
                                yaml_iter_key(&ue_pool_iter);
                            d_assert(ue_pool_key,
                                     return CORE_ERROR, );
                            if (!strcmp(ue_pool_key, "addr"))
                            {
                                char *v =
                                    (char *)yaml_iter_value(&ue_pool_iter);
                                if (v)
                                {
                                    ipstr = (const char *)strsep(&v, "/");
                                    if (ipstr)
                                    {
                                        mask_or_numbits = (const char *)v;
                                    }
                                }
                            }
                            else if (!strcmp(ue_pool_key, "apn"))
                            {
                                apn = yaml_iter_value(&ue_pool_iter);
                            }
                            else if (!strcmp(ue_pool_key, "dev"))
                            {
                                dev = yaml_iter_value(&ue_pool_iter);
                            }
                            else
                                d_warn("unknown key `%s`", ue_pool_key);
                        }

                        if (ipstr && mask_or_numbits)
                        {
                            subnet = upf_subnet_add(
                                ipstr, mask_or_numbits, apn, dev);
                            d_assert(subnet, return CORE_ERROR, );
                        }
                        else
                        {
                            d_warn("Ignore : addr(%s/%s), apn(%s)",
                                   ipstr, mask_or_numbits, apn);
                        }
                    } while (yaml_iter_type(&ue_pool_array) ==
                             YAML_SEQUENCE_NODE);
                }
                else if (!strcmp(upf_key, "dns"))
                {
                    yaml_iter_t dns_iter;
                    yaml_iter_recurse(&upf_iter, &dns_iter);
                    d_assert(yaml_iter_type(&dns_iter) !=
                                 YAML_MAPPING_NODE,
                             return CORE_ERROR, );

                    do
                    {
                        const char *v = NULL;

                        if (yaml_iter_type(&dns_iter) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&dns_iter))
                                break;
                        }

                        v = yaml_iter_value(&dns_iter);
                        if (v)
                        {
                            ipsubnet_t ipsub;
                            rv = core_ipsubnet(&ipsub, v, NULL);
                            d_assert(rv == CORE_OK, return CORE_ERROR, );

                            if (ipsub.family == AF_INET)
                            {
                                if (self.dns[0] && self.dns[1])
                                    d_warn("Ignore DNS : %s", v);
                                else if (self.dns[0])
                                    self.dns[1] = v;
                                else
                                    self.dns[0] = v;
                            }
                            else if (ipsub.family == AF_INET6)
                            {
                                if (self.dns6[0] && self.dns6[1])
                                    d_warn("Ignore DNS : %s", v);
                                else if (self.dns6[0])
                                    self.dns6[1] = v;
                                else
                                    self.dns6[0] = v;
                            }
                            else
                                d_warn("Ignore DNS : %s", v);
                        }

                    } while (
                        yaml_iter_type(&dns_iter) ==
                        YAML_SEQUENCE_NODE);
                }
                else if (!strcmp(upf_key, "p-cscf"))
                {
                    yaml_iter_t dns_iter;
                    yaml_iter_recurse(&upf_iter, &dns_iter);
                    d_assert(yaml_iter_type(&dns_iter) !=
                                 YAML_MAPPING_NODE,
                             return CORE_ERROR, );

                    self.num_of_p_cscf = 0;
                    self.num_of_p_cscf6 = 0;
                    do
                    {
                        const char *v = NULL;

                        if (yaml_iter_type(&dns_iter) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&dns_iter))
                                break;
                        }

                        v = yaml_iter_value(&dns_iter);
                        if (v)
                        {
                            ipsubnet_t ipsub;
                            rv = core_ipsubnet(&ipsub, v, NULL);
                            d_assert(rv == CORE_OK, return CORE_ERROR, );

                            if (ipsub.family == AF_INET)
                            {
                                if (self.num_of_p_cscf >= MAX_NUM_OF_P_CSCF)
                                    d_warn("Ignore P-CSCF : %s", v);
                                else
                                    self.p_cscf[self.num_of_p_cscf++] = v;
                            }
                            else if (ipsub.family == AF_INET6)
                            {
                                if (self.num_of_p_cscf6 >= MAX_NUM_OF_P_CSCF)
                                    d_warn("Ignore P-CSCF : %s", v);
                                else
                                    self.p_cscf6[self.num_of_p_cscf6++] = v;
                            }
                            else
                                d_warn("Ignore P-CSCF : %s", v);
                        }

                    } while (
                        yaml_iter_type(&dns_iter) ==
                        YAML_SEQUENCE_NODE);
                }
                else if (!strcmp(upf_key, "pfcp"))
                {
                    yaml_iter_t pfcp_array, pfcp_iter;
                    yaml_iter_recurse(&upf_iter, &pfcp_array);
                    do
                    {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.pfcp_port;
                        //const char *dev = NULL;
                        c_sockaddr_t *list = NULL;
                        sock_node_t *node = NULL;

                        if (yaml_iter_type(&pfcp_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&pfcp_iter, &pfcp_array,
                                   sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&pfcp_array) ==
                                 YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&pfcp_array))
                                break;
                            yaml_iter_recurse(&pfcp_array, &pfcp_iter);
                        }
                        else if (yaml_iter_type(&pfcp_array) ==
                                 YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR, );

                        while (yaml_iter_next(&pfcp_iter))
                        {
                            const char *pfcp_key =
                                yaml_iter_key(&pfcp_iter);
                            d_assert(pfcp_key,
                                     return CORE_ERROR, );
                            if (!strcmp(pfcp_key, "addr") ||
                                !strcmp(pfcp_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&pfcp_iter, &hostname_iter);
                                d_assert(yaml_iter_type(&hostname_iter) !=
                                             YAML_MAPPING_NODE,
                                         return CORE_ERROR, );

                                do
                                {
                                    if (yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    d_assert(num <= MAX_NUM_OF_HOSTNAME,
                                             return CORE_ERROR, );
                                    hostname[num++] =
                                        yaml_iter_value(&hostname_iter);
                                } while (
                                    yaml_iter_type(&hostname_iter) ==
                                    YAML_SEQUENCE_NODE);
                            }
                            else
                                d_warn("unknown key `%s`", pfcp_key);
                        }

                        list = NULL;
                        for (i = 0; i < num; i++)
                        {
                            rv = core_addaddrinfo(&list,
                                                  family, hostname[i], port, 0);
                            d_assert(rv == CORE_OK, return CORE_ERROR, );
                        }

                        if (list)
                        {
                            if (context_self()->parameter.no_ipv4 == 0)
                            {
                                rv = sock_add_node(&self.pfcp_list,
                                                   &node, list, AF_INET);
                                d_assert(rv == CORE_OK, return CORE_ERROR, );
                            }

                            if (context_self()->parameter.no_ipv6 == 0)
                            {
                                rv = sock_add_node(&self.pfcp_list6,
                                                   &node, list, AF_INET6);
                                d_assert(rv == CORE_OK, return CORE_ERROR, );
                            }

                            core_freeaddrinfo(list);
                        }

                    } while (yaml_iter_type(&pfcp_array) == YAML_SEQUENCE_NODE);

                    if (list_first(&self.pfcp_list) == NULL &&
                        list_first(&self.pfcp_list6) == NULL)
                    {
                        rv = sock_probe_node(
                            context_self()->parameter.no_ipv4 ? NULL : &self.pfcp_list,
                            context_self()->parameter.no_ipv6 ? NULL : &self.pfcp_list6,
                            NULL, self.pfcp_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR, );
                    }
                }
                else
                    d_warn("unknown key `%s`", upf_key);
            }
        }
    }

    rv = upf_context_validation();
    if (rv != CORE_OK)
        return rv;

    return CORE_OK;
}

status_t upf_context_setup_trace_module()
{
    int app = context_self()->logger.trace.app;
    int gtp = context_self()->logger.trace.gtp;
    int pfcp = context_self()->logger.trace.pfcp;

    if (app)
    {
        extern int _upf_context;
        d_trace_level(&_upf_context, app);
    }

    if (gtp)
    {
        extern int _upf_gtp_path;
        d_trace_level(&_upf_gtp_path, gtp);
        extern int _upf_ipfw;
        d_trace_level(&_upf_ipfw, gtp);
    }

    if (pfcp)
    {
        extern int _upf_pfcp_path;
        d_trace_level(&_upf_pfcp_path, pfcp);
        extern int _upf_n4_build;
        d_trace_level(&_upf_n4_build, pfcp);
        extern int _upf_n4_handler;
        d_trace_level(&_upf_n4_handler, pfcp);
    }

    return CORE_OK;
}

static void *sess_hash_keygen(c_uint8_t *out, int *out_len,
                              c_uint8_t *imsi, int imsi_len, c_int8_t *apn)
{
    memcpy(out, imsi, imsi_len);
    core_cpystrn((char *)(out + imsi_len), apn, MAX_APN_LEN + 1);
    *out_len = imsi_len + strlen((char *)(out + imsi_len));

    return out;
}

upf_sess_t *upf_sess_add(
    pfcp_ue_ip_addr_t *ue_ip, c_int8_t *apn,
    c_uint8_t pdn_type)
{
    upf_sess_t *sess = NULL;
    //upf_bearer_t *bearer = NULL;
    upf_subnet_t *subnet6 = NULL;

    index_alloc(&upf_sess_pool, &sess);
    d_assert(sess, return NULL, "Null param");

    sess->gnode = NULL;

    /* UPF-SEID is derived from IP address + INDEX */
    if (self.pfcp_addr)
    {
        sess->upf_seid = ((c_uint64_t)self.pfcp_addr->sin.sin_addr.s_addr << 32) | sess->index;
    }
    else if (self.pfcp_addr6)
    {
        c_uint32_t *ptr = (c_uint32_t *)&self.pfcp_addr6->sin6.sin6_addr.s6_addr[0];
        sess->upf_seid = (((c_uint64_t)(*ptr)) << 32) | sess->index;
        //sess->upf_seid = ((c_uint64_t)(*((c_uint32_t*)self.pfcp_addr6->sin6.sin6_addr.s6_addr))<<32) | sess->index;
    }
    sess->upf_seid = htobe64(sess->upf_seid);
    sess->upf_seid = 0; /* initialized to 0 */

    /* Set IMSI */
    //sess->imsi_len = imsi_len;
    //memcpy(sess->imsi, imsi, sess->imsi_len);
    //core_buffer_to_bcd(sess->imsi, sess->imsi_len, sess->imsi_bcd);

    /* Set APN */
    core_cpystrn(sess->pdn.apn, apn, MAX_APN_LEN + 1);

    list_init(&sess->dl_pdr_list);
    list_init(&sess->ul_pdr_list);

    //bearer = upf_bearer_add(sess);
    //d_assert(bearer, upf_sess_remove(sess); return NULL,
    //        "Can't add default bearer context");
    //bearer->ebi = ebi;

    sess->pdn.paa.pdn_type = pdn_type;
    if (pdn_type == PFCP_PDN_TYPE_IPV4)
    {
        sess->ipv4 = upf_ue_ip_alloc(AF_INET, apn);
        d_assert(sess->ipv4, upf_sess_remove(sess); return NULL,
                                                           "Can't allocate IPv4 Pool");
        sess->pdn.paa.addr = sess->ipv4->addr[0];
    }
    else if (pdn_type == PFCP_PDN_TYPE_IPV6)
    {
        sess->ipv6 = upf_ue_ip_alloc(AF_INET6, apn);
        d_assert(sess->ipv6, upf_sess_remove(sess); return NULL,
                                                           "Can't allocate IPv6 Pool");

        subnet6 = sess->ipv6->subnet;
        d_assert(subnet6, upf_sess_remove(sess); return NULL,
                                                        "No IPv6 subnet");

        sess->pdn.paa.len = subnet6->prefixlen;
        memcpy(sess->pdn.paa.addr6, sess->ipv6->addr, IPV6_LEN);
    }
    else if (pdn_type == PFCP_PDN_TYPE_IPV4V6)
    {
        sess->ipv4 = upf_ue_ip_alloc(AF_INET, apn);
        d_assert(sess->ipv4, upf_sess_remove(sess); return NULL,
                                                           "Can't allocate IPv4 Pool");
        sess->ipv6 = upf_ue_ip_alloc(AF_INET6, apn);
        d_assert(sess->ipv6, upf_sess_remove(sess); return NULL,
                                                           "Can't allocate IPv6 Pool");

        subnet6 = sess->ipv6->subnet;
        d_assert(subnet6, upf_sess_remove(sess); return NULL,
                                                        "No IPv6 subnet");

        sess->pdn.paa.both.addr = sess->ipv4->addr[0];
        sess->pdn.paa.both.len = subnet6->prefixlen;
        memcpy(sess->pdn.paa.both.addr6, sess->ipv6->addr, IPV6_LEN);
    }
    else
        d_assert(0, return NULL, "Unsupported PDN Type(%d)", pdn_type);

    /* Generate Hash Key : IP address(not IMSI) + APN */
    if (pdn_type == PFCP_PDN_TYPE_IPV4)
    {
        sess_hash_keygen(sess->hash_keybuf, &sess->hash_keylen,
                         (c_uint8_t *)sess->ipv4->addr, 4, apn);
    }
    else
    {
        sess_hash_keygen(sess->hash_keybuf, &sess->hash_keylen,
                         (c_uint8_t *)sess->ipv6->addr, IPV6_LEN, apn);
    }

    hash_set(self.sess_hash, sess->hash_keybuf, sess->hash_keylen, sess);

    return sess;
}

status_t upf_sess_remove(upf_sess_t *sess)
{
    upf_pdr_t *pdr;

    d_assert(self.sess_hash, return CORE_ERROR, "Null param");
    d_assert(sess, return CORE_ERROR, "Null param");

    hash_set(self.sess_hash, sess->hash_keybuf, sess->hash_keylen, NULL);

    if (sess->ipv4)
        upf_ue_ip_free(sess->ipv4);
    if (sess->ipv6)
        upf_ue_ip_free(sess->ipv6);

    //upf_bearer_remove_all(sess);
    pdr = list_first(&sess->dl_pdr_list);
    while (pdr)
    {
        upf_pdr_remove(pdr);
        pdr = list_first(&sess->dl_pdr_list);
    }

    pdr = list_first(&sess->ul_pdr_list);
    while (pdr)
    {
        upf_pdr_remove(pdr);
        pdr = list_first(&sess->ul_pdr_list);
    }

    index_free(&upf_sess_pool, sess);

    return CORE_OK;
}

status_t upf_sess_remove_all()
{
    hash_index_t *hi = NULL;
    upf_sess_t *sess = NULL;

    for (hi = upf_sess_first(); hi; hi = upf_sess_next(hi))
    {
        sess = upf_sess_this(hi);
        upf_sess_remove(sess);
    }

    return CORE_OK;
}

upf_sess_t *upf_sess_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&upf_sess_pool, index);
}

upf_sess_t *upf_sess_find_by_teid(c_uint32_t teid)
{
    return upf_sess_find(teid);
}

upf_sess_t *upf_sess_find_by_seid(c_uint64_t seid)
{
    return upf_sess_find(seid & 0xFFFFFFFF);
}

upf_sess_t *upf_sess_find_by_imsi_apn(
    c_uint8_t *imsi, int imsi_len, c_int8_t *apn)
{
    c_uint8_t keybuf[MAX_IMSI_LEN + MAX_APN_LEN + 1];
    int keylen = 0;

    d_assert(self.sess_hash, return NULL, "Null param");

    sess_hash_keygen(keybuf, &keylen, imsi, imsi_len, apn);
    return (upf_sess_t *)hash_get(self.sess_hash, keybuf, keylen);
}

upf_sess_t *upf_sess_add_by_message(pfcp_message_t *message)
{
    upf_sess_t *sess = NULL;
    //c_int8_t apn[MAX_APN_LEN];

    pfcp_session_establishment_request_t *req = &message->pfcp_session_establishment_request;

    if (req->node_id.presence == 0)
    {
        d_error("No Node ID");
        return NULL;
    }
    if (req->cp_f_seid.presence == 0)
    {
        d_error("No F_SEID");
        return NULL;
    }
    if (req->create_pdr.presence == 0)
    {
        d_error("No PDR");
        return NULL;
    }
    if (req->create_far.presence == 0)
    {
        d_error("No FAR");
        return NULL;
    }
    if (req->pdn_type.presence == 0)
    {
        d_error("No PDN Type");
        return NULL;
    }
    if (req->create_pdr.pdi.presence == 0)
    {
        d_error("No PDI in create_pdr");
        return NULL;
    }
    if (req->create_pdr.pdi.ue_ip_address.presence == 0)
    {
        d_error("No ue ip address in create_far");
        return NULL;
    }
    if (req->create_pdr.pdi.network_instance.presence == 0)
    {
        d_error("No network instance (APN) in create_far");
        return NULL;
    }

    // apn_parse(apn, req->create_pdr.pdi.network_instance.data, req->create_pdr.pdi.network_instance.len);

    // d_trace(9, "upf_sess_add_by_message() [APN:%s, PDN:%d]\n",
    //         apn, ((c_int8_t*)req->pdn_type.data)[0]);

    // sess = upf_sess_find_by_imsi_apn(req->imsi.data, req->imsi.len, apn);
    // if (!sess)
    {
        sess = upf_sess_add((pfcp_ue_ip_addr_t *)req->create_pdr.pdi.ue_ip_address.data,
                            req->create_pdr.pdi.network_instance.data,
                            ((c_int8_t *)req->pdn_type.data)[0]);
        d_assert(sess, return NULL, "No Session Context");
    }

    /* CP F-SEID */
    sess->upf_seid = sess->index;
    d_trace(-1, "upf_establishment_upf_seid : %lu", sess->upf_seid);
    return sess;
}

hash_index_t *upf_sess_first()
{
    d_assert(self.sess_hash, return NULL, "Null param");
    return hash_first(self.sess_hash);
}

hash_index_t *upf_sess_next(hash_index_t *hi)
{
    return hash_next(hi);
}

upf_sess_t *upf_sess_this(hash_index_t *hi)
{
    d_assert(hi, return NULL, "Null param");
    return hash_this_val(hi);
}

#if 0
upf_pf_t *upf_pf_add(upf_bearer_t *bearer, c_uint32_t precedence)
{
    upf_pf_t *pf = NULL;

    d_assert(bearer, return NULL, "Null param");

    pool_alloc_node(&upf_pf_pool, &pf);
    d_assert(pf, return NULL, "Null param");

    pf->identifier = NEXT_ID(bearer->pf_identifier, 1, 15);
    pf->bearer = bearer;

    list_append(&bearer->pf_list, pf);

    return pf;
}

status_t upf_pf_remove(upf_pf_t *pf)
{
    d_assert(pf, return CORE_ERROR, "Null param");
    d_assert(pf->bearer, return CORE_ERROR, "Null param");

    list_remove(&pf->bearer->pf_list, pf);
    pool_free_node(&upf_pf_pool, pf);

    return CORE_OK;
}

status_t upf_pf_remove_all(upf_bearer_t *bearer)
{
    upf_pf_t *pf = NULL, *next_pf = NULL;

    d_assert(bearer, return CORE_ERROR, "Null param");
    
    pf = upf_pf_first(bearer);
    while (pf)
    {
        next_pf = upf_pf_next(pf);

        upf_pf_remove(pf);

        pf = next_pf;
    }

    return CORE_OK;
}

upf_pf_t* upf_pf_find_by_id(upf_bearer_t *bearer, c_uint8_t id)
{
    upf_pf_t *pf = NULL;
    
    pf = upf_pf_first(bearer);
    while (pf)
    {
        if (pf->identifier == id)
            return pf;

        pf = upf_pf_next(pf);
    }

    return CORE_OK;
}

upf_pf_t* upf_pf_first(upf_bearer_t *bearer)
{
    return list_first(&bearer->pf_list);
}

upf_pf_t* upf_pf_next(upf_pf_t *pf)
{
    return list_next(pf);
}
#endif

status_t upf_ue_pool_generate()
{
    int j;
    upf_subnet_t *subnet = NULL;

    for (subnet = upf_subnet_first(); subnet; subnet = upf_subnet_next(subnet))
    {
        int index = 0;
        c_uint32_t mask_count;
        c_uint32_t broadcast[4];

        if (subnet->family == AF_INET)
        {
            if (subnet->prefixlen == 32)
                mask_count = 1;
            else if (subnet->prefixlen < 32)
                mask_count = (0xffffffff >> subnet->prefixlen) + 1;
            else
                d_assert(0, return CORE_ERROR, );
        }
        else if (subnet->family == AF_INET6)
        {
            if (subnet->prefixlen == 128)
                mask_count = 1;
            else if (subnet->prefixlen > 96 && subnet->prefixlen < 128)
                mask_count = (0xffffffff >> (subnet->prefixlen - 96)) + 1;
            else if (subnet->prefixlen <= 96)
                mask_count = 0xffffffff;
            else
                d_assert(0, return CORE_ERROR, );
        }
        else
            d_assert(0, return CORE_ERROR, );

        for (j = 0; j < 4; j++)
        {
            broadcast[j] = subnet->sub.sub[j] + ~subnet->sub.mask[j];
        }

        for (j = 0; j < mask_count && index < MAX_POOL_OF_SESS; j++)
        {
            upf_ue_ip_t *ue_ip = NULL;
            int maxbytes = 0;
            int lastindex = 0;

            ue_ip = &subnet->pool.pool[index];
            d_assert(ue_ip, return CORE_ERROR, );
            memset(ue_ip, 0, sizeof *ue_ip);

            if (subnet->family == AF_INET)
            {
                maxbytes = 4;
                lastindex = 0;
            }
            else if (subnet->family == AF_INET6)
            {
                maxbytes = 16;
                lastindex = 3;
            }

            memcpy(ue_ip->addr, subnet->sub.sub, maxbytes);
            ue_ip->addr[lastindex] += htonl(j);
            ue_ip->subnet = subnet;

            /* Exclude Network Address */
            if (memcmp(ue_ip->addr, subnet->sub.sub, maxbytes) == 0)
                continue;

            /* Exclude Broadcast Address */
            if (memcmp(ue_ip->addr, broadcast, maxbytes) == 0)
                continue;

            /* Exclude TUN IP Address */
            if (memcmp(ue_ip->addr, subnet->gw.sub, maxbytes) == 0)
                continue;

            index++;
        }
        subnet->pool.size = subnet->pool.avail = index;
    }

    return CORE_OK;
}

static upf_subnet_t *find_subnet(int family, const char *apn)
{
    upf_subnet_t *subnet = NULL;

    d_assert(apn, return NULL, );
    d_assert(family == AF_INET || family == AF_INET6, return NULL, );

    for (subnet = upf_subnet_first(); subnet; subnet = upf_subnet_next(subnet))
    {
        if (strlen(subnet->apn))
        {
            if (subnet->family == family && strcmp(subnet->apn, apn) == 0 &&
                pool_avail(&subnet->pool))
            {
                return subnet;
            }
        }
    }

    for (subnet = upf_subnet_first(); subnet; subnet = upf_subnet_next(subnet))
    {
        if (strlen(subnet->apn) == 0)
        {
            if (subnet->family == family && pool_avail(&subnet->pool))
            {
                return subnet;
            }
        }
    }

    if (subnet == NULL)
        d_error("CHECK CONFIGURATION: Cannot find UE Pool");

    return subnet;
}

upf_ue_ip_t *upf_ue_ip_alloc(int family, const char *apn)
{
    upf_subnet_t *subnet = NULL;
    upf_ue_ip_t *ue_ip = NULL;

    d_assert(apn, return NULL, );

    subnet = find_subnet(family, apn);
    d_assert(subnet, return NULL, );

    pool_alloc_node(&subnet->pool, &ue_ip);
    d_assert(ue_ip, return NULL, );

    return ue_ip;
}

status_t upf_ue_ip_free(upf_ue_ip_t *ue_ip)
{
    upf_subnet_t *subnet = NULL;

    d_assert(ue_ip, return CORE_ERROR, );
    subnet = ue_ip->subnet;

    d_assert(subnet, return CORE_ERROR, );
    pool_free_node(&subnet->pool, ue_ip);

    return CORE_OK;
}

upf_dev_t *upf_dev_add(const char *ifname)
{
    upf_dev_t *dev = NULL;

    d_assert(ifname, return NULL, );

    pool_alloc_node(&upf_dev_pool, &dev);
    d_assert(dev, return NULL, );
    memset(dev, 0, sizeof *dev);

    strcpy(dev->ifname, ifname);

    list_append(&self.dev_list, dev);

    return dev;
}

status_t upf_dev_remove(upf_dev_t *dev)
{
    d_assert(dev, return CORE_ERROR, "Null param");

    list_remove(&self.dev_list, dev);

    if (dev->link_local_addr)
        core_freeaddrinfo(dev->link_local_addr);

    pool_free_node(&upf_dev_pool, dev);

    return CORE_OK;
}

status_t upf_dev_remove_all()
{
    upf_dev_t *dev = NULL, *next_dev = NULL;

    dev = upf_dev_first();
    while (dev)
    {
        next_dev = upf_dev_next(dev);

        upf_dev_remove(dev);

        dev = next_dev;
    }

    return CORE_OK;
}

upf_dev_t *upf_dev_find_by_ifname(const char *ifname)
{
    upf_dev_t *dev = NULL;

    d_assert(ifname, return NULL, );

    dev = upf_dev_first();
    while (dev)
    {
        if (strcmp(dev->ifname, ifname) == 0)
            return dev;

        dev = upf_dev_next(dev);
    }

    return CORE_OK;
}

upf_dev_t *upf_dev_first()
{
    return list_first(&self.dev_list);
}

upf_dev_t *upf_dev_next(upf_dev_t *dev)
{
    return list_next(dev);
}

upf_subnet_t *upf_subnet_add(
    const char *ipstr, const char *mask_or_numbits,
    const char *apn, const char *ifname)
{
    status_t rv;
    upf_dev_t *dev = NULL;
    upf_subnet_t *subnet = NULL;

    d_assert(ipstr, return NULL, );
    d_assert(mask_or_numbits, return NULL, );
    d_assert(ifname, return NULL, );

    dev = upf_dev_find_by_ifname(ifname);
    if (!dev)
        dev = upf_dev_add(ifname);
    d_assert(dev, return NULL, );

    pool_alloc_node(&upf_subnet_pool, &subnet);
    d_assert(subnet, return NULL, );
    memset(subnet, 0, sizeof *subnet);

    subnet->dev = dev;

    rv = core_ipsubnet(&subnet->gw, ipstr, NULL);
    d_assert(rv == CORE_OK, return NULL, );

    rv = core_ipsubnet(&subnet->sub, ipstr, mask_or_numbits);
    d_assert(rv == CORE_OK, return NULL, );

    if (apn)
        strcpy(subnet->apn, apn);

    subnet->family = subnet->gw.family;
    subnet->prefixlen = atoi(mask_or_numbits);

    pool_init(&subnet->pool, MAX_POOL_OF_SESS);

    list_append(&self.subnet_list, subnet);

    return subnet;
}

status_t upf_subnet_remove(upf_subnet_t *subnet)
{
    d_assert(subnet, return CORE_ERROR, "Null param");

    list_remove(&self.subnet_list, subnet);

    if (pool_used(&subnet->pool))
    {
        d_warn("%d not freed in ue_ip_pool[%d] in UPF-Context",
               pool_used(&subnet->pool), pool_size(&subnet->pool));
    }
    d_trace(9, "%d not freed in ue_ip_pool[%d] in UPF-Context\n",
            pool_used(&subnet->pool), pool_size(&subnet->pool));
    pool_final(&subnet->pool);

    pool_free_node(&upf_subnet_pool, subnet);

    return CORE_OK;
}

status_t upf_subnet_remove_all()
{
    upf_subnet_t *subnet = NULL, *next_subnet = NULL;

    subnet = upf_subnet_first();
    while (subnet)
    {
        next_subnet = upf_subnet_next(subnet);

        upf_subnet_remove(subnet);

        subnet = next_subnet;
    }

    return CORE_OK;
}

upf_subnet_t *upf_subnet_first()
{
    return list_first(&self.subnet_list);
}

upf_subnet_t *upf_subnet_next(upf_subnet_t *subnet)
{
    return list_next(subnet);
}

upf_pdr_t *upf_pdr_add(upf_sess_t *sess)
{
    upf_pdr_t *pdr = NULL;

    d_assert(sess, return NULL, "Null param");

    index_alloc(&upf_pdr_pool, &pdr);
    d_assert(pdr, return NULL, "PDR context allocation failed");

    // bearer->name = NULL;
    // list_init(&bearer->pf_list);

    pdr->pdr_id = 0; // 0 means not add into list
    pdr->out_hdr_rmv = 0;

    // pdr->bearer = bearer;
    pdr->pnode = NULL;
    pdr->far = NULL;
    pdr->qer = NULL;
    pdr->urr = NULL;
    pdr->sess = sess;
    // list_append(&sess->pdr_list, pdr);

    return pdr;
}

status_t upf_pdr_remove(upf_pdr_t *pdr)
{
    d_assert(pdr, return CORE_ERROR, "Null param");
    d_assert(pdr->sess, return CORE_ERROR, "Null param");

    // 0 means not add into list
    if (pdr->pdr_id)
    {
        if (pdr->source_interface == PFCP_SRC_INTF_ACCESS)
        {
            list_remove(&pdr->sess->ul_pdr_list, pdr);
        }
        else
        {
            list_remove(&pdr->sess->dl_pdr_list, pdr);
        }
    }

    if (pdr->far)
    {
        upf_far_remove(pdr->far);
    }

    index_free(&upf_pdr_pool, pdr);

    return CORE_OK;
}

upf_pdr_t *upf_default_pdr_in_sess(upf_sess_t *sess)
{
    return upf_dl_pdr_first(sess);
}

upf_pdr_t *upf_dl_pdr_first(upf_sess_t *sess)
{
    d_assert(sess, return NULL, "Null param");
    return list_first(&sess->dl_pdr_list);
}

upf_pdr_t *upf_pdr_next(upf_pdr_t *pdr)
{
    return list_next(pdr);
}

upf_pdr_t *upf_pdr_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&upf_pdr_pool, index);
}

upf_pdr_t *upf_pdr_find_by_pdr_id(tlv_packet_detection_rule_id_t *pdr_id)
{
    if (pdr_id->presence)
    {
        int i;
        for (i = 0; i < index_size(&upf_pdr_pool); i++)
        {
            upf_pdr_t *pdr = index_find(&upf_pdr_pool, i);
            if (pdr->pdr_id == *((c_uint16_t *)pdr_id->data))
                return pdr;
        }
    }
    return NULL;
}

upf_pdr_t *upf_pdr_find_by_upf_s1u_teid(c_uint32_t teid)
{
    hash_index_t *hi = NULL;

    for (hi = upf_sess_first(); hi; hi = upf_sess_next(hi))
    {
        upf_sess_t *sess = upf_sess_this(hi);
        d_assert(sess, return NULL, );

        upf_pdr_t *pdr = NULL;
        upf_pdr_t *default_pdr = NULL;

        /* Save the default pdr */
        default_pdr = list_first(&sess->ul_pdr_list);
        d_assert(default_pdr, return NULL, "No default PDR");

        /* Found */
        pdr = default_pdr;
        /* Find the bearer with matched */
        for (; pdr; pdr = upf_pdr_next(pdr))
        {
            if (pdr->source_interface != PFCP_SRC_INTF_ACCESS)
            {
                continue;
            }

            if (pdr->upf_s1u_teid == teid)
            {
                return pdr;
            }
        }
    }
    return NULL;
}

upf_far_t *upf_far_add(void)
{
    upf_far_t *far = NULL;
    // d_assert(bearer, return NULL, "Null param");

    index_alloc(&upf_far_pool, &far);
    d_assert(far, return NULL, "FAR context allocation failed");

    // bearer->name = NULL;
    // list_init(&bearer->pf_list);
    far->far_id = far->index;

    // far->bearer = bearer;
    far->pnode = NULL;
    far->gnode = NULL;

    // list_append(&bearer->sess->far_list, far);

    return far;
}

status_t upf_far_remove(upf_far_t *far)
{
    d_assert(far, return CORE_ERROR, "Null param");
    // d_assert(pdr->bearer, return CORE_ERROR, "Null param");
    // d_assert(pdr->bearer->sess, return CORE_ERROR, "Null param");

    list_remove(&upf_self()->far_list, far);

    // if (bearer->name)
    //     CORE_FREE(bearer->name);

    // upf_pf_remove_all(bearer);

    index_free(&upf_far_pool, far);

    return CORE_OK;
}

// In UPF, pdr_id is assigned by SMF, so cannot find by index
upf_far_t *upf_far_find_by_far_id(c_uint32_t far_id)
{
    upf_far_t *far = NULL;

    // d_assert(far_id, return NULL,);

    far = list_first(&upf_self()->far_list);
    while (far)
    {
        if (far->far_id == far_id)
            break;
        far = list_next(far);
    }

    return far;
}
