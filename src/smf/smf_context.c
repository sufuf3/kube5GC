#define TRACE_MODULE _smf_context

#include "core_debug.h"
#include "core_index.h"
#include "core_lib.h"
#include "core_list.h"
#include "core_pool.h"

#include "3gpp_types.h"
#include "app/context.h"

#include <time.h>
#include <yaml.h>
#include "app/yaml_helper.h"

#include "gtp/gtp_node.h"
#include "pfcp/pfcp_node.h"

#include "smf_context.h"

static smf_context_t self;
static int context_initialized = 0;

index_declare(smf_sess_pool, smf_sess_t, MAX_POOL_OF_SESS);
index_declare(smf_bearer_pool, smf_bearer_t, MAX_POOL_OF_BEARER);

pool_declare(smf_subnet_pool, smf_subnet_t, MAX_NUM_OF_SUBNET);

index_declare(smf_pdr_pool, smf_pdr_t, MAX_POOL_OF_SESS * 2);
index_declare(smf_far_pool, smf_far_t, MAX_POOL_OF_SESS);
index_declare(smf_urr_pool, smf_urr_t, MAX_POOL_OF_SESS);
index_declare(smf_qer_pool, smf_qer_t, MAX_POOL_OF_SESS);

status_t smf_context_init()
{
    d_assert(context_initialized == 0, return CORE_ERROR,
            "SMF context already has been initialized");

    /* Initialize SMF context */
    memset(&self, 0, sizeof(smf_context_t));

    index_init(&smf_sess_pool, MAX_POOL_OF_SESS);
    index_init(&smf_bearer_pool, MAX_POOL_OF_BEARER);
    
    index_init(&smf_pdr_pool, MAX_POOL_OF_SESS * 2);
    index_init(&smf_far_pool, MAX_POOL_OF_SESS);
    index_init(&smf_urr_pool, MAX_POOL_OF_SESS);
    index_init(&smf_qer_pool, MAX_POOL_OF_SESS);
    
    list_init(&self.gtpc_list);
    list_init(&self.gtpc_list6);
    gtp_node_init();
    
    list_init(&self.pfcp_list);
    list_init(&self.pfcp_list6);
    list_init(&self.upf_n4_list);
    pfcp_node_init();
    
    list_init(&self.subnet_list);
    pool_init(&smf_subnet_pool, MAX_NUM_OF_SUBNET);
    
    self.start_time = time((time_t*)NULL);
    self.cp_function_features = 0;
    
    self.sess_hash = hash_make();

    context_initialized = 1;

    return CORE_OK;
}

status_t smf_context_final()
{
    d_assert(context_initialized == 1, return CORE_ERROR,
            "SMF context already has been finalized");

    index_final(&smf_sess_pool);
    index_final(&smf_bearer_pool);

    index_final(&smf_pdr_pool);
    index_final(&smf_far_pool);
    index_final(&smf_urr_pool);
    index_final(&smf_qer_pool);
    
    sock_remove_all_nodes(&self.gtpc_list);
    sock_remove_all_nodes(&self.gtpc_list6);
    gtp_node_final();
    
    sock_remove_all_nodes(&self.pfcp_list);
    sock_remove_all_nodes(&self.pfcp_list6);
    pfcp_node_final();

    pool_final(&smf_subnet_pool);
    smf_subnet_remove_all();
    
    hash_destroy(self.sess_hash);
    
    context_initialized = 0;
    
    return CORE_OK;
}

smf_context_t* smf_self()
{
    return &self;
}

static status_t smf_context_prepare()
{
    self.gtpc_port = GTPV2_C_UDP_PORT;
    self.pfcp_port = PFCP_UDP_PORT;

    return CORE_OK;
}

