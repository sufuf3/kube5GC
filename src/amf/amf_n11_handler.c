#define TRACE_MODULE _amf_n11_handler

#include "core_debug.h"

#include "gtp/gtp_types.h"
#include "gtp/gtp_conv.h"
#include "gtp/gtp_xact.h"

#include "amf4g_event.h"
#include "amf4g_sm.h"
#include "amf4g_context.h"

#include "s1ap_path.h"
#include "amf_n11_path.h"
#include "nas_path.h"
#include "amf4g_fd_path.h"

#include "amf_json_build.h"
#include "sbiJson/commonJsonKey.h"
#include "sbiJson/JsonTransform.h"
#include "amf_sbi_path.h"

void amf_n11_handle_create_session_response(
        amf4g_ue_t *amf4g_ue, create_session_t *pCreateSession)
{
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

    /* Data Plane(UL) : UPF-S1U */
    bearer->upf_s1u_teid = pCreateSession->upf_s1u_teid;

    d_trace(5, "    AMF4G_S11_TEID[%d] SMF_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->smf_s11_teid);
    d_trace(5, "    ENB_S1U_TEID[%d] UPF_S1U_TEID[%d]\n",
        bearer->enb_s1u_teid, bearer->upf_s1u_teid);

    memcpy(&bearer->upf_s1u_ip, &pCreateSession->upf_ip, sizeof(ip_t));
    d_info("ipv4: %d, ipv6: %d", pCreateSession->upf_ip.ipv4, pCreateSession->upf_ip.ipv6);
    d_info("ipv4: %d, ipv6: %d", bearer->upf_s1u_ip.ipv4, bearer->upf_s1u_ip.ipv6);

    /* TODO: fix it */
    // 
    amf4g_ue->smf_s11_teid = amf4g_ue->index;
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
}

void amf_n11_handle_modify_bearer_response( amf4g_ue_t *amf4g_ue, modify_bearer_t *modifyBearer)
{
    status_t rv;
    enb_ue_t *source_ue = NULL, *target_ue = NULL;

    d_assert(amf4g_ue, return, "Null param");
    d_assert(modifyBearer, goto cleanup, "Null param");

    d_trace(3, "[AMF4G] Modify Bearer Response\n");
    d_trace(5, "    AMF4G_S11_TEID[%d] SMF_S11_TEID[%d]\n",
            amf4g_ue->amf4g_s11_teid, amf4g_ue->smf_s11_teid);

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
    else if (modifyBearer->sm_context_update_type == SM_CONTEXT_UPDATE_TYPE_DOWNLINK_DATA_NOTIFICATION)
    {
        amf4g_ue_t *amf4g_ue = NULL;
        amf4g_sess_t *sess = NULL;
	    amf4g_bearer_t *bearer = NULL;
        pkbuf_t *pkbuf = NULL;
        d_trace(3, "[AMF] SM Context Update Type Downlink Data Notification\n");
	    sess = amf4g_sess_first(amf4g_ue);
	    d_assert(sess, return, "Null param");
        bearer = amf4g_bearer_first(sess);
        d_assert(bearer, return, "Null param");
        d_trace(3, "[AMF] SM Context Update Type Downlink Data Notification Ack\n");
        amf_json_build_modify_bearer(&pkbuf, bearer, SM_CONTEXT_UPDATE_TYPE_DOWNLINK_DATA_NOTIFICATION_ACK);
        amf_sbi_send_sm_context_update(pkbuf);
        pkbuf_free(pkbuf);

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
            CLEAR_UPF_S1U_PATH(sess);
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
