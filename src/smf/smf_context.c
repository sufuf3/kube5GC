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

#include "pfcp/pfcp_node.h"

#include "smf_context.h"

static smf_context_t self;
static fd_config_t s_fd_conf;
static int context_initialized = 0;

index_declare(smf_sess_pool, smf_sess_t, MAX_POOL_OF_SESS);
index_declare(smf_bearer_pool, smf_bearer_t, MAX_POOL_OF_BEARER);

pool_declare(smf_subnet_pool, smf_subnet_t, MAX_NUM_OF_SUBNET);
pool_declare(smf_pf_pool, smf_pf_t, MAX_POOL_OF_PF);

index_declare(smf_pdr_pool, smf_pdr_t, MAX_POOL_OF_SESS * 2);
index_declare(smf_far_pool, smf_far_t, MAX_POOL_OF_SESS);
index_declare(smf_urr_pool, smf_urr_t, MAX_POOL_OF_SESS);
index_declare(smf_qer_pool, smf_qer_t, MAX_POOL_OF_SESS);

status_t smf_context_init()
{
    d_assert(context_initialized == 0, return CORE_ERROR,
            "SMF context already has been initialized");

    /* Initialize FreeDiameter Config */
    memset(&s_fd_conf, 0, sizeof(fd_config_t));

    /* Initialize SMF context */
    memset(&self, 0, sizeof(smf_context_t));
    self.fd_config = &s_fd_conf;

    index_init(&smf_sess_pool, MAX_POOL_OF_SESS);
    index_init(&smf_bearer_pool, MAX_POOL_OF_BEARER);
    
    index_init(&smf_pdr_pool, MAX_POOL_OF_SESS * 2);
    index_init(&smf_far_pool, MAX_POOL_OF_SESS);
    index_init(&smf_urr_pool, MAX_POOL_OF_SESS);
    index_init(&smf_qer_pool, MAX_POOL_OF_SESS);
    
    list_init(&self.pfcp_list);
    list_init(&self.pfcp_list6);
    list_init(&self.upf_n4_list);
    pfcp_node_init();
    
    list_init(&self.subnet_list);
    pool_init(&smf_subnet_pool, MAX_NUM_OF_SUBNET);
    
    pool_init(&smf_pf_pool, MAX_POOL_OF_PF);
    
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
            
    smf_sess_remove_all();

    d_assert(self.sess_hash, , "Null param");
    hash_destroy(self.sess_hash);


    index_final(&smf_sess_pool);
    index_final(&smf_bearer_pool);

    index_final(&smf_pdr_pool);
    index_final(&smf_far_pool);
    index_final(&smf_urr_pool);
    index_final(&smf_qer_pool);
    
    pfcp_remove_all_nodes(&self.upf_n4_list);
    sock_remove_all_nodes(&self.pfcp_list);
    sock_remove_all_nodes(&self.pfcp_list6);
    pfcp_node_final();

    pool_final(&smf_subnet_pool);
    smf_subnet_remove_all();
    
    pool_final(&smf_pf_pool);
    
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
    self.pfcp_port = PFCP_UDP_PORT;
    self.fd_config->cnf_port = DIAMETER_PORT;
    self.fd_config->cnf_port_tls = DIAMETER_SECURE_PORT;

    return CORE_OK;
}

