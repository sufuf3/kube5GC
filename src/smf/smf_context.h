#ifndef __SMF_CONTEXT_H__
#define __SMF_CONTEXT_H__

#include "core.h"
#include "core_index.h"
#include "core_list.h"
#include "core_hash.h"
#include "core_msgq.h"
#include "core_network.h"
#include "core_timer.h"

#include "3gpp_types.h"
#include "gtp/gtp_node.h"
#include "gtp/gtp_message.h"

#include "fd/fd_lib.h"

#define MAX_NUM_OF_SUBNET       16
#define MAX_SMF_SESS_HASH_LEN   MAX_IMSI_LEN + MAX_APN_LEN + 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _smf_subnet_t smf_subnet_t;
typedef struct _smf_ue_ip_t smf_ue_ip_t;
    
typedef struct _smf_context_t {
    const char*         fd_conf_path;   /* SMF freeDiameter conf path */
    fd_config_t         *fd_config;     /* SMF freeDiameter config */

    c_uint32_t          pfcp_port;      /* Default: SMF PFCP local port */
    c_uint32_t          gtpc_port;      /* Default: SMF GTP-C local port */

    list_t              gtpc_list;      /* SMF GTP-C IPv4 Server List */
    list_t              gtpc_list6;     /* SMF GTP-C IPv6 Server List */
    sock_id             gtpc_sock;      /* SMF GTP-C IPv4 Socket */
    sock_id             gtpc_sock6;     /* SMF GTP-C IPv6 Socket */
    c_sockaddr_t        *gtpc_addr;     /* SMF GTP-C IPv4 Address */
    c_sockaddr_t        *gtpc_addr6;    /* SMF GTP-C IPv6 Address */

    list_t              subnet_list;    /* SMF UE Subnet List */
    
    hash_t              *sess_hash;     /* Session Hash Table (IMSI + APN) */
    
    c_uint32_t          start_time;     /* Timestamp of SMF start up time */
    msgq_id             queue_id;       /* Qsesssess for processing SMF control plane */
    tm_service_t        tm_service;     /* Timer Service */
} smf_context_t;

typedef struct _smf_sess_t {
    index_t         index;              /**< An index of this node */
    
    /* User-Lication-Info */
    tai_t           tai;
    e_cgi_t         e_cgi;
    
    /* APN Configuration */
    pdn_t           pdn;
    smf_ue_ip_t*    ipv4;
    smf_ue_ip_t*    ipv6;

    /* IMSI */
    c_uint8_t       imsi[MAX_IMSI_LEN];
    int             imsi_len;
    c_int8_t        imsi_bcd[MAX_IMSI_BCD_LEN + 1];

    /* Hash Key : IMSI + APN */
    c_uint8_t       hash_keybuf[MAX_SMF_SESS_HASH_LEN];
    int             hash_keylen;
    
    list_t          bearer_list;
    
    /* Related Context */
    gtp_node_t      *mme_node;
} smf_sess_t;

typedef struct _smf_bearer_t {
    lnode_t         node;           /**< A node of list_t */
    index_t         index;
    
    c_uint8_t       ebi;

    c_int8_t        *name;          /* PCC Rule Name */
    qos_t           qos;            /* QoS Infomration */

    /* Packet Filter Identifier Generator(1~15) */
    c_uint8_t       pf_identifier;
    /* Packet Filter List */
    list_t          pf_list;
    
    smf_sess_t      *sess;
    
    gtp_node_t      *gnode;
} smf_bearer_t;

typedef struct _smf_ue_ip_t {
    c_uint32_t      addr[4];

    /* Related Context */
    smf_subnet_t    *subnet;
} smf_ue_ip_t;

typedef struct _smf_subnet_t {
    lnode_t     node;

    ipsubnet_t  sub;                    /* Subnet : cafe::0/64 */
    ipsubnet_t  gw;                     /* Gateway : cafe::1 */
    c_int8_t    apn[MAX_APN_LEN];       /* APN : "internet", "volte", .. */

    int         family;                 /* AF_INET or AF_INET6 */
    c_uint8_t   prefixlen;              /* prefixlen */

    struct {
        int head, tail;
        int size, avail;
        mutex_id mut;
        smf_ue_ip_t *free[MAX_POOL_OF_SESS], pool[MAX_POOL_OF_SESS];
    } pool;
    
} smf_subnet_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_CONTEXT_H__ */

CORE_DECLARE(status_t)          smf_context_init(void);
CORE_DECLARE(status_t)          smf_context_final(void);
CORE_DECLARE(smf_context_t*)    smf_self(void);

CORE_DECLARE(status_t)          smf_context_parse_config(void);
CORE_DECLARE(status_t)          smf_context_setup_trace_module(void);

CORE_DECLARE(smf_sess_t*)       smf_sess_add_or_find_by_message(
                                    gtp_message_t *message);
CORE_DECLARE(smf_sess_t*)       smf_sess_add(c_uint8_t *imsi, int imsi_len, 
                                    c_int8_t *apn, c_uint8_t pdn_type, 
                                    c_uint8_t ebi);
CORE_DECLARE(status_t )         smf_sess_remove(smf_sess_t *sess);
CORE_DECLARE(status_t )         smf_sess_remove_all();
CORE_DECLARE(smf_sess_t*)       smf_sess_find(index_t index);
CORE_DECLARE(smf_sess_t*)       smf_sess_find_by_seid(c_uint64_t seid);
CORE_DECLARE(smf_sess_t*)       smf_sess_find_by_teid(c_uint32_t teid);
CORE_DECLARE(smf_sess_t*)       smf_sess_find_by_imsi_apn(
                                    c_uint8_t *imsi, int imsi_len,
                                    c_int8_t *apn);
CORE_DECLARE(smf_sess_t*)       smf_sess_this(hash_index_t *hi);
CORE_DECLARE(hash_index_t*)     smf_sess_first();
CORE_DECLARE(hash_index_t*)     smf_sess_next(hash_index_t *hi);

CORE_DECLARE(smf_bearer_t*)     smf_bearer_add(smf_sess_t *sess);
CORE_DECLARE(status_t)          smf_bearer_remove(smf_bearer_t *bearer);

CORE_DECLARE(status_t )         smf_ue_pool_generate(void);
CORE_DECLARE(smf_ue_ip_t*)      smf_ue_ip_alloc(int family, const char *apn);
CORE_DECLARE(status_t)          smf_ue_ip_free(smf_ue_ip_t *ip);

CORE_DECLARE(smf_subnet_t*)     smf_subnet_add(
                                    const char *ipstr,
                                    const char *mask_or_numbits,
                                    const char *apn);
CORE_DECLARE(status_t)          smf_subnet_remove(smf_subnet_t *subnet);
CORE_DECLARE(status_t)          smf_subnet_remove_all();
CORE_DECLARE(smf_subnet_t*)     smf_subnet_first();
CORE_DECLARE(smf_subnet_t*)     smf_subnet_next(smf_subnet_t *subnet);