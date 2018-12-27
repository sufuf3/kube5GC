#define TRACE_MODULE _amf4g_s11_handler

#include "core_debug.h"

#include "gtp/gtp_types.h"
#include "gtp/gtp_conv.h"
#include "gtp/gtp_xact.h"

#include "amf4g_event.h"
#include "amf4g_sm.h"
#include "amf4g_context.h"

#include "s1ap_path.h"
#include "amf4g_gtp_path.h"
#include "nas_path.h"
#include "amf4g_fd_path.h"

#include "amf4g_s11_build.h"
#include "amf4g_s11_handler.h"

void amf4g_s11_handle_create_session_response(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue, gtp_create_session_response_t *rsp)
{
    status_t rv;
    gtp_f_teid_t *sgw_s11_teid = NULL;
    gtp_f_teid_t *sgw_s1u_teid = NULL;

    amf4g_bearer_t *bearer = NULL;
    amf4g_sess_t *sess = NULL;
    pdn_t *pdn = NULL;
    
    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(rsp, return, "Null param");

    d_trace(3, "[AMF4G] Create Session Response\n");

    if (rsp->sender_f_teid_for_control_plane.presence == 0)
    {
        d_error("No S11 TEID");
        return;
    }
    if (rsp->pdn_address_allocation.presence == 0)
    {
        d_error("No PDN Address Allocation");
        return;
    }
    if (rsp->bearer_contexts_created.s1_u_enodeb_f_teid.presence == 0)
    {
        d_error("No S1U TEID");
        return;
    }
    if (rsp->bearer_contexts_created.presence == 0)
    {
        d_error("No Bearer");
        return;
    }
    if (rsp->bearer_contexts_created.  eps_bearer_id.presence == 0)
    {
        d_error("No EPS Bearer ID");
        return;
    }

    d_assert(amf4g_ue, return, "Null param");

    bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, 
            rsp->bearer_contexts_created.eps_bearer_id.u8);
    d_assert(bearer, return, "Null param");
    sess = bearer->sess;
    d_assert(sess, return, "Null param");
    pdn = sess->pdn;
    d_assert(pdn, return, "Null param");

    /* Control Plane(UL) : SGW-S11 */
    sgw_s11_teid = rsp->sender_f_teid_for_control_plane.data;
    amf4g_ue->sgw_s11_teid = ntohl(sgw_s11_teid->teid);

    memcpy(&pdn->paa, rsp->pdn_address_allocation.data,
            rsp->pdn_address_allocation.len);

    /* PCO */
    if (rsp->protocol_configuration_options.presence)
    {
        TLV_STORE_DATA(&sess->pgw_pco, &rsp->protocol_configuration_options);
    }

    /* Data Plane(UL) : SGW-S1U */
    sgw_s1u_teid = rsp->bearer_contexts_created.s1_u_enodeb_f_teid.data;
    bearer->sgw_s1u_teid = ntohl(sgw_s1u_teid->teid);

    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);
    d_trace(5, "    ENB_S1U_TEID[%d] SGW_S1U_TEID[%d]\n",
        bearer->enb_s1u_teid, bearer->sgw_s1u_teid);

    rv = gtp_f_teid_to_ip(sgw_s1u_teid, &bearer->sgw_s1u_ip);
    d_assert(rv == CORE_OK, return,);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");

    if (FSM_CHECK(&amf4g_ue->sm, emm_state_initial_context_setup))
    {
        rv = nas_send_attach_accept(amf4g_ue);
        d_assert(rv == CORE_OK, return, "nas_send_attach_accept failed");
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
    {
        rv = nas_send_activate_default_bearer_context_request(bearer);
        d_assert(rv == CORE_OK, return, "nas send failed");
    }
    else
        d_assert(0,, "Invalid EMM state");
}

