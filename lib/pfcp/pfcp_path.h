#ifndef __PFCP_PATH_H__
#define __PFCP_PATH_H__

#include "core_pkbuf.h"
#include "core_network.h"

typedef struct _pfcp_node_t pfcp_node_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) pfcp_server(sock_node_t *snode, sock_handler handler);
CORE_DECLARE(status_t) pfcp_client(pfcp_node_t *gnode);
CORE_DECLARE(status_t) pfcp_connect(
        sock_id ipv4, sock_id ipv6, pfcp_node_t *gnode);

CORE_DECLARE(status_t) pfcp_server_list(list_t *list, sock_handler handler);

CORE_DECLARE(sock_id) pfcp_local_sock_first(list_t *list);
CORE_DECLARE(c_sockaddr_t *) pfcp_local_addr_first(list_t *list);

CORE_DECLARE(status_t) pfcp_recv(sock_id sock, pkbuf_t **pkbuf);
CORE_DECLARE(status_t) pfcp_recvfrom(sock_id sock,
        pkbuf_t **pkbuf, c_sockaddr_t *from);
CORE_DECLARE(status_t) pfcp_send(pfcp_node_t *gnode, pkbuf_t *pkbuf);

CORE_DECLARE(pkbuf_t*) pfcp_handle_heartbeat_req(pkbuf_t *pkt, c_uint32_t recovery_time);
CORE_DECLARE(status_t) pfcp_start_heartbeat(pfcp_node_t *gnode, c_uint32_t recovery_time);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PFCP_PATH_H__ */
