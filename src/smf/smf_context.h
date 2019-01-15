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
#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_node.h"

#include "fd/fd_lib.h"

#include "JsonTransform.h"

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
    c_uint8_t           cp_function_features;

    char                rest_api_addr[100];
    char                rest_api_port[6];

    list_t              pfcp_list;      /* SMF PFCP IPv4 Server List */
    list_t              pfcp_list6;     /* SMF PFCP IPv6 Server List */
    sock_id             pfcp_sock;      /* SMF PFCP IPv4 Socket */
    sock_id             pfcp_sock6;     /* SMF PFCP IPv6 Socket */
    c_sockaddr_t        *pfcp_addr;     /* SMF PFCP IPv4 Address */
    c_sockaddr_t        *pfcp_addr6;    /* SMF PFCP IPv6 Address */
    pfcp_node_id_t      pfcp_node_id;   /* SMF PFCP Node ID (IPv4) */
    pfcp_node_id_t      pfcp_node_id6;  /* SMF PFCP Node ID (IPv6) */
    
    list_t              subnet_list;    /* SMF UE Subnet List */
    
    list_t              upf_n4_list;    /* UPF PFCP client List */
    
    hash_t              *sess_hash;     /* Session Hash Table (IMSI + APN) */

    pid_t               server_pid;
    
    #define MAX_NUM_OF_DNS              2
    const char          *dns[MAX_NUM_OF_DNS];
    const char          *dns6[MAX_NUM_OF_DNS];
    
    c_uint32_t          start_time;     /* Timestamp of SMF start up time */
    msgq_id             queue_id;       /* Qsesssess for processing SMF control plane */
    tm_service_t        tm_service;     /* Timer Service */
} smf_context_t;

typedef struct _smf_far_t smf_far_t;
typedef struct _smf_urr_t smf_urr_t;
typedef struct _smf_qer_t smf_qer_t;
typedef struct _smf_pdr_t smf_pdr_t;

typedef struct _smf_sess_t {
    index_t         index;              /**< An index of this node */
    
    c_uint64_t      smf_n4_seid;    /* SMF SEID is derived from INDEX */
    c_uint64_t      upf_n4_seid;    /* UPF SEID is received from UPF */
    
    c_int8_t        *gx_sid;        /* Gx Session ID */
    
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
    
    /* Session Related Rules */
    smf_pdr_t       *ul_pdr;
    smf_pdr_t       *dl_pdr;
    
    list_t          bearer_list;
    
    /* Related Context */
    pfcp_node_t     *upf_node;

    /* PCO */
    uint8_t         pco_buf[MAX_PCO_LEN];
    uint32_t        pco_len;

    /* Last SM Context Update Type */
    c_uint8_t       sm_context_update_type;
} smf_sess_t;