void amf4g_s11_handle_modify_bearer_response(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue, gtp_modify_bearer_response_t *rsp)
{
    status_t rv;
    enb_ue_t *source_ue = NULL, *target_ue = NULL;

    d_assert(amf4g_ue, return, "Null param");
    d_assert(xact, goto cleanup, "Null param");
    d_assert(rsp, goto cleanup, "Null param");

    d_trace(3, "[AMF4G] Modify Bearer Response\n");
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, goto cleanup, "xact_commit error");

    GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_PATH_SWITCH,
        rv = s1ap_send_path_switch_ack(amf4g_ue);
        d_assert(rv == CORE_OK,, "s1ap send error");
    );

    GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_HANDOVER_NOTIFY,
        target_ue = amf4g_ue->enb_ue;
        d_assert(target_ue, return, "Null param");
        source_ue = target_ue->source_ue;
        d_assert(source_ue, return, "Null param");

        rv = s1ap_send_ue_context_release_command(source_ue,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_successful_handover,
                S1AP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL, 300);
        d_assert(rv == CORE_OK,, "s1ap send error");
    );

    return;

cleanup:
    GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_PATH_SWITCH,);
    GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_HANDOVER_NOTIFY,);
}

void amf4g_s11_handle_delete_session_response(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue, gtp_delete_session_response_t *rsp)
{
    status_t rv;
    amf4g_sess_t *sess = NULL;

    d_assert(amf4g_ue, return, "Null param");
    d_assert(xact, return, "Null param");
    sess = GTP_XACT_RETRIEVE_SESSION(xact);
    d_assert(sess, return, "Null param");
    d_assert(rsp, goto cleanup, "Null param");

    d_trace(3, "[AMF4G] Delete Session Response\n");
    if (rsp->cause.presence == 0)
    {
        d_error("No Cause");
        goto cleanup;
    }
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, goto cleanup, "xact_commit error");

    if (FSM_CHECK(&amf4g_ue->sm, emm_state_authentication))
    {
        if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        {
            amf4g_s6a_send_air(amf4g_ue, NULL);
        }
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_de_registered))
    {
        if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        {
            rv = nas_send_detach_accept(amf4g_ue);
            d_assert(rv == CORE_OK,, "nas_send_detach_accept failed");
        }
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
    {
        amf4g_bearer_t *bearer = amf4g_default_bearer_in_sess(sess);
        d_assert(bearer, goto cleanup, "Null param");

        if (FSM_CHECK(&bearer->sm, esm_state_pdn_will_disconnect))
        {
            rv = nas_send_deactivate_bearer_context_request(bearer);
            d_assert(rv == CORE_OK,,
                "nas_send_deactivate_bearer_context_request failed");
            
            /*
             * amf4g_sess_remove() should not be called here.
             *
             * Session will be removed if Deactivate bearer context 
             * accept is received */
            CLEAR_SGW_S1U_PATH(sess);
            return;
        }
        else
            d_assert(0,, "Invalid ESM state");
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_initial_context_setup) ||
             FSM_CHECK(&amf4g_ue->sm, emm_state_exception))
    {
        if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        {
            enb_ue_t *enb_ue = NULL;

            enb_ue = amf4g_ue->enb_ue;
            d_assert(enb_ue, goto cleanup, );

            rv = s1ap_send_ue_context_release_command(enb_ue,
                S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                S1AP_UE_CTX_REL_UE_CONTEXT_REMOVE, 0);
            d_assert(rv == CORE_OK,, "s1ap send error");
        }
    }
    else
        d_assert(0,, "Invalid EMM state");

cleanup:
    if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        CLEAR_SESSION_CONTEXT(amf4g_ue);

    amf4g_sess_remove(sess);
}

