#define TRACE_MODULE _amf4g_context

#include "core_debug.h"
#include "core_pool.h"
#include "core_lib.h"
#include "core_msgq.h"
#include "core_fsm.h"
#include "core_network.h"

#include <mongoc.h>
#include <yaml.h>
#include "app/yaml_helper.h"

#include "s1ap/s1ap_message.h"
#include "gtp/gtp_xact.h"
#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"

#include "fd/fd_lib.h"

#include "app/context.h"
#include "nas_conv.h"
#include "amf4g_context.h"
#include "amf4g_event.h"
#include "s1ap_path.h"
#include "amf4g_sm.h"

#define MAX_CELL_PER_ENB            8

static amf_context_t self;
static fd_config_t g_fd_conf;

index_declare(amf_ran_pool, amf_ran_t, MAX_NUM_OF_RAN);
index_declare(amf_ue_pool, amf_ue_t, MAX_POOL_OF_5G_UE);
index_declare(ran_ue_pool, ran_ue_t, MAX_POOL_OF_5G_UE);

index_declare(amf4g_enb_pool, amf4g_enb_t, MAX_NUM_OF_ENB);
index_declare(amf4g_ue_pool, amf4g_ue_t, MAX_POOL_OF_UE);
index_declare(enb_ue_pool, enb_ue_t, MAX_POOL_OF_UE);
index_declare(amf4g_sess_pool, amf4g_sess_t, MAX_POOL_OF_SESS);
index_declare(amf4g_bearer_pool, amf4g_bearer_t, MAX_POOL_OF_BEARER);

static int context_initialized = 0;

status_t amf4g_context_init()
{
    d_assert(context_initialized == 0, return CORE_ERROR,
            "AMF4G context already has been context_initialized");

    /* Initial FreeDiameter Config */
    memset(&g_fd_conf, 0, sizeof(fd_config_t));

    /* Initialize AMF4G context */
    memset(&self, 0, sizeof(amf_context_t));
    self.fd_config = &g_fd_conf;

    list_init(&self.s1ap_list);
    list_init(&self.s1ap_list6);

    list_init(&self.ngap_list);
    list_init(&self.ngap_list6);

    index_init(&amf_ran_pool, MAX_NUM_OF_RAN);

    index_init(&amf_ue_pool, MAX_POOL_OF_5G_UE);
    index_init(&ran_ue_pool, MAX_POOL_OF_5G_UE);

    index_init(&amf4g_enb_pool, MAX_NUM_OF_ENB);

    index_init(&amf4g_ue_pool, MAX_POOL_OF_UE);
    index_init(&enb_ue_pool, MAX_POOL_OF_UE);
    index_init(&amf4g_sess_pool, MAX_POOL_OF_SESS);
    index_init(&amf4g_bearer_pool, MAX_POOL_OF_BEARER);
    pool_init(&self.m_tmsi, MAX_POOL_OF_UE);

    self.ran_sock_hash = hash_make();
    self.ran_addr_hash = hash_make();
    self.ran_id_hash = hash_make();
    self.amf_ue_ngap_id_hash = hash_make();

    self.enb_sock_hash = hash_make();
    self.enb_addr_hash = hash_make();
    self.enb_id_hash = hash_make();
    self.mme_ue_s1ap_id_hash = hash_make();
    self.imsi_ue_hash = hash_make();
    self.guti_ue_hash = hash_make();

    /* Timer value */
    self.t3413_value = 2;               /* Paging retry timer: 2 secs */
    self.s1_holding_timer_value = 30;   /* S1 holding timer: 30 secs */

    self.ng_holding_timer_value = 30;   /* NG holding timer: 30 secs */

    /* Status */
    self.overload_started = false;

    context_initialized = 1;

    return CORE_OK;
}

status_t amf4g_context_final()
{
    d_assert(context_initialized == 1, return CORE_ERROR,
            "AMF4G context already has been finalized");

    amf_ran_remove_all();
    amf4g_enb_remove_all();
    amf4g_ue_remove_all();

    if (pool_used(&self.m_tmsi))
    {
        d_error("%d not freed in M-TMSI pool[%d] in AMF4G-Context",
                pool_used(&self.m_tmsi), pool_size(&self.m_tmsi));
    }
    d_trace(9, "%d not freed in M-TMSI pool[%d] in AMF4G-Context\n",
            pool_used(&self.m_tmsi), pool_size(&self.m_tmsi));

    d_assert(self.ran_sock_hash, , "Null param");
    hash_destroy(self.ran_sock_hash);
    d_assert(self.ran_addr_hash, , "Null param");
    hash_destroy(self.ran_addr_hash);
    d_assert(self.ran_id_hash, , "Null param");
    hash_destroy(self.ran_id_hash);

    d_assert(self.amf_ue_ngap_id_hash, , "Null param");
    hash_destroy(self.amf_ue_ngap_id_hash);

    d_assert(self.enb_sock_hash, , "Null param");
    hash_destroy(self.enb_sock_hash);
    d_assert(self.enb_addr_hash, , "Null param");
    hash_destroy(self.enb_addr_hash);
    d_assert(self.enb_id_hash, , "Null param");
    hash_destroy(self.enb_id_hash);

    d_assert(self.mme_ue_s1ap_id_hash, , "Null param");
    hash_destroy(self.mme_ue_s1ap_id_hash);
    d_assert(self.imsi_ue_hash, , "Null param");
    hash_destroy(self.imsi_ue_hash);
    d_assert(self.guti_ue_hash, , "Null param");
    hash_destroy(self.guti_ue_hash);

    pool_final(&self.m_tmsi);
    index_final(&amf4g_bearer_pool);
    index_final(&amf4g_sess_pool);
    index_final(&amf4g_ue_pool);
    index_final(&enb_ue_pool);

    index_final(&amf4g_enb_pool);

    index_final(&amf_ue_pool);
    index_final(&ran_ue_pool);

    index_final(&amf_ran_pool);

    sock_remove_all_nodes(&self.s1ap_list);
    sock_remove_all_nodes(&self.s1ap_list6);

    sock_remove_all_nodes(&self.ngap_list);
    sock_remove_all_nodes(&self.ngap_list6);


    tm_delete(self.overloading_checking_timer);

    context_initialized = 0;

    return CORE_OK;
}

amf_context_t* amf4g_self()
{
    return &self;
}

static status_t amf4g_context_prepare()
{
    self.relative_capacity = 0xff;

    self.s1ap_port = S1AP_SCTP_PORT;
    self.ngap_port = NGAP_SCTP_PORT;
    self.fd_config->cnf_port = DIAMETER_PORT;
    self.fd_config->cnf_port_tls = DIAMETER_SECURE_PORT;

    return CORE_OK;
}

