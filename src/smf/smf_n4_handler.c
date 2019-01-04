#define TRACE_MODULE _smf_n4_handler

#include "core_debug.h"
#include "core_lib.h"
#include "core_list.h"

#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_node.h"
#include "pfcp/pfcp_path.h"

#include "smf_event.h"
#include "smf_context.h"
#include "smf_n4_handler.h"
#include "smf_n4_build.h"
#include "smf_pfcp_path.h"
#include "smf_n11_build.h"
#include "smf_json_build.h"
#include "smf_sbi_path.h"

void smf_n4_handle_heartbeat_request(
        pfcp_xact_t *xact, pfcp_heartbeat_request_t *req)
{
    d_trace(3, "[SMF] Heartbeat Request\n");

    status_t rv;
    pfcp_header_t h;
    pkbuf_t *pkbuf = NULL;

    do {
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

        pkbuf_free(pkbuf);
    }while(0);
}

void smf_n4_handle_heartbeat_response(
        pfcp_xact_t *xact, pfcp_heartbeat_response_t *rsp)
{
    status_t rv;

    d_trace(3, "[SMF] Heartbeat Response\n");
    do {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    } while(0);
}

void smf_n4_handle_association_setup_response(
        pfcp_xact_t *xact, pfcp_association_setup_response_t *rsp)
{
    status_t rv;
    d_trace(3, "[SMF] Association Setup Response\n");

    pfcp_node_id_t *node_id;
    c_uint8_t cause;

    do {
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
    } while(0);
    
    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void smf_n4_handle_association_update_request(
        pfcp_xact_t *xact, pfcp_association_update_request_t *req)
{
    status_t rv;
    d_trace(3, "[SMF] Association Update Request\n");

    do {
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
    } while(0);

    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void smf_n4_handle_association_update_response(
        pfcp_xact_t *xact, pfcp_association_update_response_t *rsp)
{
    status_t rv;
    d_trace(3, "[SMF] Association Update Response\n");

    c_uint8_t cause;

    do {
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
    } while(0);

    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void smf_n4_handle_association_release_request(
        pfcp_xact_t *xact, pfcp_association_release_request_t *req)
{
    status_t rv;
    d_trace(3, "[SMF] Association Release Request\n");

    do {
        if (!req->node_id.presence)
        {
            d_error("association_setup_request error: no Node ID");
            return;
        }

        pfcp_node_t *upf;

        upf = xact->gnode;
        pfcp_remove_node(&smf_self()->upf_n4_list, upf);
    } while(0);

    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void smf_n4_handle_association_release_response(
        pfcp_xact_t *xact, pfcp_association_release_response_t *rsp)
{
    status_t rv;
    d_trace(3, "[SMF] Association Release Response\n");

    c_uint8_t cause;
    do {
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
    } while(0);

    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void smf_n4_handle_session_establishment_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_establishment_response_t *rsp)
{
    status_t rv;
    d_trace(3, "[SMF] Session Establishment Response\n");
    pkbuf_t *pkbuf = NULL;
    pfcp_f_seid_t *up_f_seid = NULL;
    c_uint8_t cause;
    
    do {
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
            smf_n11_build_create_session_response(&pkbuf, sess);
            smf_sbi_send_sm_context_create(pkbuf);
        }
        pkbuf_free(pkbuf);
    } while(0);

    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void smf_n4_handle_session_modification_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_modification_response_t *rsp)
{
    status_t rv;

    d_trace(3, "[SMF] Session Modification Response\n");
    pkbuf_t *pkbuf = NULL;
    do {
        smf_n11_build_update_session_response(&pkbuf, sess);
        smf_sbi_send_sm_context_update(pkbuf);
        pkbuf_free(pkbuf);
    } while(0);

    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }

}

void smf_n4_handle_session_deletion_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_deletion_response_t *rsp)
{
    status_t rv;

    d_trace(3, "[SMF] Session Deletion Response\n");
    
    c_uint8_t cause;
    do {
        if (!rsp->cause.presence)
        {
            d_error("session_deletion_response error: no Cause");
            break;
        }
        cause = *((c_uint8_t*)rsp->cause.data);
        
        if (cause != PFCP_CAUSE_SUCCESS)
        {
            d_info("association_setup_response cause: %d", pfcp_cause_get_name(cause));
        } else
        {
            pkbuf_t *pkbuf = NULL;
            smf_n11_build_delete_session_response(&pkbuf, sess);
            smf_sbi_send_sm_context_release(pkbuf);
            pkbuf_free(pkbuf);
            /* Remove a smf session */
            smf_sess_remove(sess);
        }
    }while(0);
    
    if (xact) 
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");    
    }
}

void smf_n4_handle_session_report_request(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_report_request_t *req)
{
    status_t rv;

    d_trace(3, "[SMF] Session Report Request\n");
    
    pfcp_report_type_t report_type;
    do {
        if (!req->report_type.presence)
        {
            d_error("session_report_request error: no Report Type");
            break;
        }
        report_type = *(pfcp_report_type_t *)req->report_type.data;
        if (report_type.DLDR)
        {
            rv = smf_pfcp_send_session_report_response(sess);
            d_assert(rv == CORE_OK, return, "Send N4 PFCP Session Report Request Failed");
            // Send N1N3 Message to AMF to trigger paging
        }
    }while(0);
    
    if (xact)
    {
        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");    
    }
}