void amf4g_s11_handle_create_bearer_request(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue, gtp_create_bearer_request_t *req)
{
    status_t rv;
    amf4g_bearer_t *bearer = NULL, *default_bearer = NULL;
    amf4g_sess_t *sess = NULL;

    gtp_f_teid_t *sgw_s1u_teid = NULL;
    gtp_bearer_qos_t bearer_qos;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(req, return, "Null param");

    d_trace(3, "[AMF4G] Create Bearer Response\n");

    if (req->linked_eps_bearer_id.presence == 0)
    {
        d_error("No Linked EBI");
        return;
    }
    if (req->bearer_contexts.presence == 0)
    {
        d_error("No Bearer");
        return;
    }
    if (req->bearer_contexts.eps_bearer_id.presence == 0)
    {
        d_error("No EPS Bearer ID");
        return;
    }
    if (req->bearer_contexts.s1_u_enodeb_f_teid.presence == 0)
    {
        d_error("No GTP TEID");
        return;
    }
    if (req->bearer_contexts.bearer_level_qos.presence == 0)
    {
        d_error("No QoS");
        return;
    }
    if (req->bearer_contexts.tft.presence == 0)
    {
        d_error("No TFT");
        return;
    }

    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    sess = amf4g_sess_find_by_ebi(amf4g_ue, req->linked_eps_bearer_id.u8);
    d_assert(sess, return, 
            "No Session Context(EBI:%d)", req->linked_eps_bearer_id);

    bearer = amf4g_bearer_add(sess);
    d_assert(bearer, return, "No Bearer Context");

    /* Data Plane(UL) : SGW-S1U */
    sgw_s1u_teid = req->bearer_contexts.s1_u_enodeb_f_teid.data;
    bearer->sgw_s1u_teid = ntohl(sgw_s1u_teid->teid);
    rv = gtp_f_teid_to_ip(sgw_s1u_teid, &bearer->sgw_s1u_ip);
    d_assert(rv == CORE_OK, return,);

    /* Bearer QoS */
    d_assert(gtp_parse_bearer_qos(&bearer_qos,
        &req->bearer_contexts.bearer_level_qos) ==
        req->bearer_contexts.bearer_level_qos.len, return,);
    bearer->qos.qci = bearer_qos.qci;
    bearer->qos.arp.priority_level = bearer_qos.priority_level;
    bearer->qos.arp.pre_emption_capability =
                    bearer_qos.pre_emption_capability;
    bearer->qos.arp.pre_emption_vulnerability =
                    bearer_qos.pre_emption_vulnerability;
    bearer->qos.mbr.downlink = bearer_qos.dl_mbr;
    bearer->qos.mbr.uplink = bearer_qos.ul_mbr;
    bearer->qos.gbr.downlink = bearer_qos.dl_gbr;
    bearer->qos.gbr.uplink = bearer_qos.ul_gbr;

    /* Save Bearer TFT */
    TLV_STORE_DATA(&bearer->tft, &req->bearer_contexts.tft);

    /* Save Transaction. will be handled after EMM-attached */
    bearer->xact = xact;

    /* Before Activate DEDICATED bearer, we'll check DEFAULT bearer status */
    default_bearer = amf4g_default_bearer_in_sess(sess);
    d_assert(default_bearer, return,);

    if (/* Check if Activate Default Bearer Accept is received */
        FSM_CHECK(&default_bearer->sm, esm_state_active) &&
        /* Check if Initial Context Setup Response or 
         *          E-RAB Setup Response is received */
        AMF4G_HAVE_ENB_S1U_PATH(default_bearer))
    {
        rv = nas_send_activate_dedicated_bearer_context_request(bearer);
        d_assert(rv == CORE_OK, return,
            "nas_send_activate_dedicated_bearer_context failed");
    }
}

void amf4g_s11_handle_update_bearer_request(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue, gtp_update_bearer_request_t *req)
{
    status_t rv;
    amf4g_bearer_t *bearer = NULL;
    gtp_bearer_qos_t bearer_qos;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(req, return, "Null param");

    d_trace(3, "[AMF4G] Update Bearer Request\n");
    if (req->bearer_contexts.presence == 0)
    {
        d_error("No Bearer");
        return;
    }
    if (req->bearer_contexts.eps_bearer_id.presence == 0)
    {
        d_error("No EPS Bearer ID");
        return;
    }
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue,
            req->bearer_contexts.eps_bearer_id.u8);
    d_assert(bearer, return, 
            "No Bearer Context(EBI:%d)", req->bearer_contexts.eps_bearer_id.u8);

    /* Save Transaction. will be handled after EMM-attached */
    bearer->xact = xact;

    if (/* Check if Activate Default/Dedicated Bearer Accept is received */
        FSM_CHECK(&bearer->sm, esm_state_active) &&
        /* Check if Initial Context Setup Response or 
         *          E-RAB Setup Response is received */
        AMF4G_HAVE_ENB_S1U_PATH(bearer))
    {
        if (req->bearer_contexts.bearer_level_qos.presence == 1)
        {
            /* Bearer QoS */
            d_assert(gtp_parse_bearer_qos(&bearer_qos,
                &req->bearer_contexts.bearer_level_qos) ==
                req->bearer_contexts.bearer_level_qos.len, return,);
            bearer->qos.qci = bearer_qos.qci;
            bearer->qos.arp.priority_level = bearer_qos.priority_level;
            bearer->qos.arp.pre_emption_capability =
                            bearer_qos.pre_emption_capability;
            bearer->qos.arp.pre_emption_vulnerability =
                            bearer_qos.pre_emption_vulnerability;
            bearer->qos.mbr.downlink = bearer_qos.dl_mbr;
            bearer->qos.mbr.uplink = bearer_qos.ul_mbr;
            bearer->qos.gbr.downlink = bearer_qos.dl_gbr;
            bearer->qos.gbr.uplink = bearer_qos.ul_gbr;
        }

        if (req->bearer_contexts.tft.presence == 1)
        {
            /* Save Bearer TFT */
            TLV_STORE_DATA(&bearer->tft, &req->bearer_contexts.tft);
        }

        if (req->bearer_contexts.bearer_level_qos.presence == 1 ||
            req->bearer_contexts.tft.presence == 1)
        {
            rv = nas_send_modify_bearer_context_request(
                    bearer, 
                    req->bearer_contexts.bearer_level_qos.presence,
                    req->bearer_contexts.tft.presence);
            d_assert(rv == CORE_OK, return,
                "nas_send_deactivate_bearer_context_request failed");
        }
        else
        {
            d_warn("[IGNORE] Update Bearer Request : "
                    "Both QoS and TFT is NULL");
            //TODO SEND UPDATE BEARER RESPONSE
        }
    }
    else
    {
        if (!FSM_CHECK(&bearer->sm, esm_state_active))
        {
            d_assert(0,, "Invalid Bearer State");
        }
        else if (!AMF4G_HAVE_ENB_S1U_PATH(bearer))
        {
            d_assert(0,, "No ENB S1U PATH");
        }
        else
            d_assert(0,,);
    }

}

void amf4g_s11_handle_delete_bearer_request(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue, gtp_delete_bearer_request_t *req)
{
    status_t rv;
    amf4g_bearer_t *bearer = NULL;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(req, return, "Null param");

    d_trace(3, "[AMF4G] Delete Bearer Request\n");
    if (req->linked_eps_bearer_id.presence == 1)
    {
        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, req->linked_eps_bearer_id.u8);
        d_assert(bearer, return, 
                "No Bearer Context(EBI:%d)", req->linked_eps_bearer_id.u8);
    }
    else if (req->eps_bearer_ids.presence == 1)
    {
        bearer = amf4g_bearer_find_by_ue_ebi(
                amf4g_ue, req->eps_bearer_ids.u8);
        d_assert(bearer, return, 
                "No Bearer Context(EBI:%d)", req->eps_bearer_ids.u8);
    }
    else
    {
        d_error("No Linked EBI or EPS Bearer ID");
        return;
    }
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    /* Save Transaction. will be handled after EMM-attached */
    bearer->xact = xact;

    if (/* Check if Activate Default/Dedicated Bearer Accept is received */
        FSM_CHECK(&bearer->sm, esm_state_active) &&
        /* Check if Initial Context Setup Response or 
         *          E-RAB Setup Response is received */
        AMF4G_HAVE_ENB_S1U_PATH(bearer))
    {
        rv = nas_send_deactivate_bearer_context_request(bearer);
        d_assert(rv == CORE_OK, return,
            "nas_send_deactivate_bearer_context_request failed");
    }
    else
    {
        if (!FSM_CHECK(&bearer->sm, esm_state_active))
        {
            d_assert(0,, "Invalid Bearer State");
        }
        else if (!AMF4G_HAVE_ENB_S1U_PATH(bearer))
        {
            d_assert(0,, "No ENB S1U PATH");
        }
        else
            d_assert(0,,);
    }

}

