#define TRACE_MODULE _upf_n4_handler

#include "core_debug.h"
#include "core_lib.h"

#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"

#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_conv.h"
#include "pfcp/pfcp_node.h"
#include "pfcp/pfcp_path.h"

#include "app/context.h"
#include "upf_event.h"
#include "upf_context.h"
#include "upf_pfcp_path.h"
#include "upf_n4_handler.h"
#include "upf_n4_build.h"

void upf_n4_handle_create_pdr(upf_sess_t *sess, tlv_create_pdr_t *create_pdr, upf_pdr_t **rt_pdr)
{
    upf_pdr_t *pdr = NULL;
    c_uint32_t far_id;
    upf_far_t *far = NULL;

    d_assert(create_pdr->pdr_id.presence, return, "Mandatory IE missing: pdr_id");
    d_assert(create_pdr->precedence.presence, return, "Mandatory IE missing: precedence");
    d_assert(create_pdr->pdi.presence, return, "Mandatory IE missing: pdi");
    d_assert(create_pdr->pdi.source_interface.presence, return, "Mandatory IE missing: pdi->source_interface");

    pdr = upf_pdr_add(sess);
    pdr->presence = ntohl(*((c_uint32_t *)create_pdr->precedence.data));
    pdr->source_interface = *((c_uint8_t *)(create_pdr->pdi.source_interface.data));

    if ((pdr->source_interface != PFCP_SRC_INTF_ACCESS) && (pdr->source_interface != PFCP_SRC_INTF_CORE))
    {
        d_assert(far, upf_pdr_remove(pdr); return, "source_interface not support");
    }

    // UL
    if (pdr->source_interface == PFCP_SRC_INTF_ACCESS)
    {
        pfcp_f_teid_t *f_teid;

        d_assert(create_pdr->pdi.local_f_teid.presence, upf_pdr_remove(pdr); return, "local_f_teid for UL PDR not found");
        f_teid = (pfcp_f_teid_t *)create_pdr->pdi.local_f_teid.data;
        pdr->upf_s1u_teid = ntohl(f_teid->teid);
        // TODO: check IP address?
        // f_teid->ipv4  f_teid->ipv6
    }

    // network_instance already parsed as APN
    // pdr->pdi.ue_ip_address already used in session add
    // pdr->pdi.sdf_filter.presence
    // pdr->pdi.application_id.presence

    if (create_pdr->outer_header_removal.presence)
    {
        pdr->out_hdr_rmv = *((c_uint8_t *)(create_pdr->outer_header_removal.data));
        if ((pdr->out_hdr_rmv != PFCP_OUTER_HDR_RMV_DESC_GTPU_IP4) &&
            (pdr->out_hdr_rmv != PFCP_OUTER_HDR_RMV_DESC_GTPU_IP6))
        {
            d_assert(0, upf_pdr_remove(pdr); return, "Outer header removal type not support");
        }
    }

    if (create_pdr->far_id.presence)
    {
        far_id = ntohl(*((c_uint32_t *)create_pdr->far_id.data));
        far = upf_far_find_by_far_id(far_id);
        d_assert(far, upf_pdr_remove(pdr); return, "FAR id not found");
        pdr->far = far;

        d_trace(10, "Create %s PDR[0x%04x] TEID[0x%08x] & FAR ID[0x%08x]",
                pdr->source_interface == PFCP_SRC_INTF_ACCESS ? "UL" : "DL",
                pdr->pdr_id, pdr->upf_s1u_teid, far_id);
    }

    if (pdr->source_interface == PFCP_SRC_INTF_ACCESS)
    {
        list_append(&sess->ul_pdr_list, pdr);
    }
    else
    {
        list_append(&sess->dl_pdr_list, pdr);
    }

    // Set pdr_id after list_append for pdr_remove to check
    pdr->pdr_id = ntohs(*((c_uint16_t *)create_pdr->pdr_id.data));
    *rt_pdr = pdr;
}