typedef struct _smf_bearer_t {
    lnode_t         node;           /**< A node of list_t */
    index_t         index;

    c_uint32_t      upf_s1u_teid;   /* S1U GTP-TEID */
    c_uint32_t      enb_s1u_teid;
    
    c_uint8_t       ebi;

    c_int8_t        *name;          /* PCC Rule Name */
    qos_t           qos;            /* QoS Infomration */

    /* Packet Filter Identifier Generator(1~15) */
    c_uint8_t       pf_identifier;
    /* Packet Filter List */
    list_t          pf_list;
    
    smf_sess_t      *sess;

    pkbuf_t         *gtp_pkbuf;

    union {
        /* PFCP_F_TEID_IPV4 */
        c_uint32_t addr;

        /* PFCP_F_TEID_IPV6 */
        c_uint8_t addr6[IPV6_LEN];

        /* PFCP_F_TEID_BOTH */
        struct {
            c_uint32_t addr;
            c_uint8_t addr6[IPV6_LEN];
        } both;
    };
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

typedef struct _smf_pdr_t {
    index_t         index;
    
    c_uint16_t      pdr_id;
    c_uint32_t      precedence;
    c_uint8_t       outer_header_removal;
    c_uint8_t       source_interface;
    
    smf_bearer_t    *bearer;
    smf_far_t       *far;
    smf_qer_t       *qer;
    smf_urr_t       *urr;
    
    smf_sess_t      *sess;
} smf_pdr_t;

typedef struct _smf_far_t {
    index_t         index;
    
    c_uint32_t      far_id;
    c_uint8_t       apply_action;
    c_uint8_t       destination_interface;
    smf_bearer_t    *bearer;
} smf_far_t;

typedef struct _smf_urr_t {
    index_t         index;
    
    c_uint32_t      urr_id;
} smf_urr_t;

typedef struct _smf_qer_t {
    index_t         index;
    
    c_uint32_t      qer_id;
} smf_qer_t;

typedef struct _smf_rule_t {
    c_uint8_t proto;
ED5(c_uint8_t ipv4_local:1;,
    c_uint8_t ipv4_remote:1;,
    c_uint8_t ipv6_local:1;,
    c_uint8_t ipv6_remote:1;,
    c_uint8_t reserved:4;)
    struct {
        struct {
            c_uint32_t addr[4];
            c_uint32_t mask[4];
        } local;
        struct {
            c_uint32_t addr[4];
            c_uint32_t mask[4];
        } remote;
    } ip;
    struct {
        struct {
            c_uint16_t low;
            c_uint16_t high;
        } local;
        struct {
            c_uint16_t low;
            c_uint16_t high;
        } remote;
    } port;
} smf_rule_t;

typedef struct _smf_pf_t {
    lnode_t         node;

ED3(c_uint8_t spare:2;,
    c_uint8_t direction:2;,
    c_uint8_t identifier:4;)
    smf_rule_t      rule;

    smf_bearer_t    *bearer;
} smf_pf_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_CONTEXT_H__ */

CORE_DECLARE(status_t)          smf_context_init(void);
CORE_DECLARE(status_t)          smf_context_final(void);
CORE_DECLARE(smf_context_t*)    smf_self(void);

CORE_DECLARE(status_t)          smf_context_parse_config(void);
CORE_DECLARE(status_t)          smf_context_setup_trace_module(void);

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
CORE_DECLARE(smf_bearer_t*)     smf_bearer_find_by_ebi(
                                    smf_sess_t *sess, c_uint8_t ebi);
CORE_DECLARE(smf_bearer_t*)     smf_bearer_find_by_name(
                                    smf_sess_t *sess, c_int8_t *name);
CORE_DECLARE(smf_bearer_t*)     smf_bearer_find_by_qci_arp(smf_sess_t *sess, 
                                    c_uint8_t qci,
                                    c_uint8_t priority_level,
                                    c_uint8_t pre_emption_capability,
                                    c_uint8_t pre_emption_vulnerability);
CORE_DECLARE(smf_bearer_t*)     smf_default_bearer_in_sess(smf_sess_t *sess);
CORE_DECLARE(smf_bearer_t*)     smf_bearer_first(smf_sess_t *sess);
CORE_DECLARE(smf_bearer_t*)     smf_bearer_next(smf_bearer_t *bearer);

CORE_DECLARE(smf_pdr_t*)        smf_pdr_add(smf_bearer_t *bearer);
CORE_DECLARE(status_t)          smf_pdr_remove(smf_pdr_t *pdr);
CORE_DECLARE(smf_pdr_t*)        smf_pdr_find(index_t index);
CORE_DECLARE(smf_pdr_t*)        smf_pdr_find_by_pdr_id(c_uint16_t pdr_id);
CORE_DECLARE(smf_far_t*)        smf_far_add(smf_bearer_t *bearer);
CORE_DECLARE(status_t)          smf_far_remove(smf_far_t *far);

CORE_DECLARE(smf_pf_t*)         smf_pf_add(
                                    smf_bearer_t *bearer, c_uint32_t precedence);
CORE_DECLARE(status_t )         smf_pf_remove(smf_pf_t *pf);
CORE_DECLARE(status_t )         smf_pf_remove_all(smf_bearer_t *bearer);
CORE_DECLARE(smf_pf_t*)         smf_pf_find_by_id(
                                    smf_bearer_t *smf_bearer, c_uint8_t id);
CORE_DECLARE(smf_pf_t*)         smf_pf_first(smf_bearer_t *bearer);
CORE_DECLARE(smf_pf_t*)         smf_pf_next(smf_pf_t *pf);

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


CORE_DECLARE(smf_sess_t*)       smf_sess_add_or_find_by_JsonCreateSession(create_session_t *createSession);
CORE_DECLARE(smf_sess_t*)       smf_sess_find_by_JsonUpdateSession(modify_bearer_t *pModifyBearer);
