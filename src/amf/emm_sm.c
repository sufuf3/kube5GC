#define TRACE_MODULE _emm_sm

#include "core_debug.h"

#include "nas/nas_message.h"
#include "fd/fd_lib.h"
#include "fd/s6a/s6a_message.h"

#include "amf4g_event.h"
#include "amf4g_kdf.h"
#include "s1ap_handler.h"
#include "amf4g_fd_path.h"
#include "emm_handler.h"
#include "emm_build.h"
#include "esm_handler.h"
#include "nas_path.h"
#include "s1ap_path.h"
#include "amf_n11_path.h"
#include "amf4g_path.h"
#include "amf4g_sm.h"

void emm_state_initial(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    amf4g_sm_trace(3, e);

    FSM_TRAN(s, &emm_state_de_registered);
}

void emm_state_final(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    amf4g_sm_trace(3, e);
}

static void common_register_state(fsm_t *s, event_t *e);

void emm_state_de_registered(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    common_register_state(s, e);
}

void emm_state_registered(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    common_register_state(s, e);
}

static void common_register_state(fsm_t *s, event_t *e)
{
    status_t rv;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;
        
    amf4g_ue = amf4g_ue_find(event_get_param1(e));
    d_assert(amf4g_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            return;
        }
        case FSM_EXIT_SIG:
        {
            return;
        }
        case AMF4G_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param5(e);
            d_assert(message, return, "Null param");

            enb_ue = amf4g_ue->enb_ue;
            d_assert(enb_ue, return, "Null param");

            if (message->emm.h.security_header_type
                    == NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE)
            {
                d_trace(3, "[EMM] Service request\n");
                rv = emm_handle_service_request(
                        amf4g_ue, &message->emm.service_request);
                if (rv != CORE_OK)
                {
                    d_error("emm_handle_service_request() failed");
                    FSM_TRAN(s, emm_state_exception);
                    return;
                }

                if (!AMF4G_UE_HAVE_IMSI(amf4g_ue))
                {
                    d_warn("[EMM] Service request : Unknown UE");
                    rv = nas_send_service_reject(amf4g_ue,
                        EMM_CAUSE_UE_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK);
                    d_assert(rv == CORE_OK,,
                            "nas_send_service_reject() failed");
                    FSM_TRAN(s, &emm_state_exception);
                    return;
                }

                if (!SECURITY_CONTEXT_IS_VALID(amf4g_ue))
                {
                    d_warn("No Security Context : IMSI[%s]", amf4g_ue->imsi_bcd);
                    rv = nas_send_service_reject(amf4g_ue,
                        EMM_CAUSE_UE_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK);
                    d_assert(rv == CORE_OK,,
                        "nas_send_service_reject() failed");

                    rv = s1ap_send_ue_context_release_command(enb_ue, 
                        S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                        S1AP_UE_CTX_REL_S1_NORMAL_RELEASE, 0);
                    d_assert(rv == CORE_OK,,
                        "s1ap_send_ue_context_release_command() failed");
                    return;
                }

                rv = s1ap_send_initial_context_setup_request(amf4g_ue);
                d_assert(rv == CORE_OK, return,
                    "s1ap_send_initial_context_setup_request() failed");
                return;
            }

            switch(message->emm.h.message_type)
            {
                case NAS_IDENTITY_RESPONSE:
                {
                    d_trace(3, "[EMM] Identity response\n");
                    rv = emm_handle_identity_response(amf4g_ue,
                            &message->emm.identity_response);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_identity_response() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    if (!AMF4G_UE_HAVE_IMSI(amf4g_ue))
                    {
                        d_error("No IMSI");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    break;
                }
                case NAS_ATTACH_REQUEST:
                {
                    d_trace(3, "[EMM] Attach request\n", amf4g_ue->imsi_bcd);
                    rv = emm_handle_attach_request(
                            amf4g_ue, &message->emm.attach_request);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_attach_request() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }
                    break;
                }
                case NAS_TRACKING_AREA_UPDATE_REQUEST:
                {
                    d_trace(3, "[EMM] Tracking area update request\n");
                    rv = emm_handle_tau_request(
                            amf4g_ue, &message->emm.tracking_area_update_request);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_tau_request() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    if (!AMF4G_UE_HAVE_IMSI(amf4g_ue))
                    {
                        d_warn("[EMM] TAU request : Unknown UE");
                        rv = nas_send_tau_reject(amf4g_ue, EMM_CAUSE_UE_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK);
                        d_assert(rv == CORE_OK,,
                                "nas_send_tau_reject() failed");
                        FSM_TRAN(s, &emm_state_exception);
                        return;
                    }

                    break;
                }
                case NAS_TRACKING_AREA_UPDATE_COMPLETE:
                {
                    d_trace(3, "[EMM] Tracking area update complete\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    return;
                }
                case NAS_EMM_STATUS:
                {
                    d_warn("[EMM] EMM STATUS : IMSI[%s] Cause[%d]",
                            amf4g_ue->imsi_bcd,
                            message->emm.emm_status.emm_cause);
                    FSM_TRAN(s, &emm_state_exception);
                    return;
                }
                case NAS_DETACH_REQUEST:
                {
                    d_trace(3, "[EMM] Detach request\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    rv = emm_handle_detach_request(
                            amf4g_ue, &message->emm.detach_request_from_ue);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_attach_request() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    rv = amf4g_send_delete_session_or_detach(amf4g_ue);
                    d_assert(rv == CORE_OK,,
                            "amf4g_send_delete_session_or_detach() failed");

                    FSM_TRAN(s, &emm_state_de_registered);
                    return;
                }
                default:
                {
                    d_warn("Unknown message[%d]", message->emm.h.message_type);
                    return;
                }
            }
            break;
        }
        case AMF4G_EVT_EMM_T3413:
        {
            if (amf4g_ue->max_paging_retry >= MAX_NUM_OF_PAGING)
            {
                /* Paging failed */
                d_warn("[EMM] Paging to IMSI[%s] failed. Stop paging",
                        amf4g_ue->imsi_bcd);
                if (amf4g_ue->last_paging_msg)
                {
                    pkbuf_free(amf4g_ue->last_paging_msg);
                    amf4g_ue->last_paging_msg = NULL;
                }
            }
            else
            {
                amf4g_ue->max_paging_retry++;
                s1ap_handle_paging(amf4g_ue);
                /* Start T3413 */
                tm_start(amf4g_ue->t3413);
            }
            return;
        }
        default:
        {
            d_error("Unknown event[%s]", amf4g_event_get_name(e));
            return;
        }
    }

    if (!AMF4G_UE_HAVE_IMSI(amf4g_ue))
    {
        rv = nas_send_identity_request(amf4g_ue);
        d_assert(rv == CORE_OK, return, "nas_send_identity_request() failed");
        return;
    }

    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return, "Null param");

    switch(amf4g_ue->nas_eps.type)
    {
        case AMF4G_EPS_TYPE_ATTACH_REQUEST:
        {
            if (SECURITY_CONTEXT_IS_VALID(amf4g_ue))
            {
                rv = nas_send_emm_to_esm(amf4g_ue,
                        &amf4g_ue->pdn_connectivity_request);
                d_assert(rv == CORE_OK,, "nas_send_emm_to_esm() failed");
                FSM_TRAN(s, &emm_state_initial_context_setup);
            }
            else
            {
                if (SESSION_CONTEXT_IS_AVAILABLE(amf4g_ue))
                {
                    rv = amf_n11_send_delete_all_sessions(amf4g_ue);
                    d_assert(rv == CORE_OK,,
                        "amf_n11_send_delete_all_sessions() failed");
                }
                else
                {
                    amf4g_s6a_send_air(amf4g_ue, NULL);
                }
                FSM_TRAN(s, &emm_state_authentication);
            }
            break;
        }
        case AMF4G_EPS_TYPE_TAU_REQUEST:
        {
            S1AP_ProcedureCode_t procedureCode =
                (S1AP_ProcedureCode_t)event_get_param2(e);

            if (!SESSION_CONTEXT_IS_AVAILABLE(amf4g_ue))
            {
                d_warn("No PDN Connection : UE[%s]", amf4g_ue->imsi_bcd);
                rv = nas_send_tau_reject(amf4g_ue,
                    EMM_CAUSE_UE_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK);
                d_assert(rv == CORE_OK,, "nas_send_tau_reject() failed");
                FSM_TRAN(s, emm_state_exception);
                break;
            }

            if (!SECURITY_CONTEXT_IS_VALID(amf4g_ue))
            {
                amf4g_s6a_send_air(amf4g_ue, NULL);
                FSM_TRAN(&amf4g_ue->sm, &emm_state_authentication);
                break;
            }

            if (procedureCode == S1AP_ProcedureCode_id_initialUEMessage)
            {
                d_trace(5, "    Iniital UE Message\n");
                if (amf4g_ue->nas_eps.update.active_flag)
                {
                    rv = nas_send_tau_accept(amf4g_ue,
                            S1AP_ProcedureCode_id_InitialContextSetup);
                    d_assert(rv == CORE_OK,, "nas_send_tau_accept() failed");
                }
                else
                {
                    rv = nas_send_tau_accept(amf4g_ue,
                            S1AP_ProcedureCode_id_downlinkNASTransport);
                    d_assert(rv == CORE_OK,, "nas_send_tau_accept() failed");

                    rv = amf4g_send_release_access_bearer_or_ue_context_release(
                            amf4g_ue, enb_ue);
                    d_assert(rv == CORE_OK,, "amf4g_send_release_access_bearer"
                            "_or_ue_context_release() failed");
                }
            }
            else if (procedureCode == S1AP_ProcedureCode_id_uplinkNASTransport)
            {
                d_trace(5, "    Uplink NAS Transport\n");
                rv = nas_send_tau_accept(amf4g_ue,
                        S1AP_ProcedureCode_id_downlinkNASTransport);
                d_assert(rv == CORE_OK,, "nas_send_tau_accept() failed");
            }
            else
                d_assert(0,, "Invalid Procedure Code[%d]", procedureCode);
            break;
        }
        default:
            d_assert(0,, "Invalid NAS-EPS[%d]", amf4g_ue->nas_eps.type);
            break;
    }
}

