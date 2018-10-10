#ifndef __PFCP_NODE_H__
#define __PFCP_NODE_H__

#include "core_list.h"
#include "core_network.h"
#include "core_timer.h"

#include "pfcp_types.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SETUP_PFCP_NODE(__cTX, __gNODE) \
    do { \
        d_assert((__cTX), break, "Null param"); \
        d_assert((__gNODE), break, "Null param"); \
        (__cTX)->gnode = __gNODE; \
    } while(0)

/**
 * This structure represents the commonalities of PFCP node such as SGW-C, SGW-U,
 * PGW-C, PGW-U. Some of members may not be used by the specific type of node */
typedef struct _pfcp_node_t {
    lnode_t         node;           /* A node of list_t */

    c_sockaddr_t    *sa_list;       /* Socket Address List */

    sock_id         sock;           /* Socket instance */
    ip_t            ip;             /* Socket Address */

    list_t          local_list;    
    list_t          remote_list;
#define PFCP_NODE_ST_NULL           0
#define PFCP_NODE_ST_ASSOCIATED     1    
    c_uint8_t       state;          /* Association complete or not */
    tm_block_id     tm_heartbeat;
    pfcp_node_id_t  node_id;        /* target node id */
    union {
        c_uint8_t   up_function_features;
        c_uint8_t   cp_function_features;
    };
} pfcp_node_t;

CORE_DECLARE(status_t) pfcp_node_init(void);
CORE_DECLARE(status_t) pfcp_node_final(void);

CORE_DECLARE(status_t) pfcp_add_node(list_t *list, pfcp_node_t **node,
        c_sockaddr_t *sa_list, int no_ipv4, int no_ipv6, int prefer_ipv4);
CORE_DECLARE(pfcp_node_t *) pfcp_add_node_with_seid(
        list_t *list, pfcp_f_seid_t *f_seid,
        c_uint16_t port, int no_ipv4, int no_ipv6, int prefer_ipv4);
CORE_DECLARE(status_t) pfcp_remove_node(list_t *list, pfcp_node_t *node);
CORE_DECLARE(status_t) pfcp_remove_all_nodes(list_t *list);

CORE_DECLARE(pfcp_node_t *) pfcp_find_node(list_t *list, pfcp_f_seid_t *f_seid);
CORE_DECLARE(pfcp_node_t *) pfcp_find_node_sockaddr(list_t *list, c_sockaddr_t *from);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PFCP_NODE_H__ */
