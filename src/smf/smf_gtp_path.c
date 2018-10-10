#define TRACE_MODULE _smf_gtp_path

#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"

#include "smf_context.h"
#include "smf_event.h"
#include "smf_gtp_path.h"
#include "smf_sm.h"

static int _gtpv2_c_recv_cb(sock_id sock, void *data)
{
    event_t e;
    status_t rv;
    pkbuf_t *pkbuf = NULL;

    d_assert(sock, return -1, "Null param");

    rv = gtp_recv(sock, &pkbuf);
    if (rv != CORE_OK)
    {
        if (errno == EAGAIN)
            return 0;

        return -1;
    }

    event_set(&e, SMF_EVT_S11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    rv = smf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("mme_event_send error");
        pkbuf_free(pkbuf);
        return 0;
    }

    return 0;
}

status_t smf_gtp_open()
{
    status_t rv;

    rv = gtp_server_list(&smf_self()->gtpc_list, _gtpv2_c_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    rv = gtp_server_list(&smf_self()->gtpc_list6, _gtpv2_c_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    smf_self()->gtpc_sock = gtp_local_sock_first(&smf_self()->gtpc_list);
    smf_self()->gtpc_sock6 = gtp_local_sock_first(&smf_self()->gtpc_list6);
    smf_self()->gtpc_addr = gtp_local_addr_first(&smf_self()->gtpc_list);
    smf_self()->gtpc_addr6 = gtp_local_addr_first(&smf_self()->gtpc_list6);

    d_assert(smf_self()->gtpc_addr || smf_self()->gtpc_addr6,
            return CORE_ERROR, "No GTP Server");
    
    return CORE_OK;
}

status_t smf_gtp_close()
{
    sock_delete_list(&smf_self()->gtpc_list);
    sock_delete_list(&smf_self()->gtpc_list6);
    
    return CORE_OK;
}