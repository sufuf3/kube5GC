#define TRACE_MODULE _smf_n4_handler

#include "core_debug.h"
#include "core_lib.h"
#include "core_list.h"

#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_node.h"
#include "pfcp/pfcp_path.h"

#include "gtp/gtp_xact.h"

#include "smf_event.h"
#include "smf_context.h"
#include "smf_n4_handler.h"
#include "smf_n4_build.h"
#include "smf_pfcp_path.h"
#include "smf_s11_build.h"
#include "smf_json_build.h"
#include "smf_sbi_path.h"

void test_session()
{
    c_uint8_t imsi[MAX_IMSI_LEN + 1];
    c_uint8_t pdn_type = GTP_PDN_TYPE_IPV4;
    char imsi_bcd[MAX_IMSI_BCD_LEN + 1];
    char apn[MAX_APN_LEN] = "internet";
    int imsi_len;
    strcpy(imsi_bcd, "10052364786");
    core_bcd_to_buffer(imsi_bcd, imsi, &imsi_len);
    smf_sess_t *sess = smf_sess_add(imsi, imsi_len, apn, pdn_type, 1);
    smf_pfcp_send_session_establishment_request(sess);
    smf_pfcp_send_session_modification_request(sess);
    smf_pfcp_send_session_deletion_request(sess);
}

void smf_n4_handle_heartbeat_request(
        pfcp_xact_t *xact, pfcp_heartbeat_request_t *req)
{
    d_trace(3, "[SMF] Heartbeat Request\n");

    status_t rv;
    pfcp_header_t h;
    pkbuf_t *pkbuf = NULL;

    /* Send Heartbeat Response */
    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_HEARTBEAT_RESPONSE_TYPE;
    h.seid = 0;

    rv = smf_n4_build_heartbeat_response(
            &pkbuf);
    d_assert(rv == CORE_OK, return, "pfcp build error");

    rv = pfcp_xact_update_tx(xact, &h, pkbuf);
    d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");
    
    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");
}

void smf_n4_handle_heartbeat_response(
        pfcp_xact_t *xact, pfcp_heartbeat_response_t *rsp)
{
    d_trace(3, "[SMF] Heartbeat Response\n");
}

void smf_n4_handle_association_setup_response(
        pfcp_xact_t *xact, pfcp_association_setup_response_t *rsp)
{
    d_trace(3, "[SMF] Association Setup Response\n");

    pfcp_node_id_t *node_id;
    c_uint8_t cause;

    if (!rsp->node_id.presence)
    {
        d_error("association_setup_response error: no Node ID");
        return;
    }

    if (!rsp->cause.presence)
    {
        d_error("association_setup_response error: no Cause");
        return;
    }

    node_id = ((pfcp_node_id_t*)rsp->node_id.data);
    xact->gnode->node_id.type = node_id->type;
    switch (node_id->type)
    {
        case PFCP_NODE_ID_IPV4:
            xact->gnode->node_id.addr = node_id->addr;
            break;
        case PFCP_NODE_ID_IPV6:
            memcpy(xact->gnode->node_id.addr6, node_id->addr6, IPV6_LEN);
            break;
        default:
            d_error("association_setup_response error: invalid Node ID");
            return;
            break;
    }

    cause = *((c_uint8_t*)rsp->cause.data);
    if (cause != PFCP_CAUSE_SUCCESS)
    {
        d_info("association_setup_response cause: %d", pfcp_cause_get_name(cause));
    }
}

void smf_n4_handle_association_update_request(
        pfcp_xact_t *xact, pfcp_association_update_request_t *req)
{
    d_trace(3, "[SMF] Association Update Request\n");

    pfcp_node_t *upf = list_first(&smf_self()->upf_n4_list);

    if (!req->node_id.presence)
    {
        d_error("association_setup_request error: no Node ID");
        return;
    }

    if (req->up_function_features.presence)
    {
        c_uint8_t up_function_features = *((c_uint8_t*)req->up_function_features.data);
        upf->up_function_features = up_function_features;
    }
}