static status_t smf_context_validation()
{
    if (list_first(&self.gtpc_list) == NULL &&
        list_first(&self.gtpc_list6) == NULL)
    {
        d_error("No smf.gtpc in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    if (list_first(&self.subnet_list) == NULL)
    {
        d_error("No smf.ue_pool in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    if (list_first(&self.pfcp_list) == NULL &&
        list_first(&self.pfcp_list6) == NULL)
    {
        d_error("No smf.pfcp in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    if (list_first(&self.upf_n4_list) == NULL)
    {
        d_error("No smf.upf in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t smf_context_parse_config()
{
    status_t rv;
    config_t *config = &context_self()->config;
    yaml_document_t *document = NULL;
    yaml_iter_t root_iter;

    d_assert(config, return CORE_ERROR,);
    document = config->document;
    d_assert(document, return CORE_ERROR,);

    rv = smf_context_prepare();
    if (rv != CORE_OK) return rv;

    yaml_iter_init(&root_iter, document);
    while(yaml_iter_next(&root_iter))
    {
        const char *root_key = yaml_iter_key(&root_iter);
        d_assert(root_key, return CORE_ERROR,);
        /* SMF config */
        if (!strcmp(root_key, "smf"))
        {
            yaml_iter_t smf_iter;
            yaml_iter_recurse(&root_iter, &smf_iter);
            while(yaml_iter_next(&smf_iter))
            {
                const char *smf_key = yaml_iter_key(&smf_iter);
                if (!strcmp(smf_key, "gtpc"))
                {
                    yaml_iter_t gtpc_array, gtpc_iter;
                    yaml_iter_recurse(&smf_iter, &gtpc_array);
                    do
                    {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.gtpc_port;
                        const char *dev = NULL;
                        c_sockaddr_t *list = NULL;
                        sock_node_t *node = NULL;

                        if (yaml_iter_type(&gtpc_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&gtpc_iter, &gtpc_array,
                                    sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&gtpc_array) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&gtpc_array))
                                break;
                            yaml_iter_recurse(&gtpc_array, &gtpc_iter);
                        }
                        else if (yaml_iter_type(&gtpc_array) ==
                            YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&gtpc_iter))
                        {
                            const char *gtpc_key =
                                yaml_iter_key(&gtpc_iter);
                            d_assert(gtpc_key,
                                    return CORE_ERROR,);
                            if (!strcmp(gtpc_key, "family"))
                            {
                                const char *v = yaml_iter_value(&gtpc_iter);
                                if (v) family = atoi(v);
                                if (family != AF_UNSPEC &&
                                    family != AF_INET && family != AF_INET6)
                                {
                                    d_warn("Ignore family(%d) : AF_UNSPEC(%d), "
                                        "AF_INET(%d), AF_INET6(%d) ", 
                                        family, AF_UNSPEC, AF_INET, AF_INET6);
                                    family = AF_UNSPEC;
                                }
                            }
                            else if (!strcmp(gtpc_key, "addr") ||
                                    !strcmp(gtpc_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&gtpc_iter, &hostname_iter);
                                d_assert(yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE, return CORE_ERROR,);

                                do
                                {
                                    if (yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    d_assert(num <= MAX_NUM_OF_HOSTNAME,
                                            return CORE_ERROR,);
                                    hostname[num++] = 
                                        yaml_iter_value(&hostname_iter);
                                } while(
                                    yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else if (!strcmp(gtpc_key, "port"))
                            {
                                const char *v = yaml_iter_value(&gtpc_iter);
                                if (v)
                                {
                                    port = atoi(v);
                                    self.gtpc_port = port;
                                }
                            }
                            else if (!strcmp(gtpc_key, "dev"))
                            {
                                dev = yaml_iter_value(&gtpc_iter);
                            }
                            else
                                d_warn("unknown key `%s`", gtpc_key);
                        }

                        list = NULL;
                        for (i = 0; i < num; i++)
                        {
                            rv = core_addaddrinfo(&list,
                                    family, hostname[i], port, 0);
                            d_assert(rv == CORE_OK, return CORE_ERROR,);
                        }

                        if (list)
                        {
                            if (context_self()->parameter.no_ipv4 == 0)
                            {
                                rv = sock_add_node(&self.gtpc_list,
                                        &node, list, AF_INET);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            if (context_self()->parameter.no_ipv6 == 0)
                            {
                                rv = sock_add_node(&self.gtpc_list6,
                                        &node, list, AF_INET6);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            core_freeaddrinfo(list);
                        }

                        if (dev)
                        {
                            rv = sock_probe_node(
                                    context_self()->parameter.no_ipv4 ?
                                        NULL : &self.gtpc_list,
                                    context_self()->parameter.no_ipv6 ?
                                        NULL : &self.gtpc_list6,
                                    dev, self.gtpc_port);
                            d_assert(rv == CORE_OK, return CORE_ERROR,);
                        }

                    } while(yaml_iter_type(&gtpc_array) == YAML_SEQUENCE_NODE);

                    if (list_first(&self.gtpc_list) == NULL &&
                        list_first(&self.gtpc_list6) == NULL)
                    {
                        rv = sock_probe_node(
                                context_self()->parameter.no_ipv4 ?
                                    NULL : &self.gtpc_list,
                                context_self()->parameter.no_ipv6 ?
                                    NULL : &self.gtpc_list6,
                                NULL, self.gtpc_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                    }
                }
                else if (!strcmp(smf_key, "ue_pool"))
                {
                    yaml_iter_t ue_pool_array, ue_pool_iter;
                    yaml_iter_recurse(&smf_iter, &ue_pool_array);
                    do
                    {
                        smf_subnet_t *subnet = NULL;
                        const char *ipstr = NULL;
                        const char *mask_or_numbits = NULL;
                        const char *apn = NULL;

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
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&ue_pool_iter))
                        {
                            const char *ue_pool_key =
                                yaml_iter_key(&ue_pool_iter);
                            d_assert(ue_pool_key,
                                    return CORE_ERROR,);
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
                            else
                                d_warn("unknown key `%s`", ue_pool_key);
                        }

                        if (ipstr && mask_or_numbits)
                        {
                            subnet = smf_subnet_add(
                                    ipstr, mask_or_numbits, apn);
                            d_assert(subnet, return CORE_ERROR,);
                        }
                        else
                        {
                            d_warn("Ignore : addr(%s/%s), apn(%s)",
                                    ipstr, mask_or_numbits, apn);
                        }
                    } while(yaml_iter_type(&ue_pool_array) ==
                            YAML_SEQUENCE_NODE);
                }
                if (!strcmp(smf_key, "pfcp"))
                {
                    yaml_iter_t pfcp_array, pfcp_iter;
                    yaml_iter_recurse(&smf_iter, &pfcp_array);
                    do
                    {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.pfcp_port;
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
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&pfcp_iter))
                        {
                            const char *pfcp_key =
                                yaml_iter_key(&pfcp_iter);
                            d_assert(pfcp_key,
                                    return CORE_ERROR,);
                            if (!strcmp(pfcp_key, "addr") ||
                                    !strcmp(pfcp_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&pfcp_iter, &hostname_iter);
                                d_assert(yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE, return CORE_ERROR,);

                                do
                                {
                                    if (yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    d_assert(num <= MAX_NUM_OF_HOSTNAME,
                                            return CORE_ERROR,);
                                    hostname[num++] = 
                                        yaml_iter_value(&hostname_iter);
                                } while(
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
                            d_assert(rv == CORE_OK, return CORE_ERROR,);
                        }

                        if (list)
                        {
                            if (context_self()->parameter.no_ipv4 == 0)
                            {
                                rv = sock_add_node(&self.pfcp_list,
                                        &node, list, AF_INET);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            if (context_self()->parameter.no_ipv6 == 0)
                            {
                                rv = sock_add_node(&self.pfcp_list6,
                                        &node, list, AF_INET6);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            core_freeaddrinfo(list);
                        }

                    } while(yaml_iter_type(&pfcp_array) == YAML_SEQUENCE_NODE);

                    if (list_first(&self.pfcp_list) == NULL &&
                        list_first(&self.pfcp_list6) == NULL)
                    {
                        rv = sock_probe_node(
                                context_self()->parameter.no_ipv4 ?
                                    NULL : &self.pfcp_list,
                                context_self()->parameter.no_ipv6 ?
                                    NULL : &self.pfcp_list6,
                                NULL, self.pfcp_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                    }
                }
                else if (!strcmp(smf_key, "upf"))
                {
                    yaml_iter_t upf_array, upf_iter;
                    yaml_iter_recurse(&smf_iter, &upf_array);
                    do
                    {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.pfcp_port;
                        //const char *dev = NULL;
                        c_sockaddr_t *list = NULL;
                        pfcp_node_t *upf = NULL;

                        if (yaml_iter_type(&upf_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&upf_iter, &upf_array,
                                    sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&upf_array) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&upf_array))
                                break;
                            yaml_iter_recurse(&upf_array, &upf_iter);
                        }
                        else if (yaml_iter_type(&upf_array) ==
                            YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&upf_iter))
                        {
                            const char *upf_key =
                                yaml_iter_key(&upf_iter);
                            d_assert(upf_key,
                                    return CORE_ERROR,);
                            if (!strcmp(upf_key, "addr") ||
                                    !strcmp(upf_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&upf_iter, &hostname_iter);
                                d_assert(yaml_iter_type(&hostname_iter) !=
                                    YAML_MAPPING_NODE, return CORE_ERROR,);

                                do
                                {
                                    if (yaml_iter_type(&hostname_iter) ==
                                            YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&hostname_iter))
                                            break;
                                    }

                                    d_assert(num <= MAX_NUM_OF_HOSTNAME,
                                            return CORE_ERROR,);
                                    hostname[num++] = 
                                        yaml_iter_value(&hostname_iter);
                                } while(
                                    yaml_iter_type(&hostname_iter) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else
                                d_warn("unknown key `%s`", upf_key);
                        }

                        list = NULL;
                        for (i = 0; i < num; i++)
                        {
                            rv = core_addaddrinfo(&list,
                                    family, hostname[i], port, 0);
                            d_assert(rv == CORE_OK, return CORE_ERROR,);
                        }

                        if (list)
                        {
                            rv = pfcp_add_node(&self.upf_n4_list, &upf, list,
                                context_self()->parameter.no_ipv4,
                                context_self()->parameter.no_ipv6,
                                context_self()->parameter.prefer_ipv4);
                            d_assert(rv == CORE_OK, return CORE_ERROR,);

                            core_freeaddrinfo(list);
                        }

                    } while(yaml_iter_type(&upf_array) == YAML_SEQUENCE_NODE);
                }
            }
        }
    }

    rv = smf_context_validation();
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    return CORE_OK;
}

status_t smf_context_setup_trace_module()
{
    int app = context_self()->logger.trace.app;
    int gtpv2 = context_self()->logger.trace.gtpv2;
    int pfcp = context_self()->logger.trace.pfcp;

    if (app)
    {
        extern int _smf_context;
        d_trace_level(&_smf_context, app);
        extern int _smf_sm;
        d_trace_level(&_smf_sm, app);
    }
    
    if (gtpv2)
    {
        extern int _smf_gtp_path;
        d_trace_level(&_smf_gtp_path, gtpv2);

        extern int _gtp_node;
        d_trace_level(&_gtp_node, gtpv2);
        extern int _gtp_message;
        d_trace_level(&_gtp_message, gtpv2);
        extern int _gtp_path;
        d_trace_level(&_gtp_path, gtpv2);
        extern int _gtp_xact;
        d_trace_level(&_gtp_xact, gtpv2);

        extern int _tlv_msg;
        d_trace_level(&_tlv_msg, gtpv2);
    }
    
    if (pfcp)
    {
        extern int _pfcp_node;
        d_trace_level(&_pfcp_node, pfcp);
        extern int _pfcp_message;
        d_trace_level(&_pfcp_message, pfcp);
        extern int _pfcp_path;
        d_trace_level(&_pfcp_path, pfcp);
        extern int _pfcp_xact;
        d_trace_level(&_pfcp_xact, pfcp);
        extern int _smf_n4_handler;
        d_trace_level(&_smf_n4_handler, pfcp);
        extern int _smf_n4_build;
        d_trace_level(&_smf_n4_build, pfcp);
    }
    return CORE_OK;
}

static void *sess_hash_keygen(c_uint8_t *out, int *out_len,
        c_uint8_t *imsi, int imsi_len, c_int8_t *apn)
{
    memcpy(out, imsi, imsi_len);
    core_cpystrn((char*)(out + imsi_len), apn, MAX_APN_LEN + 1);
    *out_len = imsi_len + strlen((char*)(out + imsi_len));

    return out;
}

smf_sess_t *smf_sess_add_or_find_by_message(gtp_message_t *message)
{
    smf_sess_t *sess = NULL;
    c_int8_t apn[MAX_APN_LEN];

    gtp_create_session_request_t *req = &message->create_session_request;

    if (req->imsi.presence == 0)
    {
        d_error("No IMSI");
        return NULL;
    }
    if (req->access_point_name.presence == 0)
    {
        d_error("No APN");
        return NULL;
    }
    if (req->bearer_contexts_to_be_created.presence == 0)
    {
        d_error("No Bearer");
        return NULL;
    }
    if (req->bearer_contexts_to_be_created.eps_bearer_id.presence == 0)
    {
        d_error("No EPS Bearer ID");
        return NULL;
    }
    if (req->pdn_type.presence == 0)
    {
        d_error("No PDN Type");
        return NULL;
    }

    apn_parse(apn, req->access_point_name.data, req->access_point_name.len);

    d_trace(9, "smf_sess_add_by_message() [APN:%s, PDN:%d, EDI:%d]\n",
            apn, req->pdn_type.u8,
            req->bearer_contexts_to_be_created.eps_bearer_id.u8);

    sess = smf_sess_find_by_imsi_apn(req->imsi.data, req->imsi.len, apn);
    if (!sess)
    {
        sess = smf_sess_add(req->imsi.data, req->imsi.len, apn,
            req->pdn_type.u8,
            req->bearer_contexts_to_be_created.eps_bearer_id.u8);
        d_assert(sess, return NULL, "No Session Context");
    }

    return sess;
}

smf_sess_t* smf_sess_add(
        c_uint8_t *imsi, int imsi_len, c_int8_t *apn,
        c_uint8_t pdn_type, c_uint8_t ebi)
{
    smf_sess_t *sess = NULL;
    smf_subnet_t *subnet6 = NULL;
    smf_bearer_t *bearer = NULL;

    index_alloc(&smf_sess_pool, &sess);

    /* Set IMSI */
    sess->imsi_len = imsi_len;
    memcpy(sess->imsi, imsi, sess->imsi_len);
    core_buffer_to_bcd(sess->imsi, sess->imsi_len, sess->imsi_bcd);

    /* Set APN */
    core_cpystrn(sess->pdn.apn, apn, MAX_APN_LEN + 1);

    core_cpystrn(sess->pdn.apn, apn, MAX_APN_LEN + 1);

    list_init(&sess->bearer_list);

    bearer = smf_bearer_add(sess);
    d_assert(bearer, smf_sess_remove(sess); return NULL, 
            "Can't add default bearer context");
    bearer->ebi = ebi;

    sess->pdn.paa.pdn_type = pdn_type;
    if (pdn_type == GTP_PDN_TYPE_IPV4)
    {
        sess->ipv4 = smf_ue_ip_alloc(AF_INET, apn);
        d_assert(sess->ipv4, smf_sess_remove(sess); return NULL, 
                "Can't allocate IPv4 Pool");
        sess->pdn.paa.addr = sess->ipv4->addr[0];
    }
    else if (pdn_type == GTP_PDN_TYPE_IPV6)
    {
        sess->ipv6 = smf_ue_ip_alloc(AF_INET6, apn);
        d_assert(sess->ipv6, smf_sess_remove(sess); return NULL, 
                "Can't allocate IPv6 Pool");

        subnet6 = sess->ipv6->subnet;
        d_assert(subnet6, smf_sess_remove(sess); return NULL, 
                "No IPv6 subnet");

        sess->pdn.paa.len = subnet6->prefixlen;
        memcpy(sess->pdn.paa.addr6, sess->ipv6->addr, IPV6_LEN);
    }
    else if (pdn_type == GTP_PDN_TYPE_IPV4V6)
    {
        sess->ipv4 = smf_ue_ip_alloc(AF_INET, apn);
        d_assert(sess->ipv4, smf_sess_remove(sess); return NULL, 
                "Can't allocate IPv4 Pool");
        sess->ipv6 = smf_ue_ip_alloc(AF_INET6, apn);
        d_assert(sess->ipv6, smf_sess_remove(sess); return NULL, 
                "Can't allocate IPv6 Pool");

        subnet6 = sess->ipv6->subnet;
        d_assert(subnet6, smf_sess_remove(sess); return NULL, 
                "No IPv6 subnet");

        sess->pdn.paa.both.addr = sess->ipv4->addr[0];
        sess->pdn.paa.both.len = subnet6->prefixlen;
        memcpy(sess->pdn.paa.both.addr6, sess->ipv6->addr, IPV6_LEN);
    }
    else
        d_assert(0, return NULL, "Unsupported PDN Type(%d)", pdn_type);
    
    /* Generate Hash Key : IMSI + APN */
    sess_hash_keygen(sess->hash_keybuf, &sess->hash_keylen,
            imsi, imsi_len, apn);
    hash_set(self.sess_hash, sess->hash_keybuf, sess->hash_keylen, sess);
    
    sess->upf_node = list_first(&smf_self()->upf_n4_list);
    
    smf_pdr_t *ul_pdr = smf_pdr_add(bearer);
    sess->ul_pdr = ul_pdr;
    ul_pdr->precedence = PGWC_PRECEDENCE_BASE;
    ul_pdr->outer_header_removal = PFCP_OUTER_HDR_RMV_DESC_GTPU_IP4;
    ul_pdr->source_interface = PFCP_SRC_INTF_ACCESS;
    ul_pdr->far = smf_far_add(bearer);
    ul_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_FORW;
    ul_pdr->far->destination_interface = PFCP_FAR_DEST_INTF_CORE;
    
    smf_pdr_t *dl_pdr = smf_pdr_add(bearer);
    sess->dl_pdr = dl_pdr;
    dl_pdr->precedence = PGWC_PRECEDENCE_BASE;
    dl_pdr->outer_header_removal = PFCP_OUTER_HDR_RMV_DESC_NULL;
    dl_pdr->source_interface = PFCP_SRC_INTF_CORE;
    dl_pdr->far = smf_far_add(bearer);
    dl_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_FORW;
    dl_pdr->far->destination_interface = PFCP_FAR_DEST_INTF_ACCESS;
    
    return sess;
}

status_t smf_sess_remove(smf_sess_t *sess)
{
    d_assert(self.sess_hash, return CORE_ERROR, "Null param");
    d_assert(sess, return CORE_ERROR, "Null param");

    hash_set(self.sess_hash, sess->hash_keybuf, sess->hash_keylen, NULL);

    if (sess->ipv4)
        smf_ue_ip_free(sess->ipv4);
    if (sess->ipv6)
        smf_ue_ip_free(sess->ipv6);

    index_free(&smf_sess_pool, sess);

    return CORE_OK;
}

status_t smf_sess_remove_all()
{
    hash_index_t *hi = NULL;
    smf_sess_t *sess = NULL;

    for (hi = smf_sess_first(); hi; hi = smf_sess_next(hi))
    {
        sess = smf_sess_this(hi);
        smf_sess_remove(sess);
    }

    return CORE_OK;
}

smf_sess_t *smf_sess_this(hash_index_t *hi)
{
    d_assert(hi, return NULL, "Null param");
    return hash_this_val(hi);
}

hash_index_t* smf_sess_first()
{
    d_assert(self.sess_hash, return NULL, "Null param");
    return hash_first(self.sess_hash);
}

hash_index_t* smf_sess_next(hash_index_t *hi)
{
    return hash_next(hi);
}

smf_sess_t* smf_sess_find_by_imsi_apn(
    c_uint8_t *imsi, int imsi_len, c_int8_t *apn)
{
    c_uint8_t keybuf[MAX_IMSI_LEN+MAX_APN_LEN+1];
    int keylen = 0;

    d_assert(self.sess_hash, return NULL, "Null param");

    sess_hash_keygen(keybuf, &keylen, imsi, imsi_len, apn);
    return (smf_sess_t *)hash_get(self.sess_hash, keybuf, keylen);
}

smf_sess_t* smf_sess_find(index_t index)
{
    return index_find(&smf_sess_pool, index);
}

smf_sess_t* smf_sess_find_by_seid(c_uint64_t seid)
{
    return smf_sess_find(seid & 0xFFFFFFFF);
}

smf_sess_t* smf_sess_find_by_teid(c_uint32_t teid)
{
    return smf_sess_find(teid);
}

smf_bearer_t* smf_bearer_add(smf_sess_t *sess)
{
    smf_bearer_t *bearer = NULL;

    d_assert(sess, return NULL, "Null param");

    index_alloc(&smf_bearer_pool, &bearer);
    d_assert(bearer, return NULL, "Bearer context allocation failed");

    bearer->name = NULL;

    list_init(&bearer->pf_list);
    
    bearer->sess = sess;
    bearer->gnode = NULL;

    list_append(&sess->bearer_list, bearer);

    return bearer;
}

smf_pdr_t* smf_pdr_add(smf_bearer_t *bearer)
{
    smf_pdr_t *pdr = NULL;

    d_assert(bearer, return NULL, "Null param");

    index_alloc(&smf_pdr_pool, &pdr);
    d_assert(pdr, return NULL, "PDR context allocation failed");

    pdr->pdr_id = htons(pdr->index);
    
    pdr->bearer = bearer;

    return pdr;
}

status_t smf_pdr_remove(smf_pdr_t *pdr)
{
    d_assert(pdr, return CORE_ERROR, "Null param");
    d_assert(pdr->bearer, return CORE_ERROR, "Null param");
    d_assert(pdr->bearer->sess, return CORE_ERROR, "Null param");

    if (pdr->far)
    {
        smf_far_remove(pdr->far);
    }    

    index_free(&smf_pdr_pool, pdr);

    return CORE_OK;
}

smf_pdr_t* smf_pdr_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&smf_pdr_pool, index);
}

smf_pdr_t* smf_pdr_find_by_pdr_id(c_uint16_t pdr_id)
{
    return smf_pdr_find(pdr_id);
}

smf_far_t* smf_far_add(smf_bearer_t *bearer)
{
    smf_far_t *far = NULL;

    d_assert(bearer, return NULL, "Null param");

    index_alloc(&smf_far_pool, &far);
    d_assert(far, return NULL, "FAR context allocation failed");

    far->far_id = htonl(far->index);
    
    far->bearer = bearer;


    return far;
}

status_t smf_far_remove(smf_far_t *far)
{
    d_assert(far, return CORE_ERROR, "Null param");

    index_free(&smf_far_pool, far);

    return CORE_OK;
}


status_t smf_ue_pool_generate()
{
    int j;
    smf_subnet_t *subnet = NULL;

    for (subnet = smf_subnet_first(); subnet; subnet = smf_subnet_next(subnet))
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
                d_assert(0, return CORE_ERROR,);
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
                d_assert(0, return CORE_ERROR,);
        }
        else
            d_assert(0, return CORE_ERROR,);
        
        for (j = 0; j < 4; j++)
        {
            broadcast[j] = subnet->sub.sub[j] + ~subnet->sub.mask[j];
        }

        for (j = 0; j < mask_count && index < MAX_POOL_OF_SESS; j++)
        {
            smf_ue_ip_t *ue_ip = NULL;
            int maxbytes = 0;
            int lastindex = 0;

            ue_ip = &subnet->pool.pool[index];
            d_assert(ue_ip, return CORE_ERROR,);
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
            if (memcmp(ue_ip->addr, subnet->sub.sub, maxbytes) == 0) continue;

            /* Exclude Broadcast Address */
            if (memcmp(ue_ip->addr, broadcast, maxbytes) == 0) continue;

            /* Exclude TUN IP Address */
            if (memcmp(ue_ip->addr, subnet->gw.sub, maxbytes) == 0) continue;

            index++;
        }
        subnet->pool.size = subnet->pool.avail = index;
    }

    return CORE_OK;
}

static smf_subnet_t *find_subnet(int family, const char *apn)
{
    smf_subnet_t *subnet = NULL;

    d_assert(apn, return NULL,);
    d_assert(family == AF_INET || family == AF_INET6, return NULL,);

    for (subnet = smf_subnet_first(); subnet; subnet = smf_subnet_next(subnet))
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

    for (subnet = smf_subnet_first(); subnet; subnet = smf_subnet_next(subnet))
    {
        if (strlen(subnet->apn) == 0)
        {
            if (subnet->family == family &&
                pool_avail(&subnet->pool))
            {
                return subnet;
            }
        }
    }

    if (subnet == NULL)
        d_error("CHECK CONFIGURATION: Cannot find UE Pool");

    return subnet;
}

smf_ue_ip_t *smf_ue_ip_alloc(int family, const char *apn)
{
    smf_subnet_t *subnet = NULL;
    smf_ue_ip_t *ue_ip = NULL;

    d_assert(apn, return NULL,);

    subnet = find_subnet(family, apn);
    d_assert(subnet, return NULL,);

    pool_alloc_node(&subnet->pool, &ue_ip);
    d_assert(ue_ip, return NULL,);

    return ue_ip;
}

status_t smf_ue_ip_free(smf_ue_ip_t *ue_ip)
{
    smf_subnet_t *subnet = NULL;

    d_assert(ue_ip, return CORE_ERROR,);
    subnet = ue_ip->subnet;

    d_assert(subnet, return CORE_ERROR,);
    pool_free_node(&subnet->pool, ue_ip);

    return CORE_OK;
}

smf_subnet_t *smf_subnet_add(
        const char *ipstr, const char *mask_or_numbits,
        const char *apn)
{
    status_t rv;
    smf_subnet_t *subnet = NULL;

    d_assert(ipstr, return NULL,);
    d_assert(mask_or_numbits, return NULL,);

    pool_alloc_node(&smf_subnet_pool, &subnet);
    d_assert(subnet, return NULL,);
    memset(subnet, 0, sizeof *subnet);

    rv = core_ipsubnet(&subnet->gw, ipstr, NULL);
    d_assert(rv == CORE_OK, return NULL,);

    rv = core_ipsubnet(&subnet->sub, ipstr, mask_or_numbits);
    d_assert(rv == CORE_OK, return NULL,);

    if (apn)
        strcpy(subnet->apn, apn);

    subnet->family = subnet->gw.family;
    subnet->prefixlen = atoi(mask_or_numbits);

    pool_init(&subnet->pool, MAX_POOL_OF_SESS);

    list_append(&self.subnet_list, subnet);

    return subnet;
}

status_t smf_subnet_remove(smf_subnet_t *subnet)
{
    d_assert(subnet, return CORE_ERROR, "Null param");

    list_remove(&self.subnet_list, subnet);

    if (pool_used(&subnet->pool))
    {
        d_warn("%d not freed in ue_ip_pool[%d] in PGW-Context",
                pool_used(&subnet->pool), pool_size(&subnet->pool));
    }
    d_trace(9, "%d not freed in ue_ip_pool[%d] in PGW-Context\n",
            pool_used(&subnet->pool), pool_size(&subnet->pool));
    pool_final(&subnet->pool);

    pool_free_node(&smf_subnet_pool, subnet);

    return CORE_OK;
}

status_t smf_subnet_remove_all()
{
    smf_subnet_t *subnet = NULL, *next_subnet = NULL;

    subnet = smf_subnet_first();
    while (subnet)
    {
        next_subnet = smf_subnet_next(subnet);

        smf_subnet_remove(subnet);

        subnet = next_subnet;
    }

    return CORE_OK;
}

smf_subnet_t* smf_subnet_first()
{
    return list_first(&self.subnet_list);
}

smf_subnet_t* smf_subnet_next(smf_subnet_t *subnet)
{
    return list_next(subnet);
}