void upf_n4_handle_create_far(tlv_create_far_t *create_far, upf_far_t **rt_far)
{
    upf_far_t *far = NULL;

    d_assert(create_far->far_id.presence, return, "Mandatory IE missing: far_id");

    // Create far
    far = upf_far_add();
    far->far_id = ntohl(*((c_uint32_t *)create_far->far_id.data));
    far->apply_action = *((c_uint8_t *)(create_far->apply_action.data));
    d_assert(create_far->forwarding_parameters.presence, upf_far_remove(far); return, "Mandatory IE missing: forwarding_parameters for create_far");
    far->destination_interface = *((c_uint8_t *)create_far->forwarding_parameters.destination_interface.data);

    if (far->apply_action == PFCP_FAR_APPLY_ACTION_FORW)
    {
        // DL
        if (far->destination_interface == PFCP_FAR_DEST_INTF_ACCESS)
        {
            pfcp_outer_hdr_t *outer_hdr;
            ip_t ip;
            gtp_node_t *node = NULL;
            status_t rv;

            // Assume DL need GTP header
            d_assert(create_far->forwarding_parameters.outer_header_creation.presence, return, "Mandatory IE missing: outer_header_creation for create_far");

            outer_hdr = (pfcp_outer_hdr_t *)(create_far->forwarding_parameters.outer_header_creation.data);
            if (!(outer_hdr->gtpu_ipv4 || outer_hdr->gtpu_ipv6))
            {
                d_assert(0, upf_far_remove(far); return, "Forwarding without GTPU not supported");
            }

            far->upf_n3_teid = ntohl(outer_hdr->teid);
            rv = pfcp_outer_hdr_to_ip(outer_hdr, &ip);
            d_assert(rv == CORE_OK, upf_far_remove(far); return, "Outer hdr IP convert fail for create_far");

            node = gtp_find_node_by_ip(&upf_self()->enb_s1u_list, &ip);
            if (!node)
            {
                node = gtp_add_node_with_ip(&upf_self()->enb_s1u_list, &ip,
                                            upf_self()->gtpu_port,
                                            context_self()->parameter.no_ipv4,
                                            context_self()->parameter.no_ipv6,
                                            context_self()->parameter.prefer_ipv4);

                d_assert(node, upf_far_remove(far); return, "GTP node create fail for create_far");
                rv = gtp_client(node);
                d_assert(rv == CORE_OK, return, );
            }

            far->gnode = node;
        }
    }

    d_trace(10, "Create %s FAR ID[0x%08x]",
            far->destination_interface == PFCP_FAR_DEST_INTF_ACCESS ? "DL" : "UL",
            far->far_id);

    list_append(&upf_self()->far_list, far);
    *rt_far = far;
}