static status_t amf4g_context_validation()
{
    if (self.fd_conf_path == NULL &&
        (self.fd_config->cnf_diamid == NULL ||
        self.fd_config->cnf_diamrlm == NULL ||
        self.fd_config->cnf_addr == NULL))
    {
        d_error("No amf4g.freeDiameter in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (list_first(&self.s1ap_list) == NULL &&
        list_first(&self.s1ap_list6) == NULL)
    {
        d_error("No amf4g.s1ap in '%s'", context_self()->config.path);
        return CORE_EAGAIN;
    }

    if (self.max_num_of_served_gummei == 0)
    {
        d_error("No amf4g.gummei in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (self.served_gummei[0].num_of_plmn_id == 0)
    {
        d_error("No amf4g.gummei.plmn_id in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (self.served_gummei[0].num_of_amf4g_gid == 0)
    {
        d_error("No amf4g.gummei.amf4g_gid in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (self.served_gummei[0].num_of_amf4g_code == 0)
    {
        d_error("No amf4g.gummei.amf4g_code in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (self.num_of_served_tai == 0)
    {
        d_error("No amf4g.tai in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (self.served_tai[0].list0.tai[0].num == 0 &&
        self.served_tai[0].list2.num == 0)
    {
        d_error("No amf4g.tai.plmn_id|tac in '%s'", context_self()->config.path);
        return CORE_ERROR;
    }

    if (self.num_of_integrity_order == 0)
    {
        d_error("No amf4g.security.integrity_order in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }
    if (self.num_of_ciphering_order == 0)
    {
        d_error("no amf4g.security.ciphering_order in '%s'",
                context_self()->config.path);
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t amf4g_context_parse_config()
{
    status_t rv;
    config_t *config = &context_self()->config;
    yaml_document_t *document = NULL;
    yaml_iter_t root_iter;

    d_assert(config, return CORE_ERROR,);
    document = config->document;
    d_assert(document, return CORE_ERROR,);

    rv = amf4g_context_prepare();
    if (rv != CORE_OK) return rv;

    yaml_iter_init(&root_iter, document);
    while(yaml_iter_next(&root_iter))
    {
        const char *root_key = yaml_iter_key(&root_iter);
        d_assert(root_key, return CORE_ERROR,);
        if (!strcmp(root_key, "amf"))
        {
            yaml_iter_t amf4g_iter;
            yaml_iter_recurse(&root_iter, &amf4g_iter);
            while(yaml_iter_next(&amf4g_iter))
            {
                const char *amf4g_key = yaml_iter_key(&amf4g_iter);
                d_assert(amf4g_key, return CORE_ERROR,);
                if (!strcmp(amf4g_key, "freeDiameter"))
                {
                    yaml_node_t *node = 
                        yaml_document_get_node(document, amf4g_iter.pair->value);
                    d_assert(node, return CORE_ERROR,);
                    if (node->type == YAML_SCALAR_NODE)
                    {
                        self.fd_conf_path = yaml_iter_value(&amf4g_iter);
                    }
                    else if (node->type == YAML_MAPPING_NODE)
                    {
                        yaml_iter_t fd_iter;
                        yaml_iter_recurse(&amf4g_iter, &fd_iter);

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
                else if (!strcmp(amf4g_key, "relative_capacity"))
                {
                    const char *v = yaml_iter_value(&amf4g_iter);
                    if (v) self.relative_capacity = atoi(v);
                }
                else if (!strcmp(amf4g_key, "s1ap"))
                {
                    yaml_iter_t s1ap_array, s1ap_iter;
                    yaml_iter_recurse(&amf4g_iter, &s1ap_array);
                    do
                    {
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.s1ap_port;
                        const char *dev = NULL;
                        c_sockaddr_t *list = NULL;
                        sock_node_t *node = NULL;

                        if (yaml_iter_type(&s1ap_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&s1ap_iter, &s1ap_array,
                                    sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&s1ap_array) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&s1ap_array))
                                break;
                            yaml_iter_recurse(&s1ap_array, &s1ap_iter);
                        }
                        else if (yaml_iter_type(&s1ap_array) ==
                            YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&s1ap_iter))
                        {
                            const char *s1ap_key =
                                yaml_iter_key(&s1ap_iter);
                            d_assert(s1ap_key,
                                    return CORE_ERROR,);
                            if (!strcmp(s1ap_key, "family"))
                            {
                                const char *v = yaml_iter_value(&s1ap_iter);
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
                            else if (!strcmp(s1ap_key, "addr") ||
                                    !strcmp(s1ap_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&s1ap_iter, &hostname_iter);
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
                            else if (!strcmp(s1ap_key, "port"))
                            {
                                const char *v = yaml_iter_value(&s1ap_iter);
                                if (v)
                                {
                                    port = atoi(v);
                                    self.s1ap_port = port;
                                }
                            }
                            else if (!strcmp(s1ap_key, "dev"))
                            {
                                dev = yaml_iter_value(&s1ap_iter);
                            }
                            else
                                d_warn("unknown key `%s`", s1ap_key);
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
                                rv = sock_add_node(&self.s1ap_list,
                                        &node, list, AF_INET);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            if (context_self()->parameter.no_ipv6 == 0)
                            {
                                rv = sock_add_node(&self.s1ap_list6,
                                        &node, list, AF_INET6);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            core_freeaddrinfo(list);
                        }

                        if (dev)
                        {
                            rv = sock_probe_node(
                                    context_self()->parameter.no_ipv4 ?
                                        NULL : &self.s1ap_list,
                                    context_self()->parameter.no_ipv6 ?
                                        NULL : &self.s1ap_list6,
                                    dev, self.s1ap_port);
                            d_assert(rv == CORE_OK, return CORE_ERROR,);
                        }

                    } while(yaml_iter_type(&s1ap_array) == YAML_SEQUENCE_NODE);

                    if (list_first(&self.s1ap_list) == NULL &&
                        list_first(&self.s1ap_list6) == NULL)
                    {
                        rv = sock_probe_node(
                                context_self()->parameter.no_ipv4 ?
                                    NULL : &self.s1ap_list,
                                context_self()->parameter.no_ipv6 ?
                                    NULL : &self.s1ap_list6,
                                NULL, self.s1ap_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                    }
                    #if 1
                    if (list_first(&self.ngap_list) == NULL &&
                        list_first(&self.ngap_list6) == NULL)
                    {
                        rv = sock_probe_node(
                                context_self()->parameter.no_ipv4 ?
                                    NULL : &self.ngap_list,
                                context_self()->parameter.no_ipv6 ?
                                    NULL : &self.ngap_list6,
                                NULL, self.ngap_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                    }
                    #endif
                }
                #if 0
                else if (!strcmp(amf4g_key, "ngap"))
                {
                    yaml_iter_t ngap_array, ngap_iter;
                    yaml_iter_recurse(&amf4g_iter, &ngap_array);
                    do
                    {
                        printf("ngap_yaml exist\n");
                        int family = AF_UNSPEC;
                        int i, num = 0;
                        const char *hostname[MAX_NUM_OF_HOSTNAME];
                        c_uint16_t port = self.ngap_port;
                        const char *dev = NULL;
                        c_sockaddr_t *list = NULL;
                        sock_node_t *node = NULL;

                        if (yaml_iter_type(&ngap_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&ngap_iter, &ngap_array,
                                    sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&ngap_array) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&ngap_array))
                                break;
                            yaml_iter_recurse(&ngap_array, &ngap_iter);
                        }
                        else if (yaml_iter_type(&ngap_array) ==
                            YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&ngap_iter))
                        {
                            const char *ngap_key =
                                yaml_iter_key(&ngap_iter);
                            d_assert(ngap_key,
                                    return CORE_ERROR,);
                            if (!strcmp(ngap_key, "family"))
                            {
                                const char *v = yaml_iter_value(&s1ap_iter);
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
                            else if (!strcmp(s1ap_key, "addr") ||
                                    !strcmp(s1ap_key, "name"))
                            {
                                yaml_iter_t hostname_iter;
                                yaml_iter_recurse(&s1ap_iter, &hostname_iter);
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
                            else if (!strcmp(s1ap_key, "port"))
                            {
                                const char *v = yaml_iter_value(&s1ap_iter);
                                if (v)
                                {
                                    port = atoi(v);
                                    self.s1ap_port = port;
                                }
                            }
                            else if (!strcmp(s1ap_key, "dev"))
                            {
                                dev = yaml_iter_value(&s1ap_iter);
                            }
                            else
                                d_warn("unknown key `%s`", s1ap_key);
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
                                rv = sock_add_node(&self.s1ap_list,
                                        &node, list, AF_INET);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            if (context_self()->parameter.no_ipv6 == 0)
                            {
                                rv = sock_add_node(&self.s1ap_list6,
                                        &node, list, AF_INET6);
                                d_assert(rv == CORE_OK, return CORE_ERROR,);
                            }

                            core_freeaddrinfo(list);
                        }

                        if (dev)
                        {
                            rv = sock_probe_node(
                                    context_self()->parameter.no_ipv4 ?
                                        NULL : &self.s1ap_list,
                                    context_self()->parameter.no_ipv6 ?
                                        NULL : &self.s1ap_list6,
                                    dev, self.s1ap_port);
                            d_assert(rv == CORE_OK, return CORE_ERROR,);
                        }

                    } while(yaml_iter_type(&s1ap_array) == YAML_SEQUENCE_NODE);

                    if (list_first(&self.s1ap_list) == NULL &&
                        list_first(&self.s1ap_list6) == NULL)
                    {
                        rv = sock_probe_node(
                                context_self()->parameter.no_ipv4 ?
                                    NULL : &self.s1ap_list,
                                context_self()->parameter.no_ipv6 ?
                                    NULL : &self.s1ap_list6,
                                NULL, self.s1ap_port);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                    }
                }
                #endif
                else if (!strcmp(amf4g_key, "gummei"))
                {
                    yaml_iter_t gummei_array, gummei_iter;
                    yaml_iter_recurse(&amf4g_iter, &gummei_array);
                    do
                    {
                        served_gummei_t *gummei = NULL;
                        d_assert(self.max_num_of_served_gummei <=
                                MAX_NUM_OF_SERVED_GUMMEI, return CORE_ERROR,);
                        gummei = &self.served_gummei[
                            self.max_num_of_served_gummei];
                        d_assert(gummei, return CORE_ERROR,);

                        if (yaml_iter_type(&gummei_array) ==
                                YAML_MAPPING_NODE)
                        {
                            memcpy(&gummei_iter, &gummei_array,
                                    sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&gummei_array) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&gummei_array))
                                break;
                            yaml_iter_recurse(&gummei_array,
                                    &gummei_iter);
                        }
                        else if (yaml_iter_type(&gummei_array) ==
                            YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&gummei_iter))
                        {
                            const char *gummei_key =
                                yaml_iter_key(&gummei_iter);
                            d_assert(gummei_key,
                                    return CORE_ERROR,);
                            if (!strcmp(gummei_key, "plmn_id"))
                            {
                                yaml_iter_t plmn_id_array, plmn_id_iter;
                                yaml_iter_recurse(&gummei_iter, &plmn_id_array);
                                do
                                {
                                    plmn_id_t *plmn_id = NULL;
                                    const char *mcc = NULL, *mnc = NULL;
                                    d_assert(gummei->num_of_plmn_id <=
                                            MAX_PLMN_ID, return CORE_ERROR,);
                                    plmn_id = &gummei->plmn_id[
                                        gummei->num_of_plmn_id];
                                    d_assert(plmn_id, return CORE_ERROR,);

                                    if (yaml_iter_type(&plmn_id_array) ==
                                            YAML_MAPPING_NODE)
                                    {
                                        memcpy(&plmn_id_iter, &plmn_id_array,
                                                sizeof(yaml_iter_t));
                                    }
                                    else if (yaml_iter_type(&plmn_id_array) ==
                                        YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&plmn_id_array))
                                            break;
                                        yaml_iter_recurse(&plmn_id_array,
                                                &plmn_id_iter);
                                    }
                                    else if (yaml_iter_type(&plmn_id_array) ==
                                        YAML_SCALAR_NODE)
                                    {
                                        break;
                                    }
                                    else
                                        d_assert(0, return CORE_ERROR,);

                                    while(yaml_iter_next(&plmn_id_iter))
                                    {
                                        const char *plmn_id_key =
                                            yaml_iter_key(&plmn_id_iter);
                                        d_assert(plmn_id_key,
                                                return CORE_ERROR,);
                                        if (!strcmp(plmn_id_key, "mcc"))
                                        {
                                            mcc =
                                                yaml_iter_value(&plmn_id_iter);
                                        }
                                        else if (!strcmp(plmn_id_key, "mnc"))
                                        {
                                            mnc =
                                                yaml_iter_value(&plmn_id_iter);
                                        }
                                    }

                                    if (mcc && mnc)
                                    {
                                        plmn_id_build(plmn_id,
                                            atoi(mcc), atoi(mnc), strlen(mnc));
                                        gummei->num_of_plmn_id++;
                                    }

                                } while(yaml_iter_type(&plmn_id_array) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else if (!strcmp(gummei_key, "mme_gid"))
                            {
                                yaml_iter_t amf4g_gid_iter;
                                yaml_iter_recurse(&gummei_iter, &amf4g_gid_iter);
                                d_assert(yaml_iter_type(&amf4g_gid_iter) !=
                                    YAML_MAPPING_NODE, return CORE_ERROR,);

                                do
                                {
                                    c_uint16_t *amf4g_gid = NULL;
                                    const char *v = NULL;

                                    d_assert(gummei->num_of_amf4g_gid <=
                                            GRP_PER_MME, return CORE_ERROR,);
                                    amf4g_gid = &gummei->amf4g_gid[
                                        gummei->num_of_amf4g_gid];
                                    d_assert(amf4g_gid, return CORE_ERROR,);

                                    if (yaml_iter_type(&amf4g_gid_iter) ==
                                            YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&amf4g_gid_iter))
                                            break;
                                    }

                                    v = yaml_iter_value(&amf4g_gid_iter);
                                    if (v) 
                                    {
                                        *amf4g_gid = atoi(v);
                                        gummei->num_of_amf4g_gid++;
                                    }
                                } while(
                                    yaml_iter_type(&amf4g_gid_iter) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else if (!strcmp(gummei_key, "mme_code"))
                            {
                                yaml_iter_t amf4g_code_iter;
                                yaml_iter_recurse(&gummei_iter, &amf4g_code_iter);
                                d_assert(yaml_iter_type(&amf4g_code_iter) !=
                                    YAML_MAPPING_NODE, return CORE_ERROR,);

                                do
                                {
                                    c_uint8_t *amf4g_code = NULL;
                                    const char *v = NULL;

                                    d_assert(gummei->num_of_amf4g_code <=
                                            CODE_PER_MME, return CORE_ERROR,);
                                    amf4g_code = &gummei->amf4g_code[
                                        gummei->num_of_amf4g_code];
                                    d_assert(amf4g_code, return CORE_ERROR,);

                                    if (yaml_iter_type(&amf4g_code_iter) ==
                                            YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&amf4g_code_iter))
                                            break;
                                    }

                                    v = yaml_iter_value(&amf4g_code_iter);
                                    if (v) 
                                    {
                                        *amf4g_code = atoi(v);
                                        gummei->num_of_amf4g_code++;
                                    }
                                } while(
                                    yaml_iter_type(&amf4g_code_iter) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else
                                d_warn("unknown key `%s`", gummei_key);
                        }

                        if (gummei->num_of_plmn_id &&
                            gummei->num_of_amf4g_gid && gummei->num_of_amf4g_code)
                        {
                            self.max_num_of_served_gummei++;
                            self.max_num_of_served_guami++;
                        }
                        else
                        {
                            d_warn("Ignore gummei : "
                                    "plmn_id(%d), amf4g_gid(%d), amf4g_code(%d)",
                                gummei->num_of_plmn_id,
                                gummei->num_of_amf4g_gid, gummei->num_of_amf4g_code);
                            gummei->num_of_plmn_id = 0;
                            gummei->num_of_amf4g_gid = 0;
                            gummei->num_of_amf4g_code = 0;
                        }
                    } while(yaml_iter_type(&gummei_array) ==
                            YAML_SEQUENCE_NODE);
                }
                else if (!strcmp(amf4g_key, "tai"))
                {
                    int num_of_list0 = 0;
                    tai0_list_t *list0 = NULL;
                    tai2_list_t *list2 = NULL;

                    d_assert(self.num_of_served_tai <=
                            MAX_NUM_OF_SERVED_TAI, return CORE_ERROR,);
                    list0 = &self.served_tai[self.num_of_served_tai].list0;
                    d_assert(list0, return CORE_ERROR,);
                    list2 = &self.served_tai[self.num_of_served_tai].list2;
                    d_assert(list2, return CORE_ERROR,);

                    yaml_iter_t tai_array, tai_iter;
                    yaml_iter_recurse(&amf4g_iter, &tai_array);
                    do
                    {
                        const char *mcc = NULL, *mnc = NULL;
                        c_uint16_t tac[MAX_NUM_OF_TAI];
                        int num_of_tac = 0;

                        if (yaml_iter_type(&tai_array) == YAML_MAPPING_NODE)
                        {
                            memcpy(&tai_iter, &tai_array, sizeof(yaml_iter_t));
                        }
                        else if (yaml_iter_type(&tai_array) ==
                            YAML_SEQUENCE_NODE)
                        {
                            if (!yaml_iter_next(&tai_array))
                                break;
                            yaml_iter_recurse(&tai_array,
                                    &tai_iter);
                        }
                        else if (yaml_iter_type(&tai_array) == YAML_SCALAR_NODE)
                        {
                            break;
                        }
                        else
                            d_assert(0, return CORE_ERROR,);

                        while(yaml_iter_next(&tai_iter))
                        {
                            const char *tai_key = yaml_iter_key(&tai_iter);
                            d_assert(tai_key, return CORE_ERROR,);
                            if (!strcmp(tai_key, "plmn_id"))
                            {
                                yaml_iter_t plmn_id_iter;

                                yaml_iter_recurse(&tai_iter, &plmn_id_iter);
                                while(yaml_iter_next(&plmn_id_iter))
                                {
                                    const char *plmn_id_key =
                                        yaml_iter_key(&plmn_id_iter);
                                    d_assert(plmn_id_key,
                                            return CORE_ERROR,);
                                    if (!strcmp(plmn_id_key, "mcc"))
                                    {
                                        mcc = yaml_iter_value(&plmn_id_iter);
                                    }
                                    else if (!strcmp(plmn_id_key, "mnc"))
                                    {
                                        mnc = yaml_iter_value(&plmn_id_iter);
                                    }
                                }
                            }
                            else if (!strcmp(tai_key, "tac"))
                            {
                                yaml_iter_t tac_iter;
                                yaml_iter_recurse(&tai_iter, &tac_iter);
                                d_assert(yaml_iter_type(&tac_iter) !=
                                    YAML_MAPPING_NODE, return CORE_ERROR,);

                                do
                                {
                                    const char *v = NULL;

                                    d_assert(num_of_tac <=
                                            MAX_NUM_OF_TAI, return CORE_ERROR,);
                                    if (yaml_iter_type(&tac_iter) ==
                                            YAML_SEQUENCE_NODE)
                                    {
                                        if (!yaml_iter_next(&tac_iter))
                                            break;
                                    }

                                    v = yaml_iter_value(&tac_iter);
                                    if (v) 
                                    {
                                        tac[num_of_tac] = atoi(v);
                                        num_of_tac++;
                                    }
                                } while(
                                    yaml_iter_type(&tac_iter) ==
                                        YAML_SEQUENCE_NODE);
                            }
                            else
                                d_warn("unknown key `%s`", tai_key);
                        }

                        if (mcc && mnc && num_of_tac)
                        {
                            if (num_of_tac == 1)
                            {
                                plmn_id_build(
                                    &list2->tai[list2->num].plmn_id,
                                    atoi(mcc), atoi(mnc), strlen(mnc));
                                list2->tai[list2->num].tac = tac[0];

                                list2->num++;
                                if (list2->num > 1)
                                    list2->type = TAI2_TYPE;
                                else
                                    list2->type = TAI1_TYPE;
                            }
                            else if (num_of_tac > 1)
                            {
                                int i;
                                plmn_id_build(
                                    &list0->tai[num_of_list0].plmn_id,
                                    atoi(mcc), atoi(mnc), strlen(mnc));
                                for (i = 0; i < num_of_tac; i++)
                                {
                                    list0->tai[num_of_list0].tac[i] = tac[i];
                                }

                                list0->tai[num_of_list0].num = num_of_tac;
                                list0->tai[num_of_list0].type = TAI0_TYPE;

                                num_of_list0++;
                            }
                        }
                        else
                        {
                            d_warn("Ignore tai : mcc(%p), mnc(%p), "
                                    "num_of_tac(%d)", mcc, mnc, num_of_tac);
                        }
                    } while(yaml_iter_type(&tai_array) ==
                            YAML_SEQUENCE_NODE);

                    if (list2->num || num_of_list0)
                    {
                        self.num_of_served_tai++;
                    }
                }
                else if (!strcmp(amf4g_key, "security"))
                {
                    yaml_iter_t security_iter;
                    yaml_iter_recurse(&amf4g_iter, &security_iter);
                    while(yaml_iter_next(&security_iter))
                    {
                        const char *security_key =
                            yaml_iter_key(&security_iter);
                        d_assert(security_key, return CORE_ERROR,);
                        if (!strcmp(security_key, "integrity_order"))
                        {
                            yaml_iter_t integrity_order_iter;
                            yaml_iter_recurse(&security_iter,
                                    &integrity_order_iter);
                            d_assert(yaml_iter_type(&integrity_order_iter) !=
                                YAML_MAPPING_NODE, return CORE_ERROR,);

                            do
                            {
                                const char *v = NULL;

                                if (yaml_iter_type(&integrity_order_iter) ==
                                        YAML_SEQUENCE_NODE)
                                {
                                    if (!yaml_iter_next(&integrity_order_iter))
                                        break;
                                }

                                v = yaml_iter_value(&integrity_order_iter);
                                if (v) 
                                {
                                    int integrity_index = 
                                        self.num_of_integrity_order;
                                    if (strcmp(v, "EIA0") == 0)
                                    {
                                        self.integrity_order[integrity_index] = 
                                            NAS_SECURITY_ALGORITHMS_EIA0;
                                        self.num_of_integrity_order++;
                                    }
                                    else if (strcmp(v, "EIA1") == 0)
                                    {
                                        self.integrity_order[integrity_index] = 
                                            NAS_SECURITY_ALGORITHMS_128_EIA1;
                                        self.num_of_integrity_order++;
                                    }
                                    else if (strcmp(v, "EIA2") == 0)
                                    {
                                        self.integrity_order[integrity_index] = 
                                            NAS_SECURITY_ALGORITHMS_128_EIA2;
                                        self.num_of_integrity_order++;
                                    }
                                    else if (strcmp(v, "EIA3") == 0)
                                    {
                                        self.integrity_order[integrity_index] = 
                                            NAS_SECURITY_ALGORITHMS_128_EIA3;
                                        self.num_of_integrity_order++;
                                    }
                                }
                            } while(
                                yaml_iter_type(&integrity_order_iter) ==
                                    YAML_SEQUENCE_NODE);
                        }
                        else if (!strcmp(security_key, "ciphering_order"))
                        {
                            yaml_iter_t ciphering_order_iter;
                            yaml_iter_recurse(&security_iter,
                                    &ciphering_order_iter);
                            d_assert(yaml_iter_type(&ciphering_order_iter) !=
                                YAML_MAPPING_NODE, return CORE_ERROR,);

                            do
                            {
                                const char *v = NULL;

                                if (yaml_iter_type(&ciphering_order_iter) ==
                                        YAML_SEQUENCE_NODE)
                                {
                                    if (!yaml_iter_next(&ciphering_order_iter))
                                        break;
                                }

                                v = yaml_iter_value(&ciphering_order_iter);
                                if (v) 
                                {
                                    int ciphering_index = 
                                        self.num_of_ciphering_order;
                                    if (strcmp(v, "EEA0") == 0)
                                    {
                                        self.ciphering_order[ciphering_index] = 
                                            NAS_SECURITY_ALGORITHMS_EEA0;
                                        self.num_of_ciphering_order++;
                                    }
                                    else if (strcmp(v, "EEA1") == 0)
                                    {
                                        self.ciphering_order[ciphering_index] = 
                                            NAS_SECURITY_ALGORITHMS_128_EEA1;
                                        self.num_of_ciphering_order++;
                                    }
                                    else if (strcmp(v, "EEA2") == 0)
                                    {
                                        self.ciphering_order[ciphering_index] = 
                                            NAS_SECURITY_ALGORITHMS_128_EEA2;
                                        self.num_of_ciphering_order++;
                                    }
                                    else if (strcmp(v, "EEA3") == 0)
                                    {
                                        self.ciphering_order[ciphering_index] = 
                                            NAS_SECURITY_ALGORITHMS_128_EEA3;
                                        self.num_of_ciphering_order++;
                                    }
                                }
                            } while(
                                yaml_iter_type(&ciphering_order_iter) ==
                                    YAML_SEQUENCE_NODE);
                        }
                    }
                }
                else if(!strcmp(amf4g_key, "network_name"))
                {
                    yaml_iter_t network_name_iter;
                    yaml_iter_recurse(&amf4g_iter, &network_name_iter);

                    while(yaml_iter_next(&network_name_iter))
                    {
                        const char *network_name_key =
                        yaml_iter_key(&network_name_iter);
                        d_assert(network_name_key,
                                    return CORE_ERROR,);
                        if (!strcmp(network_name_key, "full"))
                        {  
                            nas_network_name_t *network_full_name =
                                &self.full_name;
                            const char *c_network_name =
                                yaml_iter_value(&network_name_iter);
                            c_uint8_t size = strlen(c_network_name);
                            c_uint8_t i;
                            for(i = 0;i<size;i++)
                            {
                                /* Workaround to convert the ASCII to USC-2 */
                                network_full_name->name[i*2] = 0;
                                network_full_name->name[(i*2)+1] =
                                    c_network_name[i];

                            }
                                network_full_name->length = size*2+1;
                                network_full_name->coding_scheme = 1;
                        }
                        else if (!strcmp(network_name_key, "short"))
                        {
                            nas_network_name_t *network_short_name =
                                &self.short_name;
                            const char *c_network_name =
                                yaml_iter_value(&network_name_iter);
                            c_uint8_t size = strlen(c_network_name);
                            c_uint8_t i;
                            for(i = 0;i<size;i++)
                            {
                                /* Workaround to convert the ASCII to USC-2 */
                                network_short_name->name[i*2] = 0;
                                network_short_name->name[(i*2)+1] =
                                    c_network_name[i];

                            }
                            network_short_name->length = size*2+1;
                            network_short_name->coding_scheme = 1;
                        }
                    }
                }
                else
                    d_warn("unknown key `%s`", amf4g_key);
            }
        }
        else if (!strcmp(root_key, "smf"))
        {
            yaml_iter_t smf_iter;
            yaml_iter_recurse(&root_iter, &smf_iter);
            while(yaml_iter_next(&smf_iter))
            {
                const char *smf_key = yaml_iter_key(&smf_iter);
                d_assert(smf_key, return CORE_ERROR,);
                if (!strcmp(smf_key, "http"))
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

    rv = amf4g_context_validation();
    if (rv != CORE_OK) return rv;

    return CORE_OK;
}

status_t amf4g_context_setup_trace_module()
{
    int app = context_self()->logger.trace.app;
    int s1ap = context_self()->logger.trace.s1ap;
    int nas = context_self()->logger.trace.nas;
    int diameter = context_self()->logger.trace.diameter;
    int ngap = context_self()->logger.trace.ngap;
    int sbi = context_self()->logger.trace.sbi;


    if (app)
    {
        extern int _amf4g_context;
        d_trace_level(&_amf4g_context, app);
        extern int _amf4g_sm;
        d_trace_level(&_amf4g_sm, app);
    }

    if (s1ap)
    {
        extern int _s1ap_sm;
        d_trace_level(&_s1ap_sm, s1ap);
        extern int _s1ap_build;
        d_trace_level(&_s1ap_build, s1ap);
        extern int _s1ap_conv;
        d_trace_level(&_s1ap_conv, s1ap);
        extern int _s1ap_handler;
        d_trace_level(&_s1ap_handler, s1ap);
        extern int _s1ap_sctp;
        d_trace_level(&_s1ap_sctp, s1ap);
        extern int _s1ap_path;
        d_trace_level(&_s1ap_path, s1ap);
        extern int _s1ap_decoder;
        d_trace_level(&_s1ap_decoder, s1ap);
        extern int _s1ap_encoder;
        d_trace_level(&_s1ap_encoder, s1ap);
    }

    if (nas)
    {
        extern int _emm_sm;
        d_trace_level(&_emm_sm, nas);
        extern int _esm_sm;
        d_trace_level(&_esm_sm, nas);
        extern int _emm_build;
        d_trace_level(&_emm_build, nas);
        extern int _esm_build;
        d_trace_level(&_esm_build, nas);
        extern int _emm_handler;
        d_trace_level(&_emm_handler, nas);
        extern int _esm_handler;
        d_trace_level(&_esm_handler, nas);
        extern int _nas_path;
        d_trace_level(&_nas_path, nas);
        extern int _nas_decoder;
        d_trace_level(&_nas_decoder, nas);
        extern int _nas_encoder;
        d_trace_level(&_nas_encoder, nas);
        extern int _nas_ies;
        d_trace_level(&_nas_ies, nas);
    }

    if (diameter)
    {
        extern int _amf4g_fd_path;
        d_trace_level(&_amf4g_fd_path, diameter);
        extern int _fd_init;
        d_trace_level(&_fd_init, diameter);
        extern int _fd_logger;
        d_trace_level(&_fd_logger, diameter);
    }

    if (ngap)
    {
        extern int _ngap_sctp;
        extern int _ngap_handler;
        extern int _ngap_build;
        d_trace_level(&_ngap_sctp, ngap);
        d_trace_level(&_ngap_handler, ngap);
        d_trace_level(&_ngap_build, ngap);
        
    }

    if (sbi) 
    {
        extern int _amf_json_build;
        extern int _amf_json_handler;
        extern int _amf_sbi_path;
        d_trace_level(&_amf_json_build, sbi);
        d_trace_level(&_amf_json_handler, sbi);
        d_trace_level(&_amf_sbi_path, sbi);
    }

    return CORE_OK;
}

amf4g_enb_t* amf4g_enb_add(sock_id sock, c_sockaddr_t *addr)
{
    amf4g_enb_t *enb = NULL;
    event_t e;

    d_assert(sock, return NULL,);
    d_assert(addr, return NULL,);

    index_alloc(&amf4g_enb_pool, &enb);
    d_assert(enb, return NULL, "Null param");

    enb->sock = sock;
    enb->addr = addr;
    enb->sock_type = amf4g_enb_sock_type(enb->sock);

    enb->outbound_streams = context_self()->parameter.sctp_streams;

    list_init(&enb->enb_ue_list);

    hash_set(self.enb_sock_hash, &enb->sock, sizeof(enb->sock), enb);
    hash_set(self.enb_addr_hash, enb->addr, sizeof(c_sockaddr_t), enb);
    
    event_set_param1(&e, (c_uintptr_t)enb->index);
    fsm_create(&enb->sm, s1ap_state_initial, s1ap_state_final);
    fsm_init(&enb->sm, &e);

    return enb;
}

status_t amf4g_enb_remove(amf4g_enb_t *enb)
{
    event_t e;

    d_assert(enb, return CORE_ERROR, "Null param");
    d_assert(enb->sock, return CORE_ERROR, "Null param");

    event_set_param1(&e, (c_uintptr_t)enb->index);
    fsm_final(&enb->sm, &e);
    fsm_clear(&enb->sm);

    hash_set(self.enb_sock_hash, &enb->sock, sizeof(enb->sock), NULL);
    hash_set(self.enb_addr_hash, enb->addr, sizeof(c_sockaddr_t), NULL);
    if (enb->enb_id)
        hash_set(self.enb_id_hash, &enb->enb_id, sizeof(enb->enb_id), NULL);

    enb_ue_remove_in_enb(enb);

    CORE_FREE(enb->addr);

    index_free(&amf4g_enb_pool, enb);

    return CORE_OK;
}

status_t amf4g_enb_remove_all()
{
    hash_index_t *hi = NULL;
    amf4g_enb_t *enb = NULL;

    for (hi = amf4g_enb_first(); hi; hi = amf4g_enb_next(hi))
    {
        enb = amf4g_enb_this(hi);

        if (enb->sock_type == SOCK_STREAM)
            s1ap_delete(enb->sock);

        amf4g_enb_remove(enb);
    }

    return CORE_OK;
}

amf4g_enb_t* amf4g_enb_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&amf4g_enb_pool, index);
}

amf4g_enb_t* amf4g_enb_find_by_sock(sock_id sock)
{
    d_assert(sock, return NULL,"Invalid param");
    return (amf4g_enb_t *)hash_get(self.enb_sock_hash, &sock, sizeof(sock));

    return NULL;
}

amf4g_enb_t* amf4g_enb_find_by_addr(c_sockaddr_t *addr)
{
    d_assert(addr, return NULL,"Invalid param");
    return (amf4g_enb_t *)hash_get(self.enb_addr_hash,
            addr, sizeof(c_sockaddr_t));

    return NULL;
}

amf4g_enb_t* amf4g_enb_find_by_enb_id(c_uint32_t enb_id)
{
    d_assert(enb_id, return NULL,"Invalid param");
    return (amf4g_enb_t *)hash_get(self.enb_id_hash, &enb_id, sizeof(enb_id));
}

status_t amf4g_enb_set_enb_id(amf4g_enb_t *enb, c_uint32_t enb_id)
{
    d_assert(enb, return CORE_ERROR, "Invalid param");
    d_assert(enb_id, return CORE_ERROR, "Invalid param");

    enb->enb_id = enb_id;
    hash_set(self.enb_id_hash, &enb->enb_id, sizeof(enb->enb_id), enb);

    return CORE_OK;
}

hash_index_t* amf4g_enb_first()
{
    d_assert(self.enb_sock_hash, return NULL, "Null param");
    return hash_first(self.enb_sock_hash);
}

hash_index_t* amf4g_enb_next(hash_index_t *hi)
{
    return hash_next(hi);
}

amf4g_enb_t *amf4g_enb_this(hash_index_t *hi)
{
    d_assert(hi, return NULL, "Null param");
    return hash_this_val(hi);
}

int amf4g_enb_sock_type(sock_id sock)
{
    sock_node_t *snode = NULL;

    d_assert(sock, return SOCK_STREAM,);

    for (snode = list_first(&amf4g_self()->s1ap_list);
            snode; snode = list_next(snode))
    {
        if (snode->sock == sock) return SOCK_SEQPACKET;
    }
    for (snode = list_first(&amf4g_self()->s1ap_list6);
            snode; snode = list_next(snode))
    {
        if (snode->sock == sock) return SOCK_SEQPACKET;
    }

    return SOCK_STREAM;
}

/** amf_ran handling function **/

amf_ran_t* amf_ran_add(sock_id sock, c_sockaddr_t *addr)
{
    amf_ran_t *ran = NULL;
    event_t e;

    d_assert(sock, return NULL,);
    d_assert(addr, return NULL,);

    index_alloc(&amf_ran_pool, &ran);
    d_assert(ran, return NULL, "Null param");

    ran->sock = sock;
    ran->addr = addr;
    ran->sock_type = amf_ran_sock_type(ran->sock);

    ran->outbound_streams = context_self()->parameter.sctp_streams;

    list_init(&ran->ran_ue_list);

    hash_set(self.ran_sock_hash, &ran->sock, sizeof(ran->sock), ran);
    hash_set(self.ran_addr_hash, ran->addr, sizeof(c_sockaddr_t), ran);
    
    event_set_param1(&e, (c_uintptr_t)ran->index);
    fsm_create(&ran->sm, ngap_state_initial, ngap_state_final);
    fsm_init(&ran->sm, &e);

    return ran;
}

status_t amf_ran_remove(amf_ran_t *ran)
{
    event_t e;

    d_assert(ran, return CORE_ERROR, "Null param");
    d_assert(ran->sock, return CORE_ERROR, "Null param");

    event_set_param1(&e, (c_uintptr_t)ran->index);
    fsm_final(&ran->sm, &e);
    fsm_clear(&ran->sm);

    hash_set(self.ran_sock_hash, &ran->sock, sizeof(ran->sock), NULL);
    hash_set(self.ran_addr_hash, ran->addr, sizeof(c_sockaddr_t), NULL);
    if(ran->ran_id.ran_present)
        hash_set(self.ran_id_hash, &ran->ran_id, sizeof(ran->ran_id), NULL);

        
    ran_ue_remove_in_ran(ran);

    CORE_FREE(ran->addr);

    index_free(&amf_ran_pool, ran);

    return CORE_OK;
}

status_t amf_ran_remove_all()
{
    hash_index_t *hi = NULL;
    amf_ran_t *ran = NULL;

    for (hi = amf_ran_first(); hi; hi = amf_ran_next(hi))
    {
        ran = amf_ran_this(hi);

        if (ran->sock_type == SOCK_STREAM)
            s1ap_delete(ran->sock);

        amf_ran_remove(ran);
    }

    return CORE_OK;
}

amf_ran_t* amf_ran_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&amf_ran_pool, index);
}

amf_ran_t* amf_ran_find_by_sock(sock_id sock)
{
    d_assert(sock, return NULL,"Invalid param");
    return (amf_ran_t *)hash_get(self.ran_sock_hash, &sock, sizeof(sock));

    return NULL;
}

amf_ran_t* amf_ran_find_by_addr(c_sockaddr_t *addr)
{
    d_assert(addr, return NULL,"Invalid param");
    return (amf_ran_t *)hash_get(self.ran_addr_hash,
            addr, sizeof(c_sockaddr_t));

    return NULL;
}

amf_ran_t* amf_ran_find_by_ran_id(c_uint32_t gnb_id, c_uint32_t ngenb_id, c_uint16_t n3iwf_id, int ran_present)
{
    ran_id_t ran_id;
    switch(ran_present)
    {
        case RAN_PR_GNB_ID:
        {
            d_assert(gnb_id, return NULL, "Invalid param");
            ran_id.gnb_id = gnb_id;
            break;
        }
        case RAN_PR_NgENB_ID:
        {
            d_assert(ngenb_id, return NULL, "Invalid param");
            ran_id.ngenb_id = ngenb_id;
            break;
        }
        case RAN_PR_N3IWF_ID:
        {
            d_assert(n3iwf_id, return NULL, "Invalid param");
            ran_id.n3iwf_id = n3iwf_id;
            break;
        }
        default:
        {
            d_assert(0, return NULL, "Invalid param");
            break;
        }

    }
    return (amf_ran_t *)hash_get(self.ran_id_hash, &ran_id, sizeof(ran_id));
}

status_t amf_ran_set_ran_id(amf_ran_t *ran, c_uint32_t gnb_id, c_uint32_t ngenb_id, c_uint16_t n3iwf_id, int ran_present)
{
    d_assert(ran, return CORE_ERROR, "Invalid param");
    
    ran->ran_id.ran_present = ran_present;
    switch(ran_present)
    {
        case RAN_PR_GNB_ID:
        {
            d_assert(gnb_id, return CORE_ERROR, "Invalid param");
            ran->ran_id.gnb_id = gnb_id;
            break;
        }
        case RAN_PR_NgENB_ID:
        {
            d_assert(ngenb_id, return CORE_ERROR, "Invalid param");
            ran->ran_id.ngenb_id = ngenb_id;
            break;
        }
        case RAN_PR_N3IWF_ID:
        {
            d_assert(n3iwf_id, return CORE_ERROR, "Invalid param");
            ran->ran_id.n3iwf_id = n3iwf_id;
            break;
        }
        default:
        {
            d_assert(0, return CORE_ERROR, "Invalid param");
            break;
        }

    }
    hash_set(self.ran_id_hash, &ran->ran_id, sizeof(ran->ran_id), ran);
    
    return CORE_OK;
}

hash_index_t* amf_ran_first()
{
    d_assert(self.ran_sock_hash, return NULL, "Null param");
    return hash_first(self.ran_sock_hash);
}

hash_index_t* amf_ran_next(hash_index_t *hi)
{
    return hash_next(hi);
}

amf_ran_t *amf_ran_this(hash_index_t *hi)
{
    d_assert(hi, return NULL, "Null param");
    return hash_this_val(hi);
}

int amf_ran_sock_type(sock_id sock)
{
    sock_node_t *snode = NULL;

    d_assert(sock, return SOCK_STREAM,);

    for (snode = list_first(&amf4g_self()->ngap_list);
            snode; snode = list_next(snode))
    {
        if (snode->sock == sock) return SOCK_SEQPACKET;
    }
    for (snode = list_first(&amf4g_self()->ngap_list6);
            snode; snode = list_next(snode))
    {
        if (snode->sock == sock) return SOCK_SEQPACKET;
    }

    return SOCK_STREAM;
}

/** ran_ue_context handling function **/

ran_ue_t* ran_ue_add(amf_ran_t *ran)
{
    ran_ue_t *ran_ue = NULL;

    d_assert(self.amf_ue_ngap_id_hash, return NULL, "Null param");
    d_assert(ran, return NULL, "Null param");

    index_alloc(&ran_ue_pool, &ran_ue);
    d_assert(ran_ue, return NULL, "Null param");

    ran_ue->ran_ue_ngap_id = INVALID_UE_NGAP_ID;
    ran_ue->amf_ue_ngap_id = NEXT_ID(self.amf_ue_ngap_id, 1, 0xffffffff);
    ran_ue->ran = ran;

    hash_set(self.amf_ue_ngap_id_hash, &ran_ue->amf_ue_ngap_id, 
            sizeof(ran_ue->amf_ue_ngap_id), ran_ue);
    list_append(&ran->ran_ue_list, ran_ue);

    /* Create S1 holding timer */
    ran_ue->holding_timer = timer_create(&self.tm_service,
            AMF_EVT_NGAP_NG_HOLDING_TIMER, self.ng_holding_timer_value * 1000);
    d_assert(ran_ue->holding_timer, return NULL, "Null param");
    timer_set_param1(ran_ue->holding_timer, ran_ue->index);

    return ran_ue;

}


unsigned int ran_ue_count()
{
    d_assert(self.amf_ue_ngap_id_hash, return 0, "Null param");
    return hash_count(self.amf_ue_ngap_id_hash);
}

status_t ran_ue_remove(ran_ue_t *ran_ue)
{
    status_t rv;

    d_assert(self.amf_ue_ngap_id_hash, return CORE_ERROR, "Null param");
    d_assert(ran_ue, return CORE_ERROR, "Null param");
    d_assert(ran_ue->ran, return CORE_ERROR, "Null param");

    /* De-associate NG with NAS/GMM */
    rv = ran_ue_deassociate(ran_ue);
    d_assert(rv == CORE_OK,,);

   /* Delete All Timers */
    tm_delete(ran_ue->holding_timer);

    list_remove(&ran_ue->ran->ran_ue_list, ran_ue);
    hash_set(self.amf_ue_ngap_id_hash, &ran_ue->amf_ue_ngap_id, 
            sizeof(ran_ue->amf_ue_ngap_id), NULL);

    index_free(&ran_ue_pool, ran_ue);

    return CORE_OK;
}

status_t ran_ue_remove_in_ran(amf_ran_t *ran)
{
    ran_ue_t *ran_ue = NULL, *next_ran_ue = NULL;
    
    ran_ue = ran_ue_first_in_ran(ran);
    while (ran_ue)
    {
        next_ran_ue = ran_ue_next_in_ran(ran_ue);

        ran_ue_remove(ran_ue);

        ran_ue = next_ran_ue;
    }

    return CORE_OK;
}

status_t ran_ue_switch_to_ran(ran_ue_t *ran_ue, amf_ran_t *new_ran)
{
    d_assert(ran_ue, return CORE_ERROR, "Null param");
    d_assert(ran_ue->ran, return CORE_ERROR, "Null param");
    d_assert(new_ran, return CORE_ERROR, "Null param");

    /* Remove from the old ran */
    list_remove(&ran_ue->ran->ran_ue_list, ran_ue);

    /* Add to the new ran */
    list_append(&new_ran->ran_ue_list, ran_ue);

    /* Switch to ran */
    ran_ue->ran = new_ran;

    return CORE_OK;
}

ran_ue_t* ran_ue_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&ran_ue_pool, index);
}

ran_ue_t* ran_ue_find_by_ran_ue_ngap_id(
        amf_ran_t *ran, c_uint32_t ran_ue_ngap_id)
{
    ran_ue_t *ran_ue = NULL;
    
    ran_ue = ran_ue_first_in_ran(ran);
    while (ran_ue)
    {
        if (ran_ue_ngap_id == ran_ue->ran_ue_ngap_id)
            break;

        ran_ue = ran_ue_next_in_ran(ran_ue);
    }

    return ran_ue;
}

ran_ue_t* ran_ue_find_by_amf_ue_ngap_id(c_uint32_t amf_ue_ngap_id)
{
    d_assert(self.amf_ue_ngap_id_hash, return NULL, "Null param");
    return hash_get(self.amf_ue_ngap_id_hash, 
            &amf_ue_ngap_id, sizeof(amf_ue_ngap_id));
}

ran_ue_t* ran_ue_first_in_ran(amf_ran_t *ran)
{
    return list_first(&ran->ran_ue_list);
}

ran_ue_t* ran_ue_next_in_ran(ran_ue_t *ran_ue)
{
    return list_next(ran_ue);
}


/** enb_ue_context handling function */
enb_ue_t* enb_ue_add(amf4g_enb_t *enb)
{
    enb_ue_t *enb_ue = NULL;

    d_assert(self.mme_ue_s1ap_id_hash, return NULL, "Null param");
    d_assert(enb, return NULL, "Null param");

    index_alloc(&enb_ue_pool, &enb_ue);
    d_assert(enb_ue, return NULL, "Null param");

    enb_ue->enb_ue_s1ap_id = INVALID_UE_S1AP_ID;
    enb_ue->mme_ue_s1ap_id = NEXT_ID(self.mme_ue_s1ap_id, 1, 0xffffffff);
    enb_ue->enb = enb;
    
    hash_set(self.mme_ue_s1ap_id_hash, &enb_ue->mme_ue_s1ap_id, 
            sizeof(enb_ue->mme_ue_s1ap_id), enb_ue);
    list_append(&enb->enb_ue_list, enb_ue);

    /* Create S1 holding timer */
    enb_ue->holding_timer = timer_create(&self.tm_service,
            AMF4G_EVT_S1AP_S1_HOLDING_TIMER, self.s1_holding_timer_value * 1000);
    d_assert(enb_ue->holding_timer, return NULL, "Null param");
    timer_set_param1(enb_ue->holding_timer, enb_ue->index);

    return enb_ue;

}

unsigned int enb_ue_count()
{
    d_assert(self.mme_ue_s1ap_id_hash, return 0, "Null param");
    return hash_count(self.mme_ue_s1ap_id_hash);
}

status_t enb_ue_remove(enb_ue_t *enb_ue)
{
    status_t rv;

    d_assert(self.mme_ue_s1ap_id_hash, return CORE_ERROR, "Null param");
    d_assert(enb_ue, return CORE_ERROR, "Null param");
    d_assert(enb_ue->enb, return CORE_ERROR, "Null param");

    /* De-associate S1 with NAS/EMM */
    rv = enb_ue_deassociate(enb_ue);
    d_assert(rv == CORE_OK,,);

   /* Delete All Timers */
    tm_delete(enb_ue->holding_timer);

    list_remove(&enb_ue->enb->enb_ue_list, enb_ue);
    hash_set(self.mme_ue_s1ap_id_hash, &enb_ue->mme_ue_s1ap_id, 
            sizeof(enb_ue->mme_ue_s1ap_id), NULL);

    index_free(&enb_ue_pool, enb_ue);

    return CORE_OK;
}

status_t enb_ue_remove_in_enb(amf4g_enb_t *enb)
{
    enb_ue_t *enb_ue = NULL, *next_enb_ue = NULL;
    
    enb_ue = enb_ue_first_in_enb(enb);
    while (enb_ue)
    {
        next_enb_ue = enb_ue_next_in_enb(enb_ue);

        enb_ue_remove(enb_ue);

        enb_ue = next_enb_ue;
    }

    return CORE_OK;
}

status_t enb_ue_switch_to_enb(enb_ue_t *enb_ue, amf4g_enb_t *new_enb)
{
    d_assert(enb_ue, return CORE_ERROR, "Null param");
    d_assert(enb_ue->enb, return CORE_ERROR, "Null param");
    d_assert(new_enb, return CORE_ERROR, "Null param");

    /* Remove from the old enb */
    list_remove(&enb_ue->enb->enb_ue_list, enb_ue);

    /* Add to the new enb */
    list_append(&new_enb->enb_ue_list, enb_ue);

    /* Switch to enb */
    enb_ue->enb = new_enb;

    return CORE_OK;
}

enb_ue_t* enb_ue_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&enb_ue_pool, index);
}

enb_ue_t* enb_ue_find_by_enb_ue_s1ap_id(
        amf4g_enb_t *enb, c_uint32_t enb_ue_s1ap_id)
{
    enb_ue_t *enb_ue = NULL;
    
    enb_ue = enb_ue_first_in_enb(enb);
    while (enb_ue)
    {
        if (enb_ue_s1ap_id == enb_ue->enb_ue_s1ap_id)
            break;

        enb_ue = enb_ue_next_in_enb(enb_ue);
    }

    return enb_ue;
}

enb_ue_t* enb_ue_find_by_mme_ue_s1ap_id(c_uint32_t mme_ue_s1ap_id)
{
    d_assert(self.mme_ue_s1ap_id_hash, return NULL, "Null param");
    return hash_get(self.mme_ue_s1ap_id_hash, 
            &mme_ue_s1ap_id, sizeof(mme_ue_s1ap_id));
}

enb_ue_t* enb_ue_first_in_enb(amf4g_enb_t *enb)
{
    return list_first(&enb->enb_ue_list);
}

enb_ue_t* enb_ue_next_in_enb(enb_ue_t *enb_ue)
{
    return list_next(enb_ue);
}

static status_t amf4g_ue_new_guti(amf4g_ue_t *amf4g_ue)
{
    served_gummei_t *served_gummei = NULL;

    d_assert(amf4g_ue, return CORE_ERROR, "Invalid param");
    d_assert(amf4g_self()->max_num_of_served_gummei > 0,
            return CORE_ERROR, "Invalid param");

    served_gummei = &amf4g_self()->served_gummei[0];

    d_assert(served_gummei->num_of_plmn_id > 0,
            return CORE_ERROR, "Invalid param");
    d_assert(served_gummei->num_of_amf4g_gid > 0,
            return CORE_ERROR, "Invalid param");
    d_assert(served_gummei->num_of_amf4g_code > 0,
            return CORE_ERROR, "Invalid param");

    if (amf4g_ue->m_tmsi)
    {
        /* AMF4G has a VALID GUIT
         * As such, we need to remove previous GUTI in hash table */
        hash_set(self.guti_ue_hash, &amf4g_ue->guti, sizeof(guti_t), NULL);
        d_assert(amf4g_m_tmsi_free(amf4g_ue->m_tmsi) == CORE_OK,,);
    }

    memset(&amf4g_ue->guti, 0, sizeof(guti_t));

    /* Use the first configured plmn_id and amf4g group id */
    memcpy(&amf4g_ue->guti.plmn_id, &served_gummei->plmn_id[0], PLMN_ID_LEN);
    amf4g_ue->guti.mme_gid = served_gummei->amf4g_gid[0];
    amf4g_ue->guti.mme_code = served_gummei->amf4g_code[0];

    amf4g_ue->m_tmsi = amf4g_m_tmsi_alloc();
    d_assert(amf4g_ue->m_tmsi, return CORE_ERROR,);
    amf4g_ue->guti.m_tmsi = *(amf4g_ue->m_tmsi);
    hash_set(self.guti_ue_hash, &amf4g_ue->guti, sizeof(guti_t), amf4g_ue);

    return CORE_OK;
}

amf4g_ue_t* amf4g_ue_add(enb_ue_t *enb_ue)
{
    amf4g_enb_t *enb = NULL;
    amf4g_ue_t *amf4g_ue = NULL;
    event_t e;

    d_assert(enb_ue, return NULL,);
    enb = enb_ue->enb;
    d_assert(enb, return NULL,);

    index_alloc(&amf4g_ue_pool, &amf4g_ue);
    d_assert(amf4g_ue, return NULL, "Null param");

    list_init(&amf4g_ue->sess_list);

    amf4g_ue->amf4g_s11_teid = amf4g_ue->index;
    amf4g_ue->smf_s11_teid = 0;

    /*
     * SCTP output stream identification
     * Default context_self()->parameter.sctp_streams : 30
     *   0 : Non UE signalling
     *   1-29 : UE specific association 
     */
    amf4g_ue->ostream_id = NEXT_ID(self.ostream_id, 1, enb->outbound_streams-1);

    /* Create New GUTI */
    amf4g_ue_new_guti(amf4g_ue);


    /* Create paging retry timer */
    amf4g_ue->t3413 = timer_create(&self.tm_service, AMF4G_EVT_EMM_T3413,
            self.t3413_value * 1000);
    d_assert(amf4g_ue->t3413, return NULL, "Null param");
    timer_set_param1(amf4g_ue->t3413, amf4g_ue->index);

    event_set_param1(&e, (c_uintptr_t)amf4g_ue->index);
    fsm_create(&amf4g_ue->sm, emm_state_initial, emm_state_final);
    fsm_init(&amf4g_ue->sm, &e);

    return amf4g_ue;
}

status_t amf4g_ue_remove(amf4g_ue_t *amf4g_ue)
{
    status_t rv;
    event_t e;

    d_assert(amf4g_ue, return CORE_ERROR, "Null param");

    event_set_param1(&e, (c_uintptr_t)amf4g_ue->index);
    fsm_final(&amf4g_ue->sm, &e);
    fsm_clear(&amf4g_ue->sm);

    /* Clear hash table */
    if (amf4g_ue->m_tmsi)
    {
        hash_set(self.guti_ue_hash, &amf4g_ue->guti, sizeof(guti_t), NULL);
        d_assert(amf4g_m_tmsi_free(amf4g_ue->m_tmsi) == CORE_OK,,);
    }
    if (amf4g_ue->imsi_len != 0)
        hash_set(self.imsi_ue_hash, amf4g_ue->imsi, amf4g_ue->imsi_len, NULL);
    
    /* Clear the saved PDN Connectivity Request */
    NAS_CLEAR_DATA(&amf4g_ue->pdn_connectivity_request);

    /* Clear Paging info : stop t3413, last_paing_msg */
    CLEAR_PAGING_INFO(amf4g_ue);

    /* Free UeRadioCapability */
#if 0
    if (amf4g_ue->radio_capa)
    {
        S1AP_UERadioCapability_t *radio_capa = 
            (S1AP_UERadioCapability_t *)amf4g_ue->radio_capa;

        if (radio_capa->buf)
            CORE_FREE(radio_capa->buf);
        CORE_FREE(amf4g_ue->radio_capa);
    }
#else
    S1AP_CLEAR_DATA(&amf4g_ue->ueRadioCapability);
#endif

    /* Clear Transparent Container */
    S1AP_CLEAR_DATA(&amf4g_ue->container);

    /* Delete All Timers */
    tm_delete(amf4g_ue->t3413);

    rv = amf4g_ue_deassociate(amf4g_ue);
    d_assert(rv == CORE_OK,,);

    amf4g_sess_remove_all(amf4g_ue);
    amf4g_pdn_remove_all(amf4g_ue);

    index_free(&amf4g_ue_pool, amf4g_ue);

    return CORE_OK;
}

status_t amf4g_ue_remove_all()
{
    hash_index_t *hi = NULL;
    amf4g_ue_t *amf4g_ue = NULL;

    for (hi = amf4g_ue_first(); hi; hi = amf4g_ue_next(hi))
    {
        amf4g_ue = amf4g_ue_this(hi);
        amf4g_ue_remove(amf4g_ue);
    }

    return CORE_OK;
}

amf4g_ue_t* amf4g_ue_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&amf4g_ue_pool, index);
}

amf4g_ue_t* amf4g_ue_find_by_imsi_bcd(c_int8_t *imsi_bcd)
{
    c_uint8_t imsi[MAX_IMSI_LEN];
    int imsi_len = 0;

    d_assert(imsi_bcd, return NULL,"Invalid param");

    core_bcd_to_buffer(imsi_bcd, imsi, &imsi_len);

    return amf4g_ue_find_by_imsi(imsi, imsi_len);
}

amf4g_ue_t* amf4g_ue_find_by_imsi(c_uint8_t *imsi, int imsi_len)
{
    d_assert(imsi && imsi_len, return NULL,"Invalid param");

    return (amf4g_ue_t *)hash_get(self.imsi_ue_hash, imsi, imsi_len);
}

amf4g_ue_t* amf4g_ue_find_by_guti(guti_t *guti)
{
    d_assert(guti, return NULL,"Invalid param");

    return (amf4g_ue_t *)hash_get(self.guti_ue_hash, guti, sizeof(guti_t));
}

amf4g_ue_t* amf4g_ue_find_by_teid(c_uint32_t teid)
{
    return amf4g_ue_find(teid);
}

hash_index_t *amf4g_ue_first()
{
    d_assert(self.imsi_ue_hash, return NULL, "Null param");
    return hash_first(self.imsi_ue_hash);
}

hash_index_t *amf4g_ue_next(hash_index_t *hi)
{
    return hash_next(hi);
}

amf4g_ue_t *amf4g_ue_this(hash_index_t *hi)
{
    d_assert(hi, return NULL, "Null param");
    return hash_this_val(hi);
}

amf4g_ue_t* amf4g_ue_find_by_message(nas_message_t *message)
{
    amf4g_ue_t *amf4g_ue = NULL;

    switch(message->emm.h.message_type)
    {
        case NAS_ATTACH_REQUEST:
        {
            nas_attach_request_t *attach_request =
                &message->emm.attach_request;

            nas_eps_mobile_identity_t *eps_mobile_identity =
                            &attach_request->eps_mobile_identity;

            switch(eps_mobile_identity->imsi.type)
            {
                case NAS_EPS_MOBILE_IDENTITY_IMSI:
                {
                    c_int8_t imsi_bcd[MAX_IMSI_BCD_LEN+1];

                    nas_imsi_to_bcd(
                        &eps_mobile_identity->imsi, eps_mobile_identity->length,
                        imsi_bcd);


                    amf4g_ue = amf4g_ue_find_by_imsi_bcd(imsi_bcd);
                    if (amf4g_ue)
                    {
                        d_trace(9, "known UE by IMSI[%s]\n", imsi_bcd);
                    }
                    else
                    {
                        d_trace(9, "Unknown UE by IMSI[%s]\n", imsi_bcd);
                    }
                    break;
                }
                case NAS_EPS_MOBILE_IDENTITY_GUTI:
                {
                    nas_eps_mobile_identity_guti_t *nas_guti = NULL;
                    nas_guti = &eps_mobile_identity->guti;
                    guti_t guti;

                    guti.plmn_id = nas_guti->plmn_id;
                    guti.mme_gid = nas_guti->mme_gid;
                    guti.mme_code = nas_guti->mme_code;
                    guti.m_tmsi = nas_guti->m_tmsi;

                    amf4g_ue = amf4g_ue_find_by_guti(&guti);
                    if (amf4g_ue)
                    {
                        d_trace(9, "Known UE by GUTI[G:%d,C:%d,M_TMSI:0x%x]\n",
                                guti.mme_gid,
                                guti.mme_code,
                                guti.m_tmsi);
                    }
                    else
                    {
                        d_warn("Unknown UE by GUTI[G:%d,C:%d,M_TMSI:0x%x]",
                                guti.mme_gid,
                                guti.mme_code,
                                guti.m_tmsi);
                    }
                    break;
                }
                default:
                {
                    d_error("Uknown message imsi type =%d\n",
                            eps_mobile_identity->imsi.type);
                    break;
                }
            }
            break;
        }
        case NAS_DETACH_REQUEST:
        {
            /* TODO */
            break;
        }
        case NAS_TRACKING_AREA_UPDATE_REQUEST:
        {
            nas_tracking_area_update_request_t *tau_request =
                &message->emm.tracking_area_update_request;

            nas_eps_mobile_identity_t *eps_mobile_identity =
                            &tau_request->old_guti;

            switch(eps_mobile_identity->imsi.type)
            {
                case NAS_EPS_MOBILE_IDENTITY_GUTI:
                {
                    nas_eps_mobile_identity_guti_t *nas_guti = NULL;
                    nas_guti = &eps_mobile_identity->guti;
                    guti_t guti;

                    guti.plmn_id = nas_guti->plmn_id;
                    guti.mme_gid = nas_guti->mme_gid;
                    guti.mme_code = nas_guti->mme_code;
                    guti.m_tmsi = nas_guti->m_tmsi;

                    amf4g_ue = amf4g_ue_find_by_guti(&guti);
                    if (amf4g_ue)
                    {
                        d_trace(9, "Known UE by GUTI[G:%d,C:%d,M_TMSI:0x%x]\n",
                                guti.mme_gid,
                                guti.mme_code,
                                guti.m_tmsi);
                    }
                    else
                    {
                        d_warn("Unknown UE by GUTI[G:%d,C:%d,M_TMSI:0x%x]",
                                guti.mme_gid,
                                guti.mme_code,
                                guti.m_tmsi);
                    }
                    break;
                }
                default:
                {
                    d_error("Uknown message imsi type =%d\n",
                            eps_mobile_identity->imsi.type);
                    break;
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return amf4g_ue;
}

status_t amf4g_ue_set_imsi(amf4g_ue_t *amf4g_ue, c_int8_t *imsi_bcd)
{
    d_assert(amf4g_ue && imsi_bcd, return CORE_ERROR, "Invalid param");

    core_cpystrn(amf4g_ue->imsi_bcd, imsi_bcd, MAX_IMSI_BCD_LEN+1);
    core_bcd_to_buffer(amf4g_ue->imsi_bcd, amf4g_ue->imsi, &amf4g_ue->imsi_len);

    hash_set(self.imsi_ue_hash, amf4g_ue->imsi, amf4g_ue->imsi_len, amf4g_ue);

    amf4g_ue->guti_present = 1;

    return CORE_OK;
}

int amf4g_ue_have_indirect_tunnel(amf4g_ue_t *amf4g_ue)
{
    amf4g_sess_t *sess = NULL;

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        amf4g_bearer_t *bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            if (AMF4G_HAVE_ENB_DL_INDIRECT_TUNNEL(bearer) ||
                AMF4G_HAVE_ENB_UL_INDIRECT_TUNNEL(bearer) ||
                AMF4G_HAVE_UPF_DL_INDIRECT_TUNNEL(bearer) ||
                AMF4G_HAVE_UPF_UL_INDIRECT_TUNNEL(bearer))
            {
                return 1;
            }

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    return 0;
}

status_t amf4g_ue_clear_indirect_tunnel(amf4g_ue_t *amf4g_ue)
{
    amf4g_sess_t *sess = NULL;

    d_assert(amf4g_ue, return CORE_ERROR,);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        amf4g_bearer_t *bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            CLEAR_INDIRECT_TUNNEL(bearer);

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    return CORE_OK;
}

status_t amf4g_ue_associate_enb_ue(amf4g_ue_t *amf4g_ue, enb_ue_t *enb_ue)
{
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    d_assert(enb_ue, return CORE_ERROR, "Null param");

    amf4g_ue->enb_ue = enb_ue;
    enb_ue->amf4g_ue = amf4g_ue;

    return CORE_OK;
}

status_t enb_ue_deassociate(enb_ue_t *enb_ue)
{
    d_assert(enb_ue, return CORE_ERROR, "Null param");
    enb_ue->amf4g_ue = NULL;
    
    return CORE_OK;
}
status_t ran_ue_deassociate(ran_ue_t *ran_ue)
{
    d_assert(ran_ue, return CORE_ERROR, "Null param");
    ran_ue->amf_ue = NULL;
    
    return CORE_OK;
}
status_t amf4g_ue_deassociate(amf4g_ue_t *amf4g_ue)
{
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    amf4g_ue->enb_ue = NULL;
    
    return CORE_OK;
}

status_t source_ue_associate_target_ue(
        enb_ue_t *source_ue, enb_ue_t *target_ue)
{
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(source_ue, return CORE_ERROR, "Null param");
    d_assert(target_ue, return CORE_ERROR, "Null param");
    amf4g_ue = source_ue->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");

    target_ue->amf4g_ue = amf4g_ue;
    target_ue->source_ue = source_ue;
    source_ue->target_ue = target_ue;

    return CORE_OK;
}

status_t source_ue_deassociate_target_ue(enb_ue_t *enb_ue)
{
    enb_ue_t *source_ue = NULL;
    enb_ue_t *target_ue = NULL;
    d_assert(enb_ue, return CORE_ERROR,);

    if (enb_ue->target_ue)
    {
        source_ue = enb_ue;
        target_ue = enb_ue->target_ue;

        d_assert(source_ue->target_ue, return CORE_ERROR,);
        d_assert(target_ue->source_ue, return CORE_ERROR,);
        source_ue->target_ue = NULL;
        target_ue->source_ue = NULL;
    }
    else if (enb_ue->source_ue)
    {
        target_ue = enb_ue;
        source_ue = enb_ue->source_ue;

        d_assert(source_ue->target_ue, return CORE_ERROR,);
        d_assert(target_ue->source_ue, return CORE_ERROR,);
        source_ue->target_ue = NULL;
        target_ue->source_ue = NULL;
    }

    return CORE_OK;
}
amf4g_sess_t *amf4g_sess_add(amf4g_ue_t *amf4g_ue, c_uint8_t pti)
{
    amf4g_sess_t *sess = NULL;
    amf4g_bearer_t *bearer = NULL;

    d_assert(amf4g_ue, return NULL, "Null param");
    d_assert(pti != NAS_PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED,
            return NULL, "Invalid PTI(%d)", pti);


    index_alloc(&amf4g_sess_pool, &sess);
    d_assert(sess, return NULL, "Null param");

    list_init(&sess->bearer_list);

    sess->amf4g_ue = amf4g_ue;
    sess->pti = pti;

    bearer = amf4g_bearer_add(sess);
    d_assert(bearer, amf4g_sess_remove(sess); return NULL, 
            "Can't add default bearer context");

    list_append(&amf4g_ue->sess_list, sess);

    return sess;
}

status_t amf4g_sess_remove(amf4g_sess_t *sess)
{
    d_assert(sess, return CORE_ERROR, "Null param");
    d_assert(sess->amf4g_ue, return CORE_ERROR, "Null param");
    
    list_remove(&sess->amf4g_ue->sess_list, sess);

    amf4g_bearer_remove_all(sess);

    NAS_CLEAR_DATA(&sess->ue_pco);
    TLV_CLEAR_DATA(&sess->pgw_pco);

    index_free(&amf4g_sess_pool, sess);

    return CORE_OK;
}

status_t amf4g_sess_remove_all(amf4g_ue_t *amf4g_ue)
{
    amf4g_sess_t *sess = NULL, *next_sess = NULL;
    
    sess = amf4g_sess_first(amf4g_ue);
    while (sess)
    {
        next_sess = amf4g_sess_next(sess);

        amf4g_sess_remove(sess);

        sess = next_sess;
    }

    return CORE_OK;
}

amf4g_sess_t* amf4g_sess_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&amf4g_sess_pool, index);
}

amf4g_sess_t* amf4g_sess_find_by_pti(amf4g_ue_t *amf4g_ue, c_uint8_t pti)
{
    amf4g_sess_t *sess = NULL;

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        if (pti == sess->pti)
            return sess;

        sess = amf4g_sess_next(sess);
    }

    return NULL;
}

amf4g_sess_t* amf4g_sess_find_by_ebi(amf4g_ue_t *amf4g_ue, c_uint8_t ebi)
{
    amf4g_bearer_t *bearer = NULL;

    bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, ebi);
    if (bearer)
        return bearer->sess;

    return NULL;
}

amf4g_sess_t* amf4g_sess_find_by_apn(amf4g_ue_t *amf4g_ue, c_int8_t *apn)
{
    amf4g_sess_t *sess = NULL;

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        if (sess->pdn && strcmp(sess->pdn->apn, apn) == 0)
            return sess;

        sess = amf4g_sess_next(sess);
    }

    return NULL;
}

amf4g_sess_t* amf4g_sess_first(amf4g_ue_t *amf4g_ue)
{
    return list_first(&amf4g_ue->sess_list);
}

amf4g_sess_t* amf4g_sess_next(amf4g_sess_t *sess)
{
    return list_next(sess);
}

unsigned int  amf4g_sess_count(amf4g_ue_t *amf4g_ue)
{
    unsigned int count = 0;
    amf4g_sess_t *sess = NULL;

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        sess = amf4g_sess_next(sess);
        count++;
    }

    return count;
}

amf4g_bearer_t* amf4g_bearer_add(amf4g_sess_t *sess)
{
    event_t e;

    amf4g_bearer_t *bearer = NULL;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(sess, return NULL, "Null param");
    amf4g_ue = sess->amf4g_ue;
    d_assert(amf4g_ue, return NULL, "Null param");

    index_alloc(&amf4g_bearer_pool, &bearer);
    d_assert(bearer, return NULL, "Null param");

    bearer->ebi = NEXT_ID(amf4g_ue->ebi, MIN_EPS_BEARER_ID, MAX_EPS_BEARER_ID);

    bearer->amf4g_ue = amf4g_ue;
    bearer->sess = sess;

    list_append(&sess->bearer_list, bearer);
    
    event_set_param1(&e, (c_uintptr_t)bearer->index);
    fsm_create(&bearer->sm, esm_state_initial, esm_state_final);
    fsm_init(&bearer->sm, &e);

    return bearer;
}

status_t amf4g_bearer_remove(amf4g_bearer_t *bearer)
{
    event_t e;

    d_assert(bearer, return CORE_ERROR, "Null param");
    d_assert(bearer->sess, return CORE_ERROR, "Null param");

    event_set_param1(&e, (c_uintptr_t)bearer->index);
    fsm_final(&bearer->sm, &e);
    fsm_clear(&bearer->sm);

    list_remove(&bearer->sess->bearer_list, bearer);

    TLV_CLEAR_DATA(&bearer->tft);
    
    index_free(&amf4g_bearer_pool, bearer);

    return CORE_OK;
}

status_t amf4g_bearer_remove_all(amf4g_sess_t *sess)
{
    amf4g_bearer_t *bearer = NULL, *next_bearer = NULL;

    d_assert(sess, return CORE_ERROR, "Null param");
    
    bearer = amf4g_bearer_first(sess);
    while (bearer)
    {
        next_bearer = amf4g_bearer_next(bearer);

        amf4g_bearer_remove(bearer);

        bearer = next_bearer;
    }

    return CORE_OK;
}

amf4g_bearer_t* amf4g_bearer_find(index_t index)
{
    d_assert(index, return NULL, "Invalid Index");
    return index_find(&amf4g_bearer_pool, index);
}

amf4g_bearer_t* amf4g_bearer_find_by_sess_ebi(amf4g_sess_t *sess, c_uint8_t ebi)
{
    amf4g_bearer_t *bearer = NULL;

    bearer = amf4g_bearer_first(sess);
    while(bearer)
    {
        if (ebi == bearer->ebi)
            return bearer;

        bearer = amf4g_bearer_next(bearer);
    }

    return NULL;
}

amf4g_bearer_t* amf4g_bearer_find_by_ue_ebi(amf4g_ue_t *amf4g_ue, c_uint8_t ebi)
{
    amf4g_sess_t *sess = NULL;
    amf4g_bearer_t *bearer = NULL;
    
    sess = amf4g_sess_first(amf4g_ue);
    while (sess)
    {
        bearer = amf4g_bearer_find_by_sess_ebi(sess, ebi);
        if (bearer)
        {
            return bearer;
        }

        sess = amf4g_sess_next(sess);
    }

    return NULL;
}

amf4g_bearer_t* amf4g_bearer_find_or_add_by_message(
        amf4g_ue_t *amf4g_ue, nas_message_t *message)
{
    c_uint8_t pti = NAS_PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED;
    c_uint8_t ebi = NAS_EPS_BEARER_IDENTITY_UNASSIGNED;

    amf4g_bearer_t *bearer = NULL;
    amf4g_sess_t *sess = NULL;

    d_assert(amf4g_ue, return NULL,);
    d_assert(message, return NULL,);

    pti = message->esm.h.procedure_transaction_identity;
    ebi = message->esm.h.eps_bearer_identity;

    d_trace(9, "amf4g_bearer_find_or_add_by_message() [PTI:%d, EBI:%d]\n",
            pti, ebi);

    if (ebi != NAS_EPS_BEARER_IDENTITY_UNASSIGNED)
    {
        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, ebi);
        d_assert(bearer, return NULL, "No Bearer : [EBI:%d]", ebi);
        return bearer;
    }
        
    d_assert(pti != NAS_PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED,
            return NULL, "Invalid param : [PTI:%d, EBI:%d]", pti, ebi);

    if (message->esm.h.message_type == NAS_PDN_CONNECTIVITY_REQUEST)
    {
        nas_pdn_connectivity_request_t *pdn_connectivity_request =
            &message->esm.pdn_connectivity_request;
        if (pdn_connectivity_request->presencemask &
                NAS_PDN_CONNECTIVITY_REQUEST_ACCESS_POINT_NAME_PRESENT)
            sess = amf4g_sess_find_by_apn(amf4g_ue,
                    pdn_connectivity_request->access_point_name.apn);
        else
            sess = amf4g_sess_first(amf4g_ue);

        if (!sess)
            sess = amf4g_sess_add(amf4g_ue, pti);
        else
            sess->pti = pti;

        d_assert(sess, return NULL, "No Session : [PTI:%d]", pti);
    }
    else if (message->esm.h.message_type == NAS_PDN_DISCONNECT_REQUEST)
    {
        nas_pdn_disconnect_request_t *pdn_disconnect_request = 
            &message->esm.pdn_disconnect_request;
        nas_linked_eps_bearer_identity_t *linked_eps_bearer_identity =
            &pdn_disconnect_request->linked_eps_bearer_identity;

        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue,
                linked_eps_bearer_identity->eps_bearer_identity);
        d_assert(bearer, return NULL,
                "No Bearer : [Linked-EBI:%d, PTI:%d, EBI:%d]",
                linked_eps_bearer_identity->eps_bearer_identity,
                pti, ebi);
        sess = bearer->sess;
        d_assert(sess, return NULL, "No Session : [PTI:%d]", pti);
        sess->pti = pti;

        return bearer;
    }
    else
    {
        sess = amf4g_sess_find_by_pti(amf4g_ue, pti);
        d_assert(sess, return NULL, "No Session : [PTI:%d]", pti);
    }

    bearer = amf4g_default_bearer_in_sess(sess);
    d_assert(bearer, return NULL, "No Bearer : [EBI:%d]", ebi);
    return bearer;
}

amf4g_bearer_t* amf4g_default_bearer_in_sess(amf4g_sess_t *sess)
{
    return amf4g_bearer_first(sess);
}

amf4g_bearer_t* amf4g_linked_bearer(amf4g_bearer_t *bearer)
{
    amf4g_sess_t *sess = NULL;

    d_assert(bearer, return NULL, "Null param");
    sess = bearer->sess;
    d_assert(sess, return NULL, "Null param");

    return amf4g_default_bearer_in_sess(sess);
}

amf4g_bearer_t* amf4g_bearer_first(amf4g_sess_t *sess)
{
    d_assert(sess, return NULL, "Null param");

    return list_first(&sess->bearer_list);
}

amf4g_bearer_t* amf4g_bearer_next(amf4g_bearer_t *bearer)
{
    return list_next(bearer);
}

int amf4g_bearer_is_inactive(amf4g_ue_t *amf4g_ue)
{
    amf4g_sess_t *sess = NULL;
    d_assert(amf4g_ue, return 1,);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        amf4g_bearer_t *bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            if (AMF4G_HAVE_ENB_S1U_PATH(bearer))
            {
                return 0;
            }

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    return 1;
}

status_t amf4g_bearer_set_inactive(amf4g_ue_t *amf4g_ue)
{
    amf4g_sess_t *sess = NULL;
    d_assert(amf4g_ue, return CORE_ERROR,);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        amf4g_bearer_t *bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            CLEAR_ENB_S1U_PATH(bearer);

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    return CORE_OK;
}

status_t amf4g_pdn_remove_all(amf4g_ue_t *amf4g_ue)
{
    s6a_subscription_data_t *subscription_data = NULL;

    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    subscription_data = &amf4g_ue->subscription_data;
    d_assert(subscription_data, return CORE_ERROR, "Null param");

    subscription_data->num_of_pdn = 0;
    
    return CORE_OK;
}

pdn_t* amf4g_pdn_find_by_apn(amf4g_ue_t *amf4g_ue, c_int8_t *apn)
{
    s6a_subscription_data_t *subscription_data = NULL;
    pdn_t *pdn = NULL;
    int i = 0;
    
    d_assert(amf4g_ue, return NULL, "Null param");
    subscription_data = &amf4g_ue->subscription_data;
    d_assert(subscription_data, return NULL, "Null param");

    for (i = 0; i < subscription_data->num_of_pdn; i++)
    {
        pdn = &subscription_data->pdn[i];
        if (strcmp(pdn->apn, apn) == 0)
            return pdn;
    }

    return NULL;
}

pdn_t* amf4g_default_pdn(amf4g_ue_t *amf4g_ue)
{
    s6a_subscription_data_t *subscription_data = NULL;
    pdn_t *pdn = NULL;
    int i = 0;
    
    d_assert(amf4g_ue, return NULL, "Null param");
    subscription_data = &amf4g_ue->subscription_data;
    d_assert(subscription_data, return NULL, "Null param");

    for (i = 0; i < subscription_data->num_of_pdn; i++)
    {
        pdn = &subscription_data->pdn[i];
        if (pdn->context_identifier == subscription_data->context_identifier)
            return pdn;
    }

    return NULL;
}

int amf_find_served_tai(tai_t *tai)
{
    int i = 0, j = 0, k = 0;

    d_assert(tai, return -1,);

    for (i = 0; i < self.num_of_served_tai; i++)
    {
        tai0_list_t *list0 = &self.served_tai[i].list0;
        d_assert(list0, return -1,);
        tai2_list_t *list2 = &self.served_tai[i].list2;
        d_assert(list2, return -1,);

        for (j = 0; list0->tai[j].num; j++)
        {
            d_assert(list0->tai[j].type == TAI0_TYPE,
                return -1, "type = %d", list0->tai[j].type);
            d_assert(list0->tai[j].num < MAX_NUM_OF_TAI,
                    return -1, "num = %d", list0->tai[j].num);

            for (k = 0; k < list0->tai[j].num; k++) 
            {
                if (memcmp(&list0->tai[j].plmn_id,
                            &tai->plmn_id, PLMN_ID_LEN) == 0 && 
                    list0->tai[j].tac[k] == tai->tac)
                {
                    return i;
                }
            }
        }

        if (list2->num)
        {
            d_assert(list2->type == TAI1_TYPE || list2->type == TAI2_TYPE,
                return -1, "type = %d", list2->type);
            d_assert(list2->num < MAX_NUM_OF_TAI,
                    return -1, "num = %d", list2->num);

            for (j = 0; j < list2->num; j++) 
            {
                if (memcmp(&list2->tai[j].plmn_id,
                            &tai->plmn_id, PLMN_ID_LEN) == 0 && 
                    list2->tai[j].tac == tai->tac)
                {
                    return i;
                }
            }
        }
    }

    return -1;
}

int amf4g_find_served_tai(tai_t *tai)
{
    int i = 0, j = 0, k = 0;

    d_assert(tai, return -1,);

    for (i = 0; i < self.num_of_served_tai; i++)
    {
        tai0_list_t *list0 = &self.served_tai[i].list0;
        d_assert(list0, return -1,);
        tai2_list_t *list2 = &self.served_tai[i].list2;
        d_assert(list2, return -1,);

        for (j = 0; list0->tai[j].num; j++)
        {
            d_assert(list0->tai[j].type == TAI0_TYPE,
                return -1, "type = %d", list0->tai[j].type);
            d_assert(list0->tai[j].num < MAX_NUM_OF_TAI,
                    return -1, "num = %d", list0->tai[j].num);

            for (k = 0; k < list0->tai[j].num; k++) 
            {
                if (memcmp(&list0->tai[j].plmn_id,
                            &tai->plmn_id, PLMN_ID_LEN) == 0 && 
                    list0->tai[j].tac[k] == tai->tac)
                {
                    return i;
                }
            }
        }

        if (list2->num)
        {
            d_assert(list2->type == TAI1_TYPE || list2->type == TAI2_TYPE,
                return -1, "type = %d", list2->type);
            d_assert(list2->num < MAX_NUM_OF_TAI,
                    return -1, "num = %d", list2->num);

            for (j = 0; j < list2->num; j++) 
            {
                if (memcmp(&list2->tai[j].plmn_id,
                            &tai->plmn_id, PLMN_ID_LEN) == 0 && 
                    list2->tai[j].tac == tai->tac)
                {
                    return i;
                }
            }
        }
    }

    return -1;
}

status_t amf4g_m_tmsi_pool_generate()
{
    status_t rv;
    int i, j;
    int index = 0;

    d_trace(9, "M-TMSI Pool try to generate...\n");
    for (i = 0; index < MAX_POOL_OF_UE; i++)
    {
        amf4g_m_tmsi_t *m_tmsi = NULL;
        int conflict = 0;

        m_tmsi = &self.m_tmsi.pool[index];
        rv = core_generate_random_bytes((c_uint8_t *)m_tmsi, sizeof(*m_tmsi));
        d_assert(rv == CORE_OK, return CORE_ERROR, "Cannot generate random");

        /* for mapped-GUTI */
        *m_tmsi |= 0xc0000000;
        *m_tmsi &= 0xff00ffff;

        for (j = 0; j < index; j++)
        {
            if (*m_tmsi == self.m_tmsi.pool[j])
            {
                conflict = 1;
                d_trace(11, "[M-TMSI CONFLICT]  %d:0x%x == %d:0x%x\n",
                        index, *m_tmsi, j, self.m_tmsi.pool[j]);
                break;
            }
        }
        if (conflict == 1) continue;

        index++;
    }
    self.m_tmsi.size = index;
    d_trace(9, "M-TMSI Pool generate...done\n");

    return CORE_OK;
}

amf4g_m_tmsi_t *amf4g_m_tmsi_alloc()
{
    amf4g_m_tmsi_t *m_tmsi = NULL;

    pool_alloc_node(&self.m_tmsi, &m_tmsi);
    d_assert(m_tmsi, return NULL,);

    return m_tmsi;
}

status_t amf4g_m_tmsi_free(amf4g_m_tmsi_t *m_tmsi)
{
    d_assert(m_tmsi, return CORE_ERROR,);
    pool_free_node(&self.m_tmsi, m_tmsi);

    return CORE_OK;
}

status_t amf4g_overload_checking_init()
{
    c_uint32_t duration = 10 * 1000;

    tm_block_id *timer = &self.overloading_checking_timer;
    *timer = timer_create(
            &self.tm_service, AMF4G_EVT_CHECK_OVERLOAD, duration);
    d_assert(*timer, return CORE_ERROR,);

    timer_set_param1(*timer, *timer);
    tm_start(*timer);

    return CORE_OK;
}

status_t amf_ue_deassociate(amf_ue_t *amf_ue)
{
   d_assert(amf_ue, return CORE_ERROR, "Null param");
   amf_ue->ran_ue = NULL;
    
   return CORE_OK;
}

status_t amf_ue_remove(amf_ue_t *amf_ue)
{
    // status_t rv;
    event_t e;

    d_assert(amf_ue, return CORE_ERROR, "Null param");

    event_set_param1(&e, (c_uintptr_t)amf_ue->index);
    fsm_final(&amf_ue->sm, &e);
    fsm_clear(&amf_ue->sm);
#if 0 //TODO: clean function.
    /* Clear hash table */
    if (amf4g_ue->m_tmsi)
    {
        hash_set(self.guti_ue_hash, &amf4g_ue->guti, sizeof(guti_t), NULL);
        d_assert(amf4g_m_tmsi_free(amf4g_ue->m_tmsi) == CORE_OK,,);
    }
    if (amf4g_ue->imsi_len != 0)
        hash_set(self.imsi_ue_hash, amf4g_ue->imsi, amf4g_ue->imsi_len, NULL);
    
    /* Clear the saved PDN Connectivity Request */
    NAS_CLEAR_DATA(&amf4g_ue->pdn_connectivity_request);

    /* Clear Paging info : stop t3413, last_paing_msg */
    CLEAR_PAGING_INFO(amf4g_ue);

    /* Free UeRadioCapability */
#if 0
    if (amf4g_ue->radio_capa)
    {
        S1AP_UERadioCapability_t *radio_capa = 
            (S1AP_UERadioCapability_t *)amf4g_ue->radio_capa;

        if (radio_capa->buf)
            CORE_FREE(radio_capa->buf);
        CORE_FREE(amf4g_ue->radio_capa);
    }
#else
    S1AP_CLEAR_DATA(&amf4g_ue->ueRadioCapability);
#endif

    /* Clear Transparent Container */
    S1AP_CLEAR_DATA(&amf4g_ue->container);

    /* Delete All Timers */
    tm_delete(amf4g_ue->t3413);

    rv = amf4g_ue_deassociate(amf4g_ue);
    d_assert(rv == CORE_OK,,);

    amf4g_sess_remove_all(amf4g_ue);
    amf4g_pdn_remove_all(amf4g_ue);

    index_free(&amf4g_ue_pool, amf4g_ue);
#endif
    return CORE_OK;
}

status_t source_ue_deassociate_target_ue_5g(ran_ue_t *ran_ue)
{
    ran_ue_t *source_ue = NULL;
    ran_ue_t *target_ue = NULL;
    d_assert(ran_ue, return CORE_ERROR,);

    if (ran_ue->target_ue)
    {
        source_ue = ran_ue;
        target_ue = ran_ue->target_ue;

        d_assert(source_ue->target_ue, return CORE_ERROR,);
        d_assert(target_ue->source_ue, return CORE_ERROR,);
        source_ue->target_ue = NULL;
        target_ue->source_ue = NULL;
    }
    else if (ran_ue->source_ue)
    {
        target_ue = ran_ue;
        source_ue = ran_ue->source_ue;

        d_assert(source_ue->target_ue, return CORE_ERROR,);
        d_assert(target_ue->source_ue, return CORE_ERROR,);
        source_ue->target_ue = NULL;
        target_ue->source_ue = NULL;
    }

    return CORE_OK;
}

status_t source_ue_associate_target_5g(
        ran_ue_t *source_ue, ran_ue_t *target_ue)
{
    amf_ue_t *amf_ue = NULL;

    d_assert(source_ue, return CORE_ERROR, "Null param");
    d_assert(target_ue, return CORE_ERROR, "Null param");
    amf_ue = source_ue->amf_ue;
    d_assert(amf_ue, return CORE_ERROR, "Null param");

    target_ue->amf_ue = amf_ue;
    target_ue->source_ue = source_ue;
    source_ue->target_ue = target_ue;

    return CORE_OK;
}


status_t amf_ue_associate_ran_ue(amf_ue_t *amf_ue, ran_ue_t *ran_ue)
{
    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_assert(ran_ue, return CORE_ERROR, "Null param");

    amf_ue->ran_ue = ran_ue;
    ran_ue->amf_ue = amf_ue;


    return CORE_OK;
}
