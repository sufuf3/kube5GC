#ifndef __SMF_CONTEXT_H__
#define __SMF_CONTEXT_H__

#include "core.h"
#include "core_list.h"
#include "core_msgq.h"
#include "core_network.h"
#include "core_timer.h"

#include "fd/fd_lib.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

    c_uint32_t          start_time;     /* Timestamp of SMF start up time */
    msgq_id             queue_id;       /* Qsesssess for processing SMF control plane */
    tm_service_t        tm_service;     /* Timer Service */
} smf_context_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_CONTEXT_H__ */

CORE_DECLARE(status_t)          smf_context_init(void);
CORE_DECLARE(status_t)          smf_context_final(void);
CORE_DECLARE(smf_context_t*)    smf_self(void);

CORE_DECLARE(status_t)          smf_context_parse_config(void);
CORE_DECLARE(status_t)          smf_context_setup_trace_module(void);

CORE_DECLARE(status_t )         smf_ue_pool_generate(void);