void emm_state_authentication(fsm_t *s, event_t *e)
{
    status_t rv;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    amf4g_ue = amf4g_ue_find(event_get_param1(e));
    d_assert(amf4g_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case AMF4G_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param5(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_AUTHENTICATION_RESPONSE:
                {
                    nas_authentication_response_t *authentication_response =
                        &message->emm.authentication_response;
                    nas_authentication_response_parameter_t
                        *authentication_response_parameter =
                            &authentication_response->
                                authentication_response_parameter;

                    d_trace(3, "[EMM] Authentication response\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);

                    if (authentication_response_parameter->length != 
                            amf4g_ue->xres_len ||
                        memcmp(authentication_response_parameter->res,
                            amf4g_ue->xres, amf4g_ue->xres_len) != 0)
                    {
                        rv = nas_send_authentication_reject(amf4g_ue);
                        d_assert(rv == CORE_OK,, "nas send error");
                        FSM_TRAN(&amf4g_ue->sm, &emm_state_exception);
                    }
                    else
                    {
                        FSM_TRAN(&amf4g_ue->sm, &emm_state_security_mode);
                    }
                    break;
                }
                case NAS_AUTHENTICATION_FAILURE:
                {
                    nas_authentication_failure_t *authentication_failure =
                        &message->emm.authentication_failure;
                    nas_authentication_failure_parameter_t
                        *authentication_failure_parameter = 
                            &authentication_failure->
                                authentication_failure_parameter;

                    d_trace(3, "[EMM] Authentication failure\n");
                    d_trace(5, "    IMSI[%s] EMM_CAUSE[%d]\n", amf4g_ue->imsi_bcd,
                            authentication_failure->emm_cause);

                    switch(authentication_failure->emm_cause)
                    {
                        case EMM_CAUSE_MAC_FAILURE:
                            d_warn("Authentication failure(MAC failure)");
                            break;
                        case EMM_CAUSE_NON_EPS_AUTHENTICATION_UNACCEPTABLE:
                            d_error("Authentication failure"
                                    "(Non-EPS authentication unacceptable)");
                            break;
                        case EMM_CAUSE_SYNCH_FAILURE:
                            d_warn("Authentication failure(Synch failure)");
                            amf4g_s6a_send_air(amf4g_ue,
                                    authentication_failure_parameter);
                            return;
                        default:
                            d_error("Unknown EMM_CAUSE{%d] in Authentication"
                                    " failure",
                                    authentication_failure->emm_cause);
                            break;
                    }

                    rv = nas_send_authentication_reject(amf4g_ue);
                    d_assert(rv == CORE_OK,, "nas send error");
                    FSM_TRAN(&amf4g_ue->sm, &emm_state_exception);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    d_warn("[EMM] EMM STATUS : IMSI[%s] Cause[%d]",
                            amf4g_ue->imsi_bcd,
                            message->emm.emm_status.emm_cause);
                    FSM_TRAN(s, &emm_state_exception);
                    break;
                }
                case NAS_DETACH_REQUEST:
                {
                    d_trace(3, "[EMM] Detach request\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    rv = emm_handle_detach_request(
                            amf4g_ue, &message->emm.detach_request_from_ue);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_attach_request() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    rv = amf4g_send_delete_session_or_detach(amf4g_ue);
                    d_assert(rv == CORE_OK,,
                            "amf4g_send_delete_session_or_detach() failed");

                    FSM_TRAN(s, &emm_state_de_registered);
                    break;
                }
                case NAS_ATTACH_REQUEST:
                {
                    if (SESSION_CONTEXT_IS_AVAILABLE(amf4g_ue))
                    {
                        d_info("%s %d start", __FUNCTION__, __LINE__);
                        rv = amf_n11_send_delete_all_sessions(amf4g_ue);
                        d_assert(rv == CORE_OK,,
                            "amf_n11_send_delete_all_sessions() failed");
                        d_info("%s %d end", __FUNCTION__, __LINE__);
                    }
                    else
                    {
                        d_info("%s %d start", __FUNCTION__, __LINE__);
                        amf4g_s6a_send_air(amf4g_ue, NULL);
                        d_info("%s %d end", __FUNCTION__, __LINE__);
                    }
                    break;
                }
                default:
                {
                    d_warn("Unknown message[%d]", message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event[%s]", amf4g_event_get_name(e));
            break;
        }
    }
}

void emm_state_security_mode(fsm_t *s, event_t *e)
{
    status_t rv;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    amf4g_ue = amf4g_ue_find(event_get_param1(e));
    d_assert(amf4g_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            pkbuf_t *emmbuf = NULL;

            rv = emm_build_security_mode_command(&emmbuf, amf4g_ue);
            d_assert(rv == CORE_OK && emmbuf, break, "emm build error");

            rv = nas_send_to_downlink_nas_transport(amf4g_ue, emmbuf);
            d_assert(rv == CORE_OK && emmbuf, break, "emm send error");

            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case AMF4G_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param5(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_SECURITY_MODE_COMPLETE:
                {
                    d_trace(3, "[EMM] Security mode complete\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);

                    /* Update Kenb */
                    if (SECURITY_CONTEXT_IS_VALID(amf4g_ue))
                    {
                        amf4g_kdf_enb(amf4g_ue->kasme, amf4g_ue->ul_count.i32, 
                                amf4g_ue->kenb);
                        amf4g_kdf_nh(amf4g_ue->kasme, amf4g_ue->kenb, amf4g_ue->nh);
                        amf4g_ue->nhcc = 1;
                    }

                    amf4g_s6a_send_ulr(amf4g_ue);
                    if (amf4g_ue->nas_eps.type == AMF4G_EPS_TYPE_ATTACH_REQUEST)
                    {
                        FSM_TRAN(s, &emm_state_initial_context_setup);
                    }
                    else if (amf4g_ue->nas_eps.type ==
                            AMF4G_EPS_TYPE_SERVICE_REQUEST ||
                            amf4g_ue->nas_eps.type == AMF4G_EPS_TYPE_TAU_REQUEST)
                    {
                        FSM_TRAN(s, &emm_state_registered);
                    }
                    else
                        d_assert(0,, "Invalid NAS_EPS[%d]",
                                amf4g_ue->nas_eps.type);
                    break;
                }
                case NAS_SECURITY_MODE_REJECT:
                {
                    d_warn("[EMM] Security mode reject : IMSI[%s] Cause[%d]",
                            amf4g_ue->imsi_bcd,
                            message->emm.security_mode_reject.emm_cause);
                    FSM_TRAN(s, &emm_state_exception);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    d_warn("[EMM] EMM STATUS : IMSI[%s] Cause[%d]",
                            amf4g_ue->imsi_bcd,
                            message->emm.emm_status.emm_cause);
                    FSM_TRAN(s, &emm_state_exception);
                    break;
                }
                case NAS_DETACH_REQUEST:
                {
                    d_trace(3, "[EMM] Detach request\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    rv = emm_handle_detach_request(
                            amf4g_ue, &message->emm.detach_request_from_ue);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_attach_request() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    rv = amf4g_send_delete_session_or_detach(amf4g_ue);
                    d_assert(rv == CORE_OK,,
                            "amf4g_send_delete_session_or_detach() failed");

                    FSM_TRAN(s, &emm_state_de_registered);
                    break;
                }
                default:
                {
                    d_warn("Unknown message[%d]", message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event[%s]", amf4g_event_get_name(e));
            break;
        }
    }
}

void emm_state_initial_context_setup(fsm_t *s, event_t *e)
{
    status_t rv;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    amf4g_ue = amf4g_ue_find(event_get_param1(e));
    d_assert(amf4g_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case AMF4G_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param5(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_ATTACH_COMPLETE:
                {
                    d_trace(3, "[EMM] Attach complete\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    rv = emm_handle_attach_complete(
                            amf4g_ue, &message->emm.attach_complete);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_attach_complete() failed "
                                "in emm_state_initial_context_setup");
                        FSM_TRAN(s, emm_state_exception);
                        break;
                    }
                    FSM_TRAN(s, &emm_state_registered);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    d_warn("[EMM] EMM STATUS : IMSI[%s] Cause[%d]",
                            amf4g_ue->imsi_bcd,
                            message->emm.emm_status.emm_cause);
                    FSM_TRAN(s, &emm_state_exception);
                    break;
                }
                case NAS_DETACH_REQUEST:
                {
                    d_trace(3, "[EMM] Detach request\n");
                    d_trace(5, "    IMSI[%s]\n", amf4g_ue->imsi_bcd);
                    rv = emm_handle_detach_request(
                            amf4g_ue, &message->emm.detach_request_from_ue);
                    if (rv != CORE_OK)
                    {
                        d_error("emm_handle_attach_request() failed");
                        FSM_TRAN(s, emm_state_exception);
                        return;
                    }

                    rv = amf4g_send_delete_session_or_detach(amf4g_ue);
                    d_assert(rv == CORE_OK,,
                            "amf4g_send_delete_session_or_detach() failed");

                    FSM_TRAN(s, &emm_state_de_registered);
                    break;
                }
                default:
                {
                    d_warn("Unknown message[%d]", 
                            message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event[%s]", amf4g_event_get_name(e));
            break;
        }
    }
}

void emm_state_exception(fsm_t *s, event_t *e)
{
    amf4g_sm_trace(3, e);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        default:
        {
            d_error("Unknown event[%s]", amf4g_event_get_name(e));
            break;
        }
    }
}