void upf_n4_handle_session_establishment_request(
    upf_sess_t *sess, pfcp_xact_t *p_xact, pfcp_session_establishment_request_t *req)
{
    upf_pdr_t *pdr;
    upf_pdr_t *pdr1;
    upf_far_t *far;
    upf_far_t *far1;

    d_assert(sess, return, "Null param");
    d_assert(p_xact, return, "Null param");
    // d_assert(p_xact->gtpbuf, return, "Null param");
    // d_assert(p_xact->gtp_xact, return, "Null param");

    if (req->create_far.presence)
    {
        upf_n4_handle_create_far(&req->create_far, &far);
    }

    if (req->create_far1.presence)
    {
        upf_n4_handle_create_far(&req->create_far1, &far1);
    }

    if (req->create_urr.presence)
    {
        // TODO
    }

    if (req->create_qer.presence)
    {
        // TODO
    }

    // Handle Create PDR last
    if (req->create_pdr.presence)
    {
        upf_n4_handle_create_pdr(sess, &req->create_pdr, &pdr);
    }

    if (req->create_pdr1.presence)
    {
        upf_n4_handle_create_pdr(sess, &req->create_pdr1, &pdr1);
    }

    {
        status_t rv;
        pfcp_header_t h;
        pkbuf_t *pkbuf = NULL;
        pfcp_f_seid_t *smf_f_seid = NULL;

        d_trace(3, "[UPF] Session Establishment Response\n");

        if (!req->cp_f_seid.presence)
        {
            d_error("Session Establishment Response: No CP F-SEID");
            return;
        }

        smf_f_seid = req->cp_f_seid.data;
        sess->smf_seid = be64toh(smf_f_seid->seid);

        /* Send Session Establishment Response */
        memset(&h, 0, sizeof(pfcp_header_t));
        h.type = PFCP_SESSION_ESTABLISHMENT_RESPONSE_TYPE;
        h.seid = sess->smf_seid;

        rv = upf_n4_build_session_establishment_response(
            &pkbuf, h.type, sess, req);
        d_assert(rv == CORE_OK, return, "N4 build error");

        rv = pfcp_xact_update_tx(p_xact, &h, pkbuf);
        d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");

        rv = pfcp_xact_commit(p_xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void upf_n4_handle_session_modification_request(
    upf_sess_t *sess, pfcp_xact_t *xact, pfcp_session_modification_request_t *req)
{
    d_assert(sess, return, "Null param");
    d_assert(xact, return, "Null param");

    status_t rv;
    pfcp_header_t h;
    pkbuf_t *pkbuf = NULL;

    /* Update PDR */
    if (req->update_pdr.presence)
    {
        upf_pdr_t *target_pdr = upf_pdr_find_by_pdr_id(&req->update_pdr.pdr_id);
        if (target_pdr)
        {
            /* Update PDR Precedence */
            memcpy(&target_pdr->presence, (c_uint32_t *)&req->update_pdr.precedence.data, sizeof(c_uint32_t));
        }
        else
        {
            /* PDR not found in context */
            d_assert(0, , "[UPF] Session Modification: update_pdr not found in context");
        }
    }

    /* Update FAR */
    if (req->update_far.presence)
    {
        upf_far_t *target_far = upf_far_find_by_far_id(ntohl(*(c_uint32_t *)req->update_far.far_id.data));
        if (target_far)
        {
            /* Update eNB S1U-TEID */
            if (req->update_far.update_forwarding_parameters.presence)
            {
                ip_t ip;
                gtp_node_t *node = NULL;
                pfcp_outer_hdr_t *outer_header;
                outer_header = req->update_far.update_forwarding_parameters.outer_header_creation.data;
                target_far->upf_n3_teid = ntohl(outer_header->teid);
                rv = pfcp_outer_hdr_to_ip(outer_header, &ip);
                d_assert(rv == CORE_OK, upf_far_remove(target_far); return, "Outer hdr IP convert fail for crdate_far");

                node = gtp_find_node_by_ip(&upf_self()->enb_s1u_list, &ip);
                if (!node)
                {
                    node = gtp_add_node_with_ip(&upf_self()->enb_s1u_list, &ip,
                                                upf_self()->gtpu_port,
                                                context_self()->parameter.no_ipv4,
                                                context_self()->parameter.no_ipv6,
                                                context_self()->parameter.prefer_ipv4);

                    d_assert(node, upf_far_remove(target_far); return, "GTP node create fail for crdate_far");
                    rv = gtp_client(node);
                    d_assert(rv == CORE_OK, return, );
                }
                target_far->gnode = node;
            }

            if (req->update_far.apply_action.presence)
            {
                target_far->apply_action = *(c_int8_t *)req->update_far.apply_action.data;
            }
        }
        else
        {
            /* FAR not found in context */
            d_assert(0, , "[UPF] Session Modification: update_far not found in context");
        }
    }

    /* Send Session Modification Response */
    d_trace(3, "[UPF] Session Modification Response\n");
    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_SESSION_MODIFICATION_RESPONSE_TYPE;
    h.seid = sess->smf_seid;

    rv = upf_n4_build_session_modification_response(
        &pkbuf, h.type, sess, req);
    d_assert(rv == CORE_OK, return, "N4 build error");

    rv = pfcp_xact_update_tx(xact, &h, pkbuf);
    d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");
}

void upf_n4_handle_session_deletion_request(
    upf_sess_t *sess, pfcp_xact_t *xact, pfcp_session_deletion_request_t *req)
{
    d_assert(sess, return, "Null param");
    d_assert(xact, return, "Null param");

    {
        status_t rv;
        pfcp_header_t h;
        pkbuf_t *pkbuf = NULL;

        /* Send Session Deletion Response */
        d_trace(3, "[UPF] Session Deletion Response\n");
        memset(&h, 0, sizeof(pfcp_header_t));
        h.type = PFCP_SESSION_DELETION_RESPONSE_TYPE;
        h.seid = sess->smf_seid;

        rv = upf_n4_build_session_deletion_response(
            &pkbuf, h.type, sess, req);
        d_assert(rv == CORE_OK, return, "N4 build error");

        rv = pfcp_xact_update_tx(xact, &h, pkbuf);
        d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");

        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }

    upf_sess_remove(sess);
}

void upf_n4_handle_session_report_response(
    upf_sess_t *sess, pfcp_xact_t *xact, pfcp_session_report_response_t *req)
{
    status_t rv;

    d_assert(sess, return, "Null param");
    d_assert(xact, return, "Null param");
    d_assert(req->cause.presence, return, "session_report_response error: no Cause");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");
}

void upf_n4_handle_association_setup_request(
    pfcp_xact_t *xact, pfcp_association_setup_request_t *req)
{
    pfcp_node_id_t *node_id;

    d_assert(xact, return, "association_setup_request error");
    d_assert(xact->gnode, return, "association_setup_request error");
    d_assert(req->node_id.presence, return, "association_setup_request error: no node_id");

    node_id = ((pfcp_node_id_t *)req->node_id.data);
    // (pfcp_node_id_t*) req->node_id.data)->addr

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
            d_assert(0, return, "association_setup_request error: node id type");
            break;
    }

    // d_assert(req->cause.presence, return, "association_setup_request error: no cause");

    // Accept
    xact->gnode->state = PFCP_NODE_ST_ASSOCIATED;
    {
        status_t rv;
        pfcp_header_t h;
        pkbuf_t *pkbuf = NULL;

        d_trace(3, "[UPF] Association Setup Response\n");

        /* Send Association Setup Response */
        memset(&h, 0, sizeof(pfcp_header_t));
        h.type = PFCP_ASSOCIATION_SETUP_RESPONSE_TYPE;
        h.seid = 0;

        rv = upf_n4_build_association_setup_response(
            &pkbuf, h.type);
        d_assert(rv == CORE_OK, return, "N4 build error");

        rv = pfcp_xact_update_tx(xact, &h, pkbuf);
        d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");

        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }

    // req->recovery_time_stamp;
    // req->up_function_features;
    // req->user_plane_ip_resource_information;
}

void upf_n4_handle_association_update_request(
    pfcp_xact_t *xact, pfcp_association_update_request_t *req)
{
}

void upf_n4_handle_association_release_request(
    pfcp_xact_t *xact, pfcp_association_release_request_t *req)
{
    // Release all session in this association !!
    hash_index_t *hi = NULL;
    upf_sess_t *sess = NULL;

    for (hi = upf_sess_first(); hi; hi = upf_sess_next(hi))
    {
        sess = upf_sess_this(hi);
        if (sess->pnode == xact->gnode)
        {
            upf_sess_remove(sess);
        }
    }

    {
        status_t rv;
        pfcp_header_t h;
        pkbuf_t *pkbuf = NULL;

        d_trace(3, "[UPF] Association Release Response\n");

        /* Send Association Release Response */
        memset(&h, 0, sizeof(pfcp_header_t));
        h.type = PFCP_ASSOCIATION_RELEASE_RESPONSE_TYPE;
        h.seid = 0;

        rv = upf_n4_build_association_release_response(
            &pkbuf, h.type);
        d_assert(rv == CORE_OK, return, "N4 build error");

        rv = pfcp_xact_update_tx(xact, &h, pkbuf);
        d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");

        rv = pfcp_xact_commit(xact);
        d_assert(rv == CORE_OK, return, "xact_commit error");
    }
}

void upf_n4_handle_heartbeat_request(
    pfcp_xact_t *xact, pfcp_heartbeat_request_t *req)
{
    status_t rv;
    pfcp_header_t h;
    pkbuf_t *pkbuf = NULL;

    d_trace(3, "[UPF] Heartbeat Request\n");

    /* Send Heartbeat Response */
    memset(&h, 0, sizeof(pfcp_header_t));
    h.type = PFCP_HEARTBEAT_RESPONSE_TYPE;
    h.seid = 0;

    rv = upf_n4_build_heartbeat_response(
        &pkbuf, h.type);
    d_assert(rv == CORE_OK, return, "N4 build error");

    rv = pfcp_xact_update_tx(xact, &h, pkbuf);
    d_assert(rv == CORE_OK, return, "pfcp_xact_update_tx error");

    rv = pfcp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");
}

void upf_n4_handle_heartbeat_response(
    pfcp_xact_t *xact, pfcp_heartbeat_response_t *req)
{
    d_trace(3, "[UPF] Heartbeat Response\n");
    // Nothing to do
}
