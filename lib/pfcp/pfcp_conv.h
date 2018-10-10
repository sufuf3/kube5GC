#ifndef __PFCP_CONV_H__
#define __PFCP_CONV_H__

#include "core_errno.h"
#include "pfcp_types.h"

typedef struct _tlv_bearer_context_t tlv_bearer_context_t;
typedef struct _pfcp_create_indirect_data_forwarding_tunnel_request_t pfcp_create_indirect_data_forwarding_tunnel_request_t;
typedef struct _pfcp_create_indirect_data_forwarding_tunnel_response_t pfcp_create_indirect_data_forwarding_tunnel_response_t;
typedef struct c_sockaddr_t c_sockaddr_t;
typedef struct _pfcp_f_seid_t pfcp_f_seid_t;
typedef struct _ip_t ip_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PFCP_MAX_NUM_OF_INDIRECT_TUNNEL      11

/* Create Indirect Data Forwarding Tunnel Request/Response */
CORE_DECLARE(void) pfcp_bearers_in_create_indirect_tunnel_request(
    tlv_bearer_context_t *bearers[][PFCP_MAX_NUM_OF_INDIRECT_TUNNEL],
    pfcp_create_indirect_data_forwarding_tunnel_request_t *req);
CORE_DECLARE(void) pfcp_bearers_in_create_indirect_tunnel_response(
    tlv_bearer_context_t *bearers[][PFCP_MAX_NUM_OF_INDIRECT_TUNNEL],
    pfcp_create_indirect_data_forwarding_tunnel_response_t *rsp);

CORE_DECLARE(status_t) pfcp_f_seid_to_sockaddr(
    pfcp_f_seid_t *f_seid, c_uint16_t port, c_sockaddr_t **list);
CORE_DECLARE(status_t) pfcp_sockaddr_to_f_seid(
    c_sockaddr_t *addr, c_sockaddr_t *addr6, pfcp_f_seid_t *f_seid, int *len);
CORE_DECLARE(status_t) pfcp_f_seid_to_ip(pfcp_f_seid_t *f_seid, ip_t *ip);
CORE_DECLARE(status_t) pfcp_ip_to_f_seid(
        ip_t *ip, pfcp_f_seid_t *f_seid, int *len);
CORE_DECLARE(status_t) pfcp_ip_to_f_teid(
        ip_t *ip, pfcp_f_teid_t *f_teid, int *len);
CORE_DECLARE(status_t) pfcp_sockaddr_to_f_teid(
    c_sockaddr_t *addr, c_sockaddr_t *addr6, pfcp_f_teid_t *f_teid, int *len);
CORE_DECLARE(status_t) pfcp_outer_hdr_to_ip(pfcp_outer_hdr_t *outer_hdr, ip_t *ip);        
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PFCP_CONV_H__ */

