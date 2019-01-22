#ifndef __UPF_CONTEXT_H__
#define __UPF_CONTEXT_H__

#include "core_list.h"
#include "core_index.h"
#include "core_errno.h"
#include "core_hash.h"
#include "core_network.h"
#include "core_msgq.h"
#include "core_timer.h"

#include "gtp/gtp_types.h"
#include "gtp/gtp_message.h"

#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_message.h"
typedef struct _pfcp_node_t pfcp_node_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_NUM_OF_DEV          16
#define MAX_NUM_OF_SUBNET       16

typedef struct _gtp_node_t gtp_node_t;

typedef struct _upf_context_t {

    c_uint32_t      gtpu_port;      /* Default: UPF GTP-U local port */
    const char      *tun_ifname;    /* Default:: uptun */

    c_uint32_t      pfcp_port;      /* Default: SMF PFCP local port */
    list_t          pfcp_list;      /* SMF PFCP IPv4 Server List */
    list_t          pfcp_list6;     /* SMF PFCP IPv6 Server List */
    sock_id         pfcp_sock;      /* SMF PFCP IPv4 Socket */
    sock_id         pfcp_sock6;     /* SMF PFCP IPv6 Socket */
    c_sockaddr_t    *pfcp_addr;     /* SMF PFCP IPv4 Address */
    c_sockaddr_t    *pfcp_addr6;    /* SMF PFCP IPv6 Address */
    c_uint32_t      recovery_time;  /* the UTC time when the node started */

    list_t          gtpu_list;      /* UPF GTPU IPv4 Server List */
    list_t          gtpu_list6;     /* UPF GTPU IPv6 Server List */
    sock_id         gtpu_sock;      /* UPF GTPU IPv4 Socket */
    sock_id         gtpu_sock6;     /* UPF GTPU IPv6 Socket */
    c_sockaddr_t    *gtpu_addr;     /* UPF GTPU IPv4 Address */
    c_sockaddr_t    *gtpu_addr6;    /* UPF GTPU IPv6 Address */

    list_t          dev_list;       /* UPF Tun Device List */
    list_t          subnet_list;    /* UPF UE Subnet List */

    msgq_id         queue_id;       /* Qsesssess for processing UPF control plane */
    tm_service_t    tm_service;     /* Timer Service */

#define MAX_NUM_OF_DNS              2
    const char      *dns[MAX_NUM_OF_DNS];
    const char      *dns6[MAX_NUM_OF_DNS];

#define MAX_NUM_OF_P_CSCF           16
    const char      *p_cscf[MAX_NUM_OF_P_CSCF];
    int             num_of_p_cscf;
    int             p_cscf_index;
    const char      *p_cscf6[MAX_NUM_OF_P_CSCF];
    int             num_of_p_cscf6;
    int             p_cscf6_index;

    list_t          enb_s1u_list;  /* ENB GTPU Node List */
    list_t          upf_n4_list;  /* UPF PFCP Node List */
    list_t          far_list;
    list_t          qer_list;
    list_t          urr_list;
    list_t          ip_pool_list;

    hash_t          *sess_hash; /* hash table (IMSI+APN) */
} upf_context_t;

typedef struct _upf_subnet_t upf_subnet_t;
typedef struct _upf_ue_ip_t {
    c_uint32_t      addr[4];

    /* Related Context */
    upf_subnet_t    *subnet;
} upf_ue_ip_t;

typedef struct _upf_dev_t {
    lnode_t     node;

    c_int8_t    ifname[IFNAMSIZ];
    sock_id     sock;

    c_sockaddr_t *link_local_addr;
} upf_dev_t;

typedef struct _upf_subnet_t {
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
        upf_ue_ip_t *free[MAX_POOL_OF_SESS], pool[MAX_POOL_OF_SESS];
    } pool;

    /* Related Context */
    upf_dev_t   *dev;
} upf_subnet_t;

typedef struct _upf_pdr_t upf_pdr_t;