void amf4g_s11_handle_release_access_bearers_response(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue,
        gtp_release_access_bearers_response_t *rsp)
{
    status_t rv;
    enb_ue_t *enb_ue = NULL;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(rsp, return, "Null param");

    d_trace(3, "[AMF4G] Release Access Bearers Response\n");
    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return, "Null param");

    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK,, "xact_commit error");

    if (rsp->cause.presence == 0)
    {
        d_error("No Cause");
        return;
    }

    rv = CLEAR_BEARER_CONTEXT(amf4g_ue);
    d_assert(rv == CORE_OK,, "AMF4G_BEARER_SET_INACTIVE failed");

    rv = s1ap_send_ue_context_release_command(enb_ue,
            S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
            S1AP_UE_CTX_REL_S1_NORMAL_RELEASE, 0);
    d_assert(rv == CORE_OK,, "s1ap send error");
}

void amf4g_s11_handle_downlink_data_notification(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue,
        gtp_downlink_data_notification_t *noti)
{
    status_t rv;
    gtp_header_t h;
    pkbuf_t *s11buf = NULL;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(noti, return, "Null param");

    d_trace(3, "[AMF4G] Downlink Data Notification\n");
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    /* Build Downlink data notification ack */
    memset(&h, 0, sizeof(gtp_header_t));
    h.type = GTP_DOWNLINK_DATA_NOTIFICATION_ACKNOWLEDGE_TYPE;
    h.teid = amf4g_ue->sgw_s11_teid;

    rv = amf4g_s11_build_downlink_data_notification_ack(&s11buf, h.type);
    d_assert(rv == CORE_OK, return, "S11 build error");

    rv = gtp_xact_update_tx(xact, &h, s11buf);
    d_assert(rv == CORE_OK, return, "xact_update_tx error");

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");
}

void amf4g_s11_handle_create_indirect_data_forwarding_tunnel_response(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue,
        gtp_create_indirect_data_forwarding_tunnel_response_t *rsp)
{
    status_t rv;
    amf4g_bearer_t *bearer = NULL;
    enb_ue_t *source_ue = NULL;
    int i;

    tlv_bearer_context_t *bearers[GTP_MAX_NUM_OF_INDIRECT_TUNNEL];
    gtp_f_teid_t *teid = NULL;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(rsp, return, "Null param");

    source_ue = amf4g_ue->enb_ue;
    d_assert(source_ue, return, "Null param");

    d_trace(3, "[AMF4G] Create Indirect Data Forwarding Tunnel Response\n");
    if (rsp->cause.presence == 0)
    {
        d_error("No Cause");
        return;
    }
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");

    gtp_bearers_in_create_indirect_tunnel_response(&bearers, rsp);

    for (i = 0; bearers[i]->presence; i++)
    {
        if (bearers[i]->eps_bearer_id.presence == 0)
        {
            d_error("No EBI");
            return;
        }

        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, 
                    bearers[i]->eps_bearer_id.u8);
        d_assert(bearer, return, "No Bearer Context");

        if (bearers[i]->s4_u_sgsn_f_teid.presence)
        {
            teid = bearers[i]->s4_u_sgsn_f_teid.data;
            d_assert(teid, return,);

            bearer->sgw_dl_teid = ntohl(teid->teid);
            rv = gtp_f_teid_to_ip(teid, &bearer->sgw_dl_ip);
            d_assert(rv == CORE_OK, return,);
        }
        if (bearers[i]->s2b_u_epdg_f_teid_5.presence)
        {
            teid = bearers[i]->s2b_u_epdg_f_teid_5.data;
            d_assert(teid, return,);

            bearer->sgw_ul_teid = ntohl(teid->teid);
            rv = gtp_f_teid_to_ip(teid, &bearer->sgw_ul_ip);
            d_assert(rv == CORE_OK, return,);
        }
    }

    rv = s1ap_send_handover_command(source_ue);
    d_assert(rv == CORE_OK,, "s1ap send error");
}

