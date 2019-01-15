#define TRACE_MODULE _smf_pfcp_path

#include "3gpp_types.h"
#include "pfcp/pfcp_message.h"
#include "pfcp/pfcp_node.h"
#include "pfcp/pfcp_path.h"
#include "pfcp/pfcp_conv.h"

#include "smf_pfcp_path.h"
#include "smf_n4_build.h"
#include "smf_context.h"
#include "smf_event.h"

static int _smf_pfcp_recv_cb(sock_id sock, void *data)
{
    event_t e;
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;
    pfcp_node_t *upf;
    pfcp_header_t *pfcp_h = NULL;

    rv = pfcp_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    pfcp_h = (pfcp_header_t*)pkbuf->payload;
    // Check version
    if (pfcp_h->version > PFCP_VERSION)
    {
        unsigned char v_fail[8];
        pfcp_header_t *pfcp_out = (pfcp_header_t *)v_fail;
        
        d_info("Unsupported PFCP version: %d", pfcp_h->version);
        pfcp_out->flags = (PFCP_VERSION << 5); /* set version */
        pfcp_out->type = PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE;
        pfcp_out->length = htons(4);
        pfcp_out->sqn_only = pfcp_h->sqn_only;
        core_sendto(sock, v_fail, 8, 0, &from);
        pkbuf_free(pkbuf);
        return 0;
    }

    upf = pfcp_find_node_sockaddr(&smf_self()->upf_n4_list, &from);
    d_assert(upf, return -1, "upf node not found");

    event_set(&e, SMF_EVT_N4_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, (c_uintptr_t)upf);
    rv = smf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("smf_event_send error");
        pkbuf_free(pkbuf);
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t smf_pfcp_open()
{
    status_t rv;

    rv = pfcp_server_list(&smf_self()->pfcp_list, _smf_pfcp_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    rv = pfcp_server_list(&smf_self()->pfcp_list6, _smf_pfcp_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    smf_self()->pfcp_sock = pfcp_local_sock_first(&smf_self()->pfcp_list);
    smf_self()->pfcp_sock6 = pfcp_local_sock_first(&smf_self()->pfcp_list6);

    smf_self()->pfcp_addr = pfcp_local_addr_first(&smf_self()->pfcp_list);
    smf_self()->pfcp_addr6 = pfcp_local_addr_first(&smf_self()->pfcp_list6);
    d_assert(smf_self()->pfcp_addr || smf_self()->pfcp_addr6,
            return CORE_ERROR, "No PFCP Server");

    if (smf_self()->pfcp_addr)
    {
        smf_self()->pfcp_node_id.type = PFCP_NODE_ID_IPV4;
        smf_self()->pfcp_node_id.addr = smf_self()->pfcp_addr->sin.sin_addr.s_addr;
    }

    if (smf_self()->pfcp_addr6)
    {
        smf_self()->pfcp_node_id6.type = PFCP_NODE_ID_IPV6;
        memcpy(smf_self()->pfcp_node_id6.addr6, 
                smf_self()->pfcp_addr6->sin6.sin6_addr.s6_addr, IPV6_LEN);
    }

    pfcp_node_t *upf;
    for (upf = list_first(&smf_self()->upf_n4_list); upf; upf = list_next(upf)) {
        rv = pfcp_connect(smf_self()->pfcp_sock, smf_self()->pfcp_sock6, upf);
        d_assert(rv == CORE_OK, continue, "error connecting upf");
        smf_pfcp_send_association_setup_request(upf);
    }

    return CORE_OK;
}

status_t smf_pfcp_close()
{
    sock_delete_list(&smf_self()->pfcp_list);
    sock_delete_list(&smf_self()->pfcp_list6);

    pfcp_node_t *upf;
    for (upf = list_first(&smf_self()->upf_n4_list); upf; upf = list_next(upf)) {
        if (upf->state == PFCP_NODE_ST_ASSOCIATED)
            smf_pfcp_send_association_release_request(upf);
    }

    pfcp_remove_all_nodes(&smf_self()->upf_n4_list);
    return CORE_OK;
}

status_t smf_pfcp_send_heartbeat_request(pfcp_node_t *pnode)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_HEARTBEAT_REQUEST_TYPE;
    h.seid = 0;

    rv = smf_n4_build_heartbeat_request(&pkbuf);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(pnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");
	pkbuf_free(pkbuf);

    return CORE_OK;
}

status_t smf_pfcp_send_association_setup_request(pfcp_node_t *pnode)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_ASSOCIATION_SETUP_REQUEST_TYPE;
    h.seid = 0;

    rv = smf_n4_build_association_setup_request(&pkbuf);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(pnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");
	pkbuf_free(pkbuf);

    return CORE_OK;
}

status_t smf_pfcp_send_association_update_request(
        pfcp_node_t *pnode)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_ASSOCIATION_UPDATE_REQUEST_TYPE;
    h.seid = 0;

    rv = smf_n4_build_association_update_request(&pkbuf);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(pnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}
status_t smf_pfcp_send_association_release_request(pfcp_node_t * pnode)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE;
    h.seid = 0;

    rv = smf_n4_build_association_release_request(&pkbuf);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(pnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}

status_t smf_pfcp_send_session_establishment_request(smf_sess_t *sess)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE;
    h.seid = 0;

    rv = smf_n4_build_session_establishment_request(&pkbuf, sess);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(sess->upf_node, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}

status_t smf_pfcp_send_setup_donwlink_session_modification_request(smf_sess_t *sess)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_SESSION_MODIFICATION_REQUEST_TYPE;
    h.seid = sess->upf_n4_seid;

    rv = smf_n4_build_session_modification_request_for_setup_downlink(&pkbuf, sess);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(sess->upf_node, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}

status_t smf_pfcp_send_an_release_session_modification_request(smf_sess_t *sess)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_SESSION_MODIFICATION_REQUEST_TYPE;
    h.seid = sess->upf_n4_seid;

    rv = smf_n4_build_session_modification_request_for_an_release(&pkbuf, sess);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(sess->upf_node, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}

status_t smf_pfcp_send_session_deletion_request(smf_sess_t *sess)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_SESSION_DELETION_REQUEST_TYPE;
    h.seid = sess->upf_n4_seid;

    rv = smf_n4_build_session_deletion_request(&pkbuf, sess);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(sess->upf_node, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}

status_t smf_pfcp_send_session_report_response(smf_sess_t *sess)
{
    status_t rv;

    pfcp_header_t h;
    pfcp_xact_t *xact;
    pkbuf_t *pkbuf = NULL;

    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_SESSION_REPORT_RESPONSE_TYPE;
    h.seid = sess->upf_n4_seid;

    rv = smf_n4_build_session_report_response(&pkbuf, sess);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build error");

    xact = pfcp_xact_local_create(sess->upf_node, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "pfcp xact build error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

	pkbuf_free(pkbuf);
    return CORE_OK;
}