typedef struct _upf_sess_t {
    index_t         index;          /**< An index of this node */

    //c_uint32_t      smf_seid0;     /* SMF-SEID0 is derived from INDEX */
    //c_uint32_t      smf_seid1;     /* SMF-SEID0 is derived from IP address */
    //c_uint32_t      upf_seid0;     /* UPF-SEID0 is received from UPF */
    //c_uint32_t      upf_seid1;     /* UPF-SEID0 is received from UPF */
    c_uint64_t      smf_seid;      /* SMF-SEID0 is derived from INDEX */
    c_uint64_t      upf_seid;      /* UPF-SEID0 is received from UPF */

    /* APN Configuration */
    pdn_t           pdn;
    upf_ue_ip_t*    ipv4;
    upf_ue_ip_t*    ipv6;

    /* User-Lication-Info */
    tai_t           tai;
    e_cgi_t         e_cgi;

    /* Hash Key : IMSI+APN */
    c_uint8_t       hash_keybuf[IPV6_LEN/*MAX_IMSI_LEN*/+MAX_APN_LEN+1];
    int             hash_keylen;


    /* Related Context */
    gtp_node_t      *gnode;
    pfcp_node_t     *pnode;
    list_t          dl_pdr_list;
    list_t          ul_pdr_list;
} upf_sess_t;


typedef struct _upf_far_t upf_far_t;
typedef struct _upf_urr_t upf_urr_t;
typedef struct _upf_qer_t upf_qer_t;
typedef struct _upf_bar_t upf_bar_t;

typedef struct _upf_pdr_t {
    lnode_t         node; /**< A node of list_t */
    index_t         index;

    c_uint32_t      upf_s1u_teid;   /* UPF_S1U is derived from INDEX [from SMF now] */
    //c_uint8_t       ebi;
    c_uint8_t       ul_dl;// DL or UL PDR

    c_uint16_t      pdr_id;   /* UPF_S1U is derived from INDEX */

#define SMF_TEID_IP_DESC_IPV4  1
#define SMF_TEID_IP_DESC_IPV6  2
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

    //c_int8_t        *name;          /* PCC Rule Name */
    //qos_t           qos;            /* QoS Infomration */
    c_uint32_t      presence;
    c_uint8_t       out_hdr_rmv;
    c_uint8_t       source_interface;

    /* Packet Filter Identifier Generator(1~15) */
    //c_uint8_t       pf_identifier;
    /* Packet Filter List */
    //list_t          pf_list;
    upf_far_t      *far;
    upf_qer_t      *qer;
    upf_urr_t      *urr;
    
    pfcp_node_t     *pnode;
    upf_sess_t      *sess;
} upf_pdr_t;


typedef struct _upf_far_t {
    lnode_t         node; /**< A node of list_t */
    index_t         index;
    c_uint32_t      far_id;
    c_uint16_t      ref_cnt;
    c_uint8_t       create;  // has created or not
    c_uint8_t       apply_action;
    c_uint8_t       destination_interface;

    c_uint32_t      upf_n3_teid;
        
    upf_bar_t       *bar;
    pfcp_node_t     *pnode;
    gtp_node_t      *gnode;
} upf_far_t;

typedef struct _upf_urr_t {
    lnode_t         node; /**< A node of list_t */
    index_t         index;
    c_uint16_t      urr_id;
    c_uint16_t      ref_cnt;
    
    pfcp_node_t     *pnode;
} upf_urr_t;

typedef struct _upf_qer_t {
    lnode_t         node; /**< A node of list_t */
    index_t         index;
    c_uint16_t      qer_id;
    c_uint16_t      ref_cnt;
    
    pfcp_node_t     *pnode;
} upf_qer_t;

typedef struct _upf_bar_t {
    lnode_t         node; /**< A node of list_t */
    index_t         index;
    c_uint16_t      bar_id;
    c_uint16_t      ref_cnt;
    
    pfcp_node_t     *pnode;
} upf_bar_t;

typedef struct _upf_rule_t {
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
} upf_rule_t;

typedef struct _upf_pf_t {
    lnode_t         node;

ED3(c_uint8_t spare:2;,
    c_uint8_t direction:2;,
    c_uint8_t identifier:4;)
    upf_rule_t      rule;

    //upf_bearer_t    *bearer;
} upf_pf_t;