void amf4g_s11_handle_delete_indirect_data_forwarding_tunnel_response(
        gtp_xact_t *xact, amf4g_ue_t *amf4g_ue,
        gtp_delete_indirect_data_forwarding_tunnel_response_t *rsp)
{
    status_t rv;

    d_assert(xact, return, "Null param");
    d_assert(amf4g_ue, return, "Null param");
    d_assert(rsp, return, "Null param");

    d_trace(3, "[AMF4G] Delete Indirect Data Forwarding Tunnel Response\n");
    if (rsp->cause.presence == 0)
    {
        d_error("No Cause");
        return;
    }
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK,, "xact_commit error");

    rv = amf4g_ue_clear_indirect_tunnel(amf4g_ue);
    d_assert(rv == CORE_OK,, "amf4g_ue_clear_indirect_tunnel() failed");
}

void amf_n11_handle_create_session_response(
        amf4g_ue_t *amf4g_ue, create_session_t *pCreateSession)
{
#if 1
    status_t rv;

    amf4g_bearer_t *bearer = NULL;
    amf4g_sess_t *sess = NULL;
    pdn_t *pdn = NULL;
    
    d_assert(amf4g_ue, return, "Null param");
    d_assert(pCreateSession, return, "Null param");

    d_trace(3, "[AMF4G] Create Session Response\n");

    d_assert(amf4g_ue, return, "Null param");

    bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, pCreateSession->ebi);
    d_assert(bearer, return, "Null param");
    sess = bearer->sess;
    d_assert(sess, return, "Null param");
    pdn = sess->pdn;
    d_assert(pdn, return, "Null param");

    memcpy(&pdn->paa, &pCreateSession->pdn.paa, sizeof(pCreateSession->pdn.paa));
    d_info("PAA dst addr: %d", pdn->paa.addr);
    
    /* PCO */
    sess->pgw_pco.data = core_calloc(pCreateSession->ue_pco.length, sizeof(c_uint8_t));
    memcpy(sess->pgw_pco.data, pCreateSession->ue_pco.buffer, pCreateSession->ue_pco.length);
    sess->pgw_pco.len = pCreateSession->ue_pco.length;
    sess->pgw_pco.presence = 1;

    /* Data Plane(UL) : SGW-S1U */
    bearer->sgw_s1u_teid = pCreateSession->sgw_s1u_teid;

    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);
    d_trace(5, "    ENB_S1U_TEID[%d] SGW_S1U_TEID[%d]\n",
        bearer->enb_s1u_teid, bearer->sgw_s1u_teid);

    memcpy(&bearer->sgw_s1u_ip, &pCreateSession->sgw_ip, sizeof(ip_t));
    d_info("ipv4: %d, ipv6: %d", pCreateSession->sgw_ip.ipv4, pCreateSession->sgw_ip.ipv6);
    d_info("ipv4: %d, ipv6: %d", bearer->sgw_s1u_ip.ipv4, bearer->sgw_s1u_ip.ipv6);

    /* TODO: fix it */
    // 
    amf4g_ue->sgw_s11_teid = amf4g_ue->index;
    /**********/ 
    d_info("create Session");
    if (FSM_CHECK(&amf4g_ue->sm, emm_state_initial_context_setup))
    {
        rv = nas_send_attach_accept(amf4g_ue);
        d_assert(rv == CORE_OK, return, "nas_send_attach_accept failed");
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
    {
        rv = nas_send_activate_default_bearer_context_request(bearer);
        d_assert(rv == CORE_OK, return, "nas send failed");
    }
    else
        d_assert(0,, "Invalid EMM state");
#endif
}