void smf_n4_handle_association_update_response(
        pfcp_xact_t *xact, pfcp_association_update_response_t *rsp)
{
    d_trace(3, "[SMF] Association Update Response\n");

    c_uint8_t cause;

    if (!rsp->node_id.presence)
    {
        d_error("association_setup_response error: no Node ID");
        return;
    }

    if (!rsp->cause.presence)
    {
        d_error("association_setup_response error: no Cause");
        return;
    }

    cause = *((c_uint8_t*)rsp->cause.data);
    if (cause != PFCP_CAUSE_SUCCESS)
    {
        d_info("association_setup_response cause: %d", pfcp_cause_get_name(cause));
    }
}

void smf_n4_handle_association_release_request(
        pfcp_xact_t *xact, pfcp_association_release_request_t *req)
{
    d_trace(3, "[SMF] Association Release Request\n");

    if (!req->node_id.presence)
    {
        d_error("association_setup_request error: no Node ID");
        return;
    }

    pfcp_node_t *upf;

    upf = xact->gnode;
    pfcp_remove_node(&smf_self()->upf_n4_list, upf);
}

void smf_n4_handle_association_release_response(
        pfcp_xact_t *xact, pfcp_association_release_response_t *rsp)
{
    d_trace(3, "[SMF] Association Release Response\n");

    c_uint8_t cause;

    if (!rsp->node_id.presence)
    {
        d_error("association_setup_request error: no Node ID");
        return;
    }

    if (!rsp->cause.presence)
    {
        d_error("association_setup_request error: no Cause");
        return;
    }
    
    cause = *((c_uint8_t*)rsp->cause.data);
    if (cause != PFCP_CAUSE_SUCCESS)
    {
        d_info("association_setup_response cause: %d", pfcp_cause_get_name(cause));
    } else
    {
        pfcp_node_t *upf;

        upf = xact->gnode;
        pfcp_remove_node(&smf_self()->upf_n4_list, upf);
    }
}

void smf_n4_handle_session_establishment_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_establishment_response_t *rsp)
{
    d_trace(3, "[SMF] Session Establishment Response\n");
#ifndef FIVE_G_CORE
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    gtp_header_t h;
    pfcp_f_seid_t *up_f_seid = NULL;
    c_uint8_t cause;
    
    if (!rsp->cause.presence)
    {
        d_error("session_establishment_response error: no Cause");
        return;
    }
    
    if (!rsp->up_f_seid.presence)
    {
        d_error("session_establishment_response error: no UP F-SEID");
        return;
    }
    
    cause = *((c_uint8_t*)rsp->cause.data);
    up_f_seid = rsp->up_f_seid.data;
    
    if (cause != PFCP_CAUSE_SUCCESS)
    {
        d_info("association_setup_response cause: %d", pfcp_cause_get_name(cause));
    } else
    {
        sess->upf_n4_seid = be64toh(up_f_seid->seid);
        memset(&h, 0, sizeof(gtp_header_t));
        h.type = GTP_CREATE_SESSION_RESPONSE_TYPE;
        h.teid = sess->mme_s11_teid;
        rv = smf_s11_build_create_session_response(
            &pkbuf, sess);
        d_assert(rv == CORE_OK, return, "gtp build error");

        rv = gtp_xact_update_tx(sess->s11_xact, &h, pkbuf);
        d_assert(rv == CORE_OK, return, "gtp_xact_update_tx error");

        rv = gtp_xact_commit(sess->s11_xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
#else
    pkbuf_t *pkbuf = NULL;
    smf_n11_build_create_session_response(&pkbuf, sess);
    smf_sbi_send_sm_context_update(pkbuf);
#endif
}

void smf_n4_handle_session_modification_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_modification_response_t *rsp)
{
    d_trace(3, "[SMF] Session Modification Response\n");
    
    if (!rsp->cause.presence)
    {
        d_error("session_modification_response error: no Cause");
        return;
    }
}

void smf_n4_handle_session_deletion_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_deletion_response_t *rsp)
{
    d_trace(3, "[SMF] Session Deletion Response\n");
    
    c_uint8_t cause;
    
    if (!rsp->cause.presence)
    {
        d_error("session_deletion_response error: no Cause");
        return;
    }
    cause = *((c_uint8_t*)rsp->cause.data);
    if (cause != PFCP_CAUSE_SUCCESS)
    {
        d_info("association_setup_response cause: %d", pfcp_cause_get_name(cause));
    } else
    {
        
    }
}