CORE_DECLARE(status_t)      upf_context_init(void);
CORE_DECLARE(status_t)      upf_context_final(void);
CORE_DECLARE(upf_context_t*) upf_self(void);

CORE_DECLARE(status_t)      upf_context_parse_config(void);
CORE_DECLARE(status_t)      upf_context_setup_trace_module(void);

CORE_DECLARE(upf_sess_t *)  upf_sess_add_by_message(pfcp_message_t *message);
CORE_DECLARE(upf_sess_t*)   upf_sess_add(
        pfcp_ue_ip_addr_t *ue_ip, c_int8_t *apn, 
        c_uint8_t pdn_type);        
CORE_DECLARE(status_t )     upf_sess_remove(upf_sess_t *sess);
CORE_DECLARE(status_t )     upf_sess_remove_all();
CORE_DECLARE(upf_sess_t*)   upf_sess_find(index_t index);
CORE_DECLARE(upf_sess_t*)   upf_sess_find_by_teid(c_uint32_t teid);
CORE_DECLARE(upf_sess_t*)   upf_sess_find_by_seid(c_uint64_t seid);
CORE_DECLARE(upf_sess_t*)   upf_sess_find_by_imsi_apn(
        c_uint8_t *imsi, int imsi_len, c_int8_t *apn);
CORE_DECLARE(hash_index_t *)  upf_sess_first();
CORE_DECLARE(hash_index_t *)  upf_sess_next(hash_index_t *hi);
CORE_DECLARE(upf_sess_t *)  upf_sess_this(hash_index_t *hi);

CORE_DECLARE(status_t )     upf_ue_pool_generate();
CORE_DECLARE(upf_ue_ip_t *) upf_ue_ip_alloc(int family, const char *apn);
CORE_DECLARE(status_t)      upf_ue_ip_free(upf_ue_ip_t *ip);

CORE_DECLARE(upf_dev_t*)    upf_dev_add(const char *ifname);
CORE_DECLARE(status_t )     upf_dev_remove(upf_dev_t *dev);
CORE_DECLARE(status_t )     upf_dev_remove_all();
CORE_DECLARE(upf_dev_t*)    upf_dev_find_by_ifname(const char *ifname);
CORE_DECLARE(upf_dev_t*)    upf_dev_first();
CORE_DECLARE(upf_dev_t*)    upf_dev_next(upf_dev_t *dev);

CORE_DECLARE(upf_subnet_t*) upf_subnet_add(
        const char *ipstr, const char *mask_or_numbits,
        const char *apn, const char *ifname);
CORE_DECLARE(status_t )     upf_subnet_remove(upf_subnet_t *subnet);
CORE_DECLARE(status_t )     upf_subnet_remove_all();
CORE_DECLARE(upf_subnet_t*) upf_subnet_first();
CORE_DECLARE(upf_subnet_t*) upf_subnet_next(upf_subnet_t *subnet);

CORE_DECLARE(upf_pdr_t*)   upf_pdr_add(upf_sess_t *sess);
CORE_DECLARE(status_t)     upf_pdr_remove(upf_pdr_t *pdr);
CORE_DECLARE(upf_pdr_t*)   upf_default_pdr_in_sess(upf_sess_t *sess);
CORE_DECLARE(upf_pdr_t*)   upf_dl_pdr_first(upf_sess_t *sess);
CORE_DECLARE(upf_pdr_t*)   upf_pdr_next(upf_pdr_t *pdr);
CORE_DECLARE(upf_pdr_t*)   upf_pdr_find(index_t index);
CORE_DECLARE(upf_pdr_t*)   upf_pdr_find_by_pdr_id(tlv_packet_detection_rule_id_t *pdr_id);
CORE_DECLARE(upf_pdr_t*)   upf_pdr_find_by_upf_s1u_teid(c_uint32_t teid);
CORE_DECLARE(upf_far_t*)   upf_far_add(void);
CORE_DECLARE(status_t)     upf_far_remove(upf_far_t *far);
CORE_DECLARE(upf_far_t*)   upf_far_find_by_far_id(c_uint32_t far_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UPF_CONTEXT_H__ */