void amf_n11_handle_modify_bearer_response( amf4g_ue_t *amf4g_ue, modify_bearer_t *modifyBearer)
{
    status_t rv;
    enb_ue_t *source_ue = NULL, *target_ue = NULL;

    d_assert(amf4g_ue, return, "Null param");
    d_assert(modifyBearer, goto cleanup, "Null param");

    d_trace(3, "[AMF4G] Modify Bearer Response\n");
    d_trace(5, "    AMF4G_S11_TEID[%d] SGW_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->sgw_s11_teid);

    if (modifyBearer->sm_context_update_type == SM_CONTEXT_UPDATE_TYPE_MODIFY)
    {
        GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_PATH_SWITCH,
            rv = s1ap_send_path_switch_ack(amf4g_ue);
            d_assert(rv == CORE_OK,, "s1ap send error");
        );

        GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_HANDOVER_NOTIFY,
            target_ue = amf4g_ue->enb_ue;
            d_assert(target_ue, return, "Null param");
            source_ue = target_ue->source_ue;
            d_assert(source_ue, return, "Null param");

            rv = s1ap_send_ue_context_release_command(source_ue,
                    S1AP_Cause_PR_radioNetwork,
                    S1AP_CauseRadioNetwork_successful_handover,
                    S1AP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL, 300);
            d_assert(rv == CORE_OK,, "s1ap send error");
        );

        return;

    cleanup:
        GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_PATH_SWITCH,);
        GTP_COUNTER_CHECK(amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_HANDOVER_NOTIFY,);
    }
    else if (modifyBearer->sm_context_update_type == SM_CONTEXT_UPDATE_TYPE_RELEASE_ACCESS)
    {
        enb_ue_t* enb_ue = amf4g_ue->enb_ue;
        d_assert(enb_ue, return, "Null param");

        rv = CLEAR_BEARER_CONTEXT(amf4g_ue);
        d_assert(rv == CORE_OK,, "AMF4G_BEARER_SET_INACTIVE failed");

        rv = s1ap_send_ue_context_release_command(enb_ue,
                S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                S1AP_UE_CTX_REL_S1_NORMAL_RELEASE, 0);
        d_assert(rv == CORE_OK,, "s1ap send error");
    }
    else
    {
        d_error("Unknown SM Context Update type: %02X", modifyBearer->sm_context_update_type);
    }
}

void amf_n11_handle_delete_session_response(amf4g_ue_t *amf4g_ue, delete_session_t *deleteSession)
{
    status_t rv;
    amf4g_sess_t *sess = NULL;

    d_assert(amf4g_ue, return, "Null param");

    d_trace(3, "[AMF] Release Access Bearers Response\n");
    sess = amf4g_sess_find_by_ebi(amf4g_ue, deleteSession->ebi);
    d_assert(sess, return, "Null param");

        if (FSM_CHECK(&amf4g_ue->sm, emm_state_authentication))
    {
        if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        {
            amf4g_s6a_send_air(amf4g_ue, NULL);
        }
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_de_registered))
    {
        if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        {
            rv = nas_send_detach_accept(amf4g_ue);
            d_assert(rv == CORE_OK,, "nas_send_detach_accept failed");
        }
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
    {
        amf4g_bearer_t *bearer = amf4g_default_bearer_in_sess(sess);
        d_assert(bearer, goto cleanup, "Null param");

        if (FSM_CHECK(&bearer->sm, esm_state_pdn_will_disconnect))
        {
            rv = nas_send_deactivate_bearer_context_request(bearer);
            d_assert(rv == CORE_OK,,
                "nas_send_deactivate_bearer_context_request failed");
            
            /*
             * amf4g_sess_remove() should not be called here.
             *
             * Session will be removed if Deactivate bearer context 
             * accept is received */
            CLEAR_SGW_S1U_PATH(sess);
            return;
        }
        else
            d_assert(0,, "Invalid ESM state");
    }
    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_initial_context_setup) ||
             FSM_CHECK(&amf4g_ue->sm, emm_state_exception))
    {
        if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        {
            enb_ue_t *enb_ue = NULL;

            enb_ue = amf4g_ue->enb_ue;
            d_assert(enb_ue, goto cleanup, );

            rv = s1ap_send_ue_context_release_command(enb_ue,
                S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                S1AP_UE_CTX_REL_UE_CONTEXT_REMOVE, 0);
            d_assert(rv == CORE_OK,, "s1ap send error");
        }
    }
    else
        d_assert(0,, "Invalid EMM state");

cleanup:
    if (amf4g_sess_count(amf4g_ue) == 1) /* Last Session */
        CLEAR_SESSION_CONTEXT(amf4g_ue);

    amf4g_sess_remove(sess);
}