static status_t smf_context_validation()
{
    if (self.fd_conf_path == NULL &&
        (self.fd_config->cnf_diamid == NULL ||
        self.fd_config->cnf_diamrlm == NULL ||
        self.fd_config->cnf_addr == NULL))
    {
        d_error("No smf.freeDiameter in '%s'",
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
    if (self.fd_conf_path == NULL &&
        (self.fd_config->cnf_diamid == NULL ||
        self.fd_config->cnf_diamrlm == NULL ||
        self.fd_config->cnf_addr == NULL))
    {
        d_error("No smf.freeDiameter in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    if (self.dns[0] == NULL && self.dns6[0] == NULL)
    {
        d_error("No smf.dns in '%s'",
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
                if (!strcmp(smf_key, "ue_pool"))
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
                else if (!strcmp(smf_key, "freeDiameter"))
                {
                    yaml_node_t *node = 
                        yaml_document_get_node(document,smf_iter.pair->value);
                    d_assert(node, return CORE_ERROR,);
                    if (node->type == YAML_SCALAR_NODE)
                    {
                        self.fd_conf_path = yaml_iter_value(&smf_iter);
                    }
                    else if (node->type == YAML_MAPPING_NODE)
                    {
                        yaml_iter_t fd_iter;
                        yaml_iter_recurse(&smf_iter, &fd_iter);

                        while(yaml_iter_next(&fd_iter))
                        {
                            const char *fd_key = yaml_iter_key(&fd_iter);
                            d_assert(fd_key, return CORE_ERROR,);
                            if (!strcmp(fd_key, "identity"))
                            {
                                self.fd_config->cnf_diamid =
                                    yaml_iter_value(&fd_iter);
                            }
                            else if (!strcmp(fd_key, "realm"))
                            {
                                self.fd_config->cnf_diamrlm = 
                                    yaml_iter_value(&fd_iter);
                            }
                            else if (!strcmp(fd_key, "port"))
                            {
                                const char *v = yaml_iter_value(&fd_iter);
                                if (v) self.fd_config->cnf_port = atoi(v);
                            }
                            else if (!strcmp(fd_key, "sec_port"))
                            {
                                const char *v = yaml_iter_value(&fd_iter);
                                if (v) self.fd_config->cnf_port_tls = atoi(v);
                            }
                            else if (!strcmp(fd_key, "no_sctp"))
                            {
                                self.fd_config->cnf_flags.no_sctp =
                                    yaml_iter_bool(&fd_iter);
                            }
                            else if (!strcmp(fd_key, "listen_on"))
                            {
                                self.fd_config->cnf_addr = 
                                    yaml_iter_value(&fd_iter);
                            }
                            else if (!strcmp(fd_key, "load_extension"))
                            {
                                yaml_iter_t ext_array, ext_iter;
                                yaml_iter_recurse(&fd_iter, &ext_array);
                                do
                                {
                                    const char *module = NULL;
                                    const char *conf = NULL;

                                    if (yaml_iter_type(&ext_array) ==
                                        YAML_MAPPING_NODE)
                                    {
                                        memcpy(&ext_iter, &ext_array,
                                                sizeof(yaml_iter_t));
                                    }
                                    else if (yaml_iter_type(&ext_array) ==
                                        YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&ext_array))
                                            break;
                                        yaml_iter_recurse(
                                                &ext_array, &ext_iter);
                                    }
                                    else if (yaml_iter_type(&ext_array) ==
                                        YAML_SCALAR_NODE)
                                    {
                                        break;
                                    }
                                    else
                                        d_assert(0, return CORE_ERROR,);

                                    while(yaml_iter_next(&ext_iter))
                                    {
                                        const char *ext_key =
                                            yaml_iter_key(&ext_iter);
                                        d_assert(ext_key,
                                                return CORE_ERROR,);
                                        if (!strcmp(ext_key, "module"))
                                        {
                                            module = yaml_iter_value(&ext_iter);
                                        }
                                        else if (!strcmp(ext_key, "conf"))
                                        {
                                            conf = yaml_iter_value(&ext_iter);
                                        }
                                        else
                                            d_warn("unknown key `%s`", ext_key);
                                    }

                                    if (module)
                                    {
                                        self.fd_config->
                                            ext[self.fd_config->num_of_ext].
                                                module = module;
                                        self.fd_config->
                                            ext[self.fd_config->num_of_ext].
                                                conf = conf;
                                        self.fd_config->num_of_ext++;
                                    }
                                } while(yaml_iter_type(&ext_array) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else if (!strcmp(fd_key, "connect"))
                            {
                                yaml_iter_t conn_array, conn_iter;
                                yaml_iter_recurse(&fd_iter, &conn_array);
                                do
                                {
                                    const char *identity = NULL;
                                    const char *addr = NULL;
                                    c_uint16_t port = 0;

                                    if (yaml_iter_type(&conn_array) ==
                                        YAML_MAPPING_NODE)
                                    {
                                        memcpy(&conn_iter, &conn_array,
                                                sizeof(yaml_iter_t));
                                    }
                                    else if (yaml_iter_type(&conn_array) ==
                                        YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&conn_array))
                                            break;
                                        yaml_iter_recurse(&conn_array, &conn_iter);
                                    }
                                    else if (yaml_iter_type(&conn_array) ==
                                        YAML_SCALAR_NODE)
                                    {
                                        break;
                                    }
                                    else
                                        d_assert(0, return CORE_ERROR,);

                                    while(yaml_iter_next(&conn_iter))
                                    {
                                        const char *conn_key =
                                            yaml_iter_key(&conn_iter);
                                        d_assert(conn_key,
                                                return CORE_ERROR,);
                                        if (!strcmp(conn_key, "identity"))
                                        {
                                            identity = yaml_iter_value(&conn_iter);
                                        }
                                        else if (!strcmp(conn_key, "addr"))
                                        {
                                            addr = yaml_iter_value(&conn_iter);
                                        }
                                        else if (!strcmp(conn_key, "port"))
                                        {
                                            const char *v =
                                                yaml_iter_value(&conn_iter);
                                            if (v) port = atoi(v);
                                        }
                                        else
                                            d_warn("unknown key `%s`", conn_key);
                                    }

                                    if (identity && addr)
                                    {
                                        self.fd_config->
                                            conn[self.fd_config->num_of_conn].
                                                identity = identity;
                                        self.fd_config->
                                            conn[self.fd_config->num_of_conn].
                                                addr = addr;
                                        self.fd_config->
                                            conn[self.fd_config->num_of_conn].
                                                port = port;
                                        self.fd_config->num_of_conn++;
                                    }
                                } while(yaml_iter_type(&conn_array) ==
                                        YAML_SEQUENCE_NODE);
                                
                            }
                            else
                                d_warn("unknown key `%s`", fd_key);
                        }
                    }
                }
                else if (!strcmp(smf_key, "dns"))
                {
                    yaml_iter_t dns_iter;
                    yaml_iter_recurse(&smf_iter, &dns_iter);
                    d_assert(yaml_iter_type(&dns_iter) !=
                        YAML_MAPPING_NODE, return CORE_ERROR,);

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
                            d_assert(rv == CORE_OK, return CORE_ERROR,);

                            if (ipsub.family == AF_INET)
                            {
                                if (self.dns[0] && self.dns[1])
                                    d_warn("Ignore DNS : %s", v);
                                else if (self.dns[0]) self.dns[1] = v;
                                else self.dns[0] = v;
                            }
                            else if (ipsub.family == AF_INET6)
                            {
                                if (self.dns6[0] && self.dns6[1])
                                    d_warn("Ignore DNS : %s", v);
                                else if (self.dns6[0]) self.dns6[1] = v;
                                else self.dns6[0] = v;
                            }
                            else
                                d_warn("Ignore DNS : %s", v);
                        }

                    } while(
                        yaml_iter_type(&dns_iter) ==
                            YAML_SEQUENCE_NODE);
                }
                else if (!strcmp(smf_key, "http"))
                {
                    yaml_iter_t http_iter;
                    yaml_iter_recurse(&smf_iter, &http_iter);
                    while(yaml_iter_next(&http_iter))
                    {
                        const char *http_key = yaml_iter_key(&http_iter);
                        if (!strcmp(http_key, "addr"))
                        {
                            strcpy(self.rest_api_addr, yaml_iter_value(&http_iter));
                        }
                        else if (!strcmp(http_key, "port"))
                        {
                            strcpy(self.rest_api_port, yaml_iter_value(&http_iter));
                        }
                    }
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
    int pfcp = context_self()->logger.trace.pfcp;
    int sbi = context_self()->logger.trace.sbi;

    if (app)
    {
        extern int _smf_context;
        d_trace_level(&_smf_context, app);
        extern int _smf_sm;
        d_trace_level(&_smf_sm, app);
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
    if (sbi)
    {
        extern int _json_tranform;
        d_trace_level(&_json_tranform, sbi);
        extern int _smf_sbi_path;
        d_trace_level(&_smf_sbi_path, sbi);
        extern int _smf_n11_handler;
        d_trace_level(&_smf_n11_handler, sbi);
        extern int _smf_n11_build;
        d_trace_level(&_smf_n11_build, sbi);
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
    
    sess->smf_n4_seid = 0;
    
    /* Generate Hash Key : IMSI + APN */
    sess_hash_keygen(sess->hash_keybuf, &sess->hash_keylen,
            imsi, imsi_len, apn);
    hash_set(self.sess_hash, sess->hash_keybuf, sess->hash_keylen, sess);
    
    sess->upf_node = list_first(&smf_self()->upf_n4_list);
    bearer->upf_s1u_teid = bearer->index;
    
    smf_pdr_t *ul_pdr = smf_pdr_add(bearer);
    sess->ul_pdr = ul_pdr;
    ul_pdr->precedence = PGWC_PRECEDENCE_BASE;
    ul_pdr->outer_header_removal = PFCP_OUTER_HDR_RMV_DESC_GTPU_IP4;
    ul_pdr->source_interface = PFCP_SRC_INTF_ACCESS;
    ul_pdr->far = smf_far_add(bearer);
    ul_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_FORW;
    ul_pdr->far->destination_interface = PFCP_FAR_DEST_INTF_CORE;
    d_trace(10, "SEID: [0x%016llx] UL PDR ID: [%d] UL FAR ID: [%d]\n",
                sess->smf_n4_seid, ul_pdr->pdr_id, ul_pdr->far->far_id);
    
    smf_pdr_t *dl_pdr = smf_pdr_add(bearer);
    sess->dl_pdr = dl_pdr;
    dl_pdr->precedence = PGWC_PRECEDENCE_BASE;
    dl_pdr->outer_header_removal = PFCP_OUTER_HDR_RMV_DESC_NULL;
    dl_pdr->source_interface = PFCP_SRC_INTF_CORE;
    dl_pdr->far = smf_far_add(bearer);
    dl_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_FORW;
    dl_pdr->far->destination_interface = PFCP_FAR_DEST_INTF_ACCESS;
    d_trace(10, "SEID: [0x%016llx] DL PDR ID: [%d] DL FAR ID: [%d]\n",
                sess->smf_n4_seid, dl_pdr->pdr_id, dl_pdr->far->far_id);
    
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

    list_append(&sess->bearer_list, bearer);

    return bearer;
}

smf_bearer_t* smf_bearer_find_by_name(smf_sess_t *sess, c_int8_t *name)
{
    smf_bearer_t *bearer = NULL;
    
    d_assert(sess, return NULL, "Null param");
    d_assert(name, return NULL, "Null param");

    bearer = smf_bearer_first(sess);
    while (bearer)
    {
        if (bearer->name && strcmp(bearer->name, name) == 0)
            return bearer;

        bearer = smf_bearer_next(bearer);
    }

    return NULL;
}

smf_bearer_t* smf_bearer_find_by_qci_arp(smf_sess_t *sess, 
                                c_uint8_t qci,
                                c_uint8_t priority_level,
                                c_uint8_t pre_emption_capability,
                                c_uint8_t pre_emption_vulnerability)
{
    smf_bearer_t *bearer = NULL;

    d_assert(sess, return NULL, "Null param");

    bearer = smf_default_bearer_in_sess(sess);
    if (!bearer) return NULL;

    if (sess->pdn.qos.qci == qci &&
        sess->pdn.qos.arp.priority_level == priority_level &&
        sess->pdn.qos.arp.pre_emption_capability == 
            pre_emption_capability &&
        sess->pdn.qos.arp.pre_emption_vulnerability == 
            pre_emption_vulnerability)
    {
        return bearer;
    }

    bearer = smf_bearer_next(bearer);
    while(bearer)
    {
        if (bearer->qos.qci == qci &&
            bearer->qos.arp.priority_level == priority_level &&
            bearer->qos.arp.pre_emption_capability == 
                pre_emption_capability &&
            bearer->qos.arp.pre_emption_vulnerability == 
                pre_emption_vulnerability)
        {
            return bearer;
        }
        bearer = smf_bearer_next(bearer);
    }

    return NULL;
}

smf_bearer_t* smf_default_bearer_in_sess(smf_sess_t *sess)
{
    return smf_bearer_first(sess);
}

smf_bearer_t* smf_bearer_first(smf_sess_t *sess)
{
    d_assert(sess, return NULL, "Null param");
    return list_first(&sess->bearer_list);
}

smf_bearer_t* smf_bearer_next(smf_bearer_t *bearer)
{
    return list_next(bearer);
}

smf_bearer_t* smf_bearer_find_by_ebi(smf_sess_t *sess, c_uint8_t ebi)
{
    smf_bearer_t *bearer = NULL;
    
    d_assert(sess, return NULL, "Null param");

    bearer = smf_bearer_first(sess);
    while (bearer)
    {
        if (bearer->ebi == ebi)
            break;

        bearer = smf_bearer_next(bearer);
    }

    return bearer;
}

smf_pdr_t* smf_pdr_add(smf_bearer_t *bearer)
{
    smf_pdr_t *pdr = NULL;

    d_assert(bearer, return NULL, "Null param");

    index_alloc(&smf_pdr_pool, &pdr);
    d_assert(pdr, return NULL, "PDR context allocation failed");

    pdr->pdr_id = pdr->index;
    
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

    far->far_id = far->index;
    far->bearer = bearer;

    return far;
}

status_t smf_far_remove(smf_far_t *far)
{
    d_assert(far, return CORE_ERROR, "Null param");

    index_free(&smf_far_pool, far);

    return CORE_OK;
}

smf_pf_t *smf_pf_add(smf_bearer_t *bearer, c_uint32_t precedence)
{
    smf_pf_t *pf = NULL;

    d_assert(bearer, return NULL, "Null param");

    pool_alloc_node(&smf_pf_pool, &pf);
    d_assert(pf, return NULL, "Null param");

    pf->identifier = NEXT_ID(bearer->pf_identifier, 1, 15);
    pf->bearer = bearer;

    list_append(&bearer->pf_list, pf);

    return pf;
}

status_t smf_pf_remove(smf_pf_t *pf)
{
    d_assert(pf, return CORE_ERROR, "Null param");
    d_assert(pf->bearer, return CORE_ERROR, "Null param");

    list_remove(&pf->bearer->pf_list, pf);
    pool_free_node(&smf_pf_pool, pf);

    return CORE_OK;
}

status_t smf_pf_remove_all(smf_bearer_t *bearer)
{
    smf_pf_t *pf = NULL, *next_pf = NULL;

    d_assert(bearer, return CORE_ERROR, "Null param");
    
    pf = smf_pf_first(bearer);
    while (pf)
    {
        next_pf = smf_pf_next(pf);

        smf_pf_remove(pf);

        pf = next_pf;
    }

    return CORE_OK;
}

smf_pf_t* smf_pf_find_by_id(smf_bearer_t *bearer, c_uint8_t id)
{
    smf_pf_t *pf = NULL;
    
    pf = smf_pf_first(bearer);
    while (pf)
    {
        if (pf->identifier == id)
            return pf;

        pf = smf_pf_next(pf);
    }

    return CORE_OK;
}

smf_pf_t* smf_pf_first(smf_bearer_t *bearer)
{
    return list_first(&bearer->pf_list);
}

smf_pf_t* smf_pf_next(smf_pf_t *pf)
{
    return list_next(pf);
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
        d_warn("%d not freed in ue_ip_pool[%d] in SMF-Context",
                pool_used(&subnet->pool), pool_size(&subnet->pool));
    }
    d_trace(9, "%d not freed in ue_ip_pool[%d] in SMF-Context\n",
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

smf_sess_t *smf_sess_add_or_find_by_JsonCreateSession(create_session_t *createSession)
{
    smf_sess_t *sess = NULL;
    c_int8_t apn[MAX_APN_LEN] = {0};
    if (createSession->imsi == NULL) {
        d_error("No IMSI");
        return NULL;
    }

    if (createSession->apn != NULL) {
        memcpy(apn, createSession->apn, strlen(createSession->apn));
    }
    else {
        d_error("No APN");
        return NULL;
    }
        
    d_trace(9, "smf_sess_add_by_message() [APN:%s, PDN:%d, EDI:%d]\n",
             apn, createSession->pdn.pdn_type, createSession->ebi);

    sess = smf_sess_find_by_imsi_apn(createSession->imsi, createSession->imsi_len, apn);
    if (!sess)
    {
        d_info("APN_type: %d", createSession->pdn.pdn_type);
        sess = smf_sess_add(createSession->imsi, createSession->imsi_len, apn,
            createSession->pdn.paa.pdn_type,
            createSession->ebi);
        d_assert(sess, return NULL, "No Session Context");
    }
    else 
    {
        sess->dl_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_FORW;
    } 

    return sess;
}

smf_sess_t *smf_sess_find_by_JsonUpdateSession(modify_bearer_t *pModifyBearer)
{
    smf_sess_t *sess = NULL;
    c_int8_t apn[MAX_APN_LEN] = {0};
    if (pModifyBearer->imsi == NULL) {
        d_error("No IMSI");
        return NULL;
    }
    if (pModifyBearer->apn != NULL) {
        memcpy(apn, pModifyBearer->apn, strlen(pModifyBearer->apn));
    }
    else {
        d_error("No APN");
        return NULL;
    }

    sess = smf_sess_find_by_imsi_apn(pModifyBearer->imsi, pModifyBearer->imsi_len, apn);
    if (!sess)
    {
        d_assert(sess, return NULL, "No Session Context");
    }
    return sess;
}
