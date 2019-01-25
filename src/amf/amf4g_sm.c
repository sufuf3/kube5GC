#define TRACE_MODULE _amf4g_sm

#include <math.h>

#include "core_debug.h"
#include "core_lib.h"

#include "s1ap/s1ap_message.h"
#include "nas/nas_message.h"
#include "gtp/gtp_xact.h"
#include "fd/fd_lib.h"

#include "app/util.h"
#include "amf4g_context.h"

#include "amf4g_event.h"
#include "amf4g_sm.h"
#include "s1ap_handler.h"
#include "s1ap_path.h"
#include "ngap_handler.h"
#include "ngap_path.h"
#include "s1ap_build.h"
#include "nas_security.h"
#include "nas_path.h"
#include "emm_handler.h"
#include "esm_handler.h"
#include "amf_n11_path.h"
#include "amf4g_fd_path.h"
#include "amf4g_s6a_handler.h"
#include "amf4g_path.h"
#include "amf_sbi_path.h"

#include "JsonTransform.h"
#include "amf_json_handler.h"
#include "amf_n11_handler.h"

void amf4g_state_initial(fsm_t *s, event_t *e)
{
    amf4g_sm_trace(3, e);

    d_assert(s, return, "Null param");

    FSM_TRAN(s, &amf4g_state_operational);
}

void amf4g_state_final(fsm_t *s, event_t *e)
{
    amf4g_sm_trace(3, e);

    d_assert(s, return, "Null param");
}

void amf4g_state_operational(fsm_t *s, event_t *e)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];

    amf4g_sm_trace(3, e);

    d_assert(s, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            rv = s1ap_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish S1AP path");
                break;
            }
            rv = ngap_open();
            if(rv != CORE_OK)
            {
                d_error("Can't establish NGAP path");
                break;
            }
            rv = amf_sbi_server_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish AMF-SBI path");
                break;
            }
            break;
        }
        case FSM_EXIT_SIG:
        {
            rv = s1ap_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close S1AP path");
            }
            rv = ngap_close();
            if(rv != CORE_OK)
            {
                d_error("Can't close NGAP path");
                break;
            }
            rv = amf_sbi_server_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close SMF-SBI path");
                break;
            }
            break;
        }
        case AMF4G_EVT_S1AP_LO_ACCEPT:
        {
            sock_id sock = (sock_id)event_get_param1(e);
            d_assert(sock, break,);
            c_sockaddr_t *addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break,);
            amf4g_enb_t *enb = NULL;

            d_trace(1, "eNB-S1 accepted[%s] in master_sm module\n", 
                CORE_ADDR(addr, buf));
                    
            enb = amf4g_enb_find_by_addr(addr);
            if (!enb)
            {
#if USE_USRSCTP != 1
                status_t rv;

                rv = sock_register(sock, s1ap_recv_handler, NULL);
                d_assert(rv == CORE_OK, break, "register s1ap_recv_cb failed");
#endif
                enb = amf4g_enb_add(sock, addr);
                d_assert(enb, break, "Null param");
            }
            else
            {
                d_warn("eNB context duplicated with IP-address [%s]!!!", 
                        CORE_ADDR(addr, buf));
                sock_delete(sock);
                d_warn("S1 Socket Closed");
            }

            break;
        }
        case AMF4G_EVT_S1AP_LO_SCTP_COMM_UP:
        {
            amf4g_enb_t *enb = NULL;
            sock_id sock = 0;
            c_sockaddr_t *addr = NULL;
            c_uint16_t outbound_streams = 0;

            sock = (sock_id)event_get_param1(e);
            d_assert(sock, break, "Null param");
            addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break, "Null param");

            outbound_streams = (c_uint16_t)event_get_param4(e);

            enb = amf4g_enb_find_by_addr(addr);
            if (!enb)
            {
#if USE_USRSCTP != 1
                status_t rv;

                rv = sock_register(sock, s1ap_recv_handler, NULL);
                d_assert(rv == CORE_OK, break, "register s1ap_recv_cb failed");
#endif
                enb = amf4g_enb_add(sock, addr);
                d_assert(enb, break, "Null param");
            }
            else
            {
                CORE_FREE(addr);
            }

            enb->outbound_streams =
                    c_min(outbound_streams, enb->outbound_streams);

            d_trace(3, "eNB-S1 SCTP_COMM_UP[%s] Outbound Streams[%d]\n", 
                CORE_ADDR(addr, buf), enb->outbound_streams);

            break;
        }
        case AMF4G_EVT_S1AP_LO_CONNREFUSED:
        {
            amf4g_enb_t *enb = NULL;
            sock_id sock = 0;
            c_sockaddr_t *addr = NULL;

            sock = (sock_id)event_get_param1(e);
            d_assert(sock, break, "Null param");
            addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break, "Null param");

            enb = amf4g_enb_find_by_addr(addr);
            CORE_FREE(addr);

            if (enb)
            {
                d_trace(1, "eNB-S1[%x] connection refused!!!\n", enb->enb_id);
                amf4g_enb_remove(enb);
            }
            else
            {
                d_warn("Socket connection refused, Already Removed!");
            }

            break;
        }
        case AMF4G_EVT_S1AP_MESSAGE:
        {
            s1ap_message_t message;
            amf4g_enb_t *enb = NULL;
            sock_id sock = 0;
            c_sockaddr_t *addr = NULL;
            pkbuf_t *pkbuf = NULL;

            sock = (sock_id)event_get_param1(e);
            d_assert(sock, break, "Null param");

            addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break, "Null param");
            
            pkbuf = (pkbuf_t *)event_get_param3(e);
            d_assert(pkbuf, break, "Null param");

            enb = amf4g_enb_find_by_addr(addr);
            CORE_FREE(addr);

            d_assert(enb, pkbuf_free(pkbuf); break, "No eNB context");
            d_assert(FSM_STATE(&enb->sm), pkbuf_free(pkbuf); break,
                    "No S1AP State Machine");

            rv = s1ap_decode_pdu(&message, pkbuf);
            if (rv != CORE_OK)
            {            
                d_assert(0, s1ap_free_pdu(&message); pkbuf_free(pkbuf); break,
                        "Can't decode S1AP_PDU");
            }

            event_set_param1(e, (c_uintptr_t)enb->index);
            event_set_param4(e, (c_uintptr_t)&message);
            fsm_dispatch(&enb->sm, (fsm_event_t*)e);

            s1ap_free_pdu(&message);
            pkbuf_free(pkbuf);
            break;
        }
        case AMF4G_EVT_S1AP_DELAYED_SEND:
        {
            enb_ue_t *enb_ue = NULL;
            pkbuf_t *pkbuf = NULL;
            tm_block_id timer = 0;

            enb_ue = enb_ue_find(event_get_param1(e));
            d_assert(enb_ue, break,);

            pkbuf = (pkbuf_t *)event_get_param2(e);
            d_assert(pkbuf, break,);

            timer = event_get_param3(e);
            d_assert(timer, pkbuf_free(pkbuf); break,);

            rv = s1ap_send_to_enb_ue(enb_ue, pkbuf);
            d_assert(rv == CORE_OK, pkbuf_free(pkbuf),);

            tm_delete(timer);
            break;
        }
        case AMF4G_EVT_S1AP_S1_HOLDING_TIMER:
        {
            enb_ue_t *enb_ue = NULL;

            enb_ue = enb_ue_find(event_get_param1(e));
            d_assert(enb_ue, break, "No ENB UE context");
            d_warn("Implicit S1 release");
            d_warn("    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]",
                enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

            rv = enb_ue_remove(enb_ue);
            d_assert(rv == CORE_OK,,);
            break;
        }
        case AMF_EVT_NGAP_LO_ACCEPT:
        {
            sock_id sock = (sock_id)event_get_param1(e);
            d_assert(sock, break,);
            c_sockaddr_t *addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break,);
            amf_ran_t *ran = NULL;

            d_trace(1, "RAN-NG accepted[%s] in master_sm module\n", 
                CORE_ADDR(addr, buf));
                    
            ran = amf_ran_find_by_addr(addr);
            if (!ran)
            {
#if USE_USRSCTP != 1
                status_t rv;

                rv = sock_register(sock, ngap_recv_handler, NULL);
                d_assert(rv == CORE_OK, break, "register ngap_recv_cb failed");
#endif
                ran = amf_ran_add(sock, addr);
                d_assert(ran, break, "Null param");
            }
            else
            {
                d_warn("RAN context duplicated with IP-address [%s]!!!", 
                        CORE_ADDR(addr, buf));
                sock_delete(sock);
                d_warn("NG Socket Closed");
            }

            break;
        }
        case AMF_EVT_NGAP_LO_SCTP_COMM_UP:
        {
            amf_ran_t *ran = NULL;
            sock_id sock = 0;
            c_sockaddr_t *addr = NULL;
            c_uint16_t outbound_streams = 0;

            sock = (sock_id)event_get_param1(e);
            d_assert(sock, break, "Null param");
            addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break, "Null param");

            outbound_streams = (c_uint16_t)event_get_param4(e);

            ran = amf_ran_find_by_addr(addr);
            if (!ran)
            {
#if USE_USRSCTP != 1
                status_t rv;

                rv = sock_register(sock, ngap_recv_handler, NULL);
                d_assert(rv == CORE_OK, break, "register ngap_recv_cb failed");
#endif
                ran = amf_ran_add(sock, addr);
                d_assert(ran, break, "Null param");
            }
            else
            {
                CORE_FREE(addr);
            }

            ran->outbound_streams =
                    c_min(outbound_streams, ran->outbound_streams);

            d_trace(3, "RAN-NG SCTP_COMM_UP[%s] Outbound Streams[%d]\n", 
                CORE_ADDR(addr, buf), ran->outbound_streams);

            break;
        }
        case AMF_EVT_NGAP_LO_CONNREFUSED:
        {
            amf_ran_t *ran = NULL;
            sock_id sock = 0;
            c_sockaddr_t *addr = NULL;

            sock = (sock_id)event_get_param1(e);
            d_assert(sock, break, "Null param");
            addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break, "Null param");

            ran = amf_ran_find_by_addr(addr);
            CORE_FREE(addr);

            if (ran)
            {
                d_trace(1, "RAN-NG[%x] connection refused!!!\n", ran->ran_id);
                amf_ran_remove(ran);
            }
            else
            {
                d_warn("Socket connection refused, Already Removed!");
            }

            break;
        }
        case AMF_EVT_NGAP_MESSAGE:
        {
            ngap_message_t message;
            amf_ran_t *ran = NULL;
            sock_id sock = 0;
            c_sockaddr_t *addr = NULL;
            pkbuf_t *pkbuf = NULL;

            sock = (sock_id)event_get_param1(e);
            d_assert(sock, break, "Null param");

            addr = (c_sockaddr_t *)event_get_param2(e);
            d_assert(addr, break, "Null param");
            
            pkbuf = (pkbuf_t *)event_get_param3(e);
            d_assert(pkbuf, break, "Null param");

            ran = amf_ran_find_by_addr(addr);
            CORE_FREE(addr);

            d_assert(ran, pkbuf_free(pkbuf); break, "No RAN context");
            d_assert(FSM_STATE(&ran->sm), pkbuf_free(pkbuf); break,
                    "No NGAP State Machine");

            rv = ngap_decode_pdu(&message, pkbuf);
            if (rv != CORE_OK)
            {
                d_assert(0, ngap_free_pdu(&message); pkbuf_free(pkbuf); break,
                        "Can't decode NGAP_PDU");
            }

            event_set_param1(e, (c_uintptr_t)ran->index);
            event_set_param4(e, (c_uintptr_t)&message);
            fsm_dispatch(&ran->sm, (fsm_event_t*)e);

            ngap_free_pdu(&message);
            pkbuf_free(pkbuf);
            break;
        }
        case AMF_EVT_NGAP_DELAYED_SEND:
        {
            ran_ue_t *ran_ue = NULL;
            pkbuf_t *pkbuf = NULL;
            tm_block_id timer = 0;

            ran_ue = ran_ue_find(event_get_param1(e));
            d_assert(ran_ue, break,);

            pkbuf = (pkbuf_t *)event_get_param2(e);
            d_assert(pkbuf, break,);

            timer = event_get_param3(e);
            d_assert(timer, pkbuf_free(pkbuf); break,);

            rv = ngap_send_to_ran_ue(ran_ue, pkbuf);
            d_assert(rv == CORE_OK, pkbuf_free(pkbuf),);

            tm_delete(timer);
            break;
        }
        case AMF_EVT_NGAP_NG_HOLDING_TIMER:
        {
            ran_ue_t *ran_ue = NULL;

            ran_ue = ran_ue_find(event_get_param1(e));
            d_assert(ran_ue, break, "No RAN UE context");
            d_warn("Implicit NG release");
            d_warn("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]",
                ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);

            rv = ran_ue_remove(ran_ue);
            d_assert(rv == CORE_OK,,);
            break;
        }
        case AMF4G_EVT_EMM_MESSAGE:
        {
            nas_message_t message;
            pkbuf_t *pkbuf = NULL;
            enb_ue_t *enb_ue = NULL;
            amf4g_ue_t *amf4g_ue = NULL;

            enb_ue = enb_ue_find(event_get_param1(e));
            d_assert(enb_ue, break, "No ENB UE context");
            pkbuf = (pkbuf_t *)event_get_param4(e);
            d_assert(pkbuf, break, "Null param");
            d_assert(nas_emm_decode(&message, pkbuf) == CORE_OK,
                    pkbuf_free(pkbuf); break, "Can't decode NAS_EMM");

            amf4g_ue = enb_ue->amf4g_ue;
            if (!amf4g_ue)
            {
                amf4g_ue = amf4g_ue_find_by_message(&message);
                if (!amf4g_ue)
                {
                    amf4g_ue = amf4g_ue_add(enb_ue);
                    d_assert(amf4g_ue, pkbuf_free(pkbuf); break, "Null param");
                }
                else
                {
                    /* Here, if the AMF4G_UE Context is found,
                     * the integrity check is not performed
                     * For example, ATTACH_REQUEST, 
                     * TRACKING_AREA_UPDATE_REQUEST message
                     *
                     * Now, We will check the MAC in the NAS message*/
                    nas_security_header_type_t h;
                    h.type = (c_uint8_t)event_get_param3(e);
                    if (h.integrity_protected)
                    {
                        /* Decryption was performed in S1AP handler.
                         * So, we disabled 'ciphered' 
                         * not to decrypt NAS message */
                        h.ciphered = 0;
                        d_assert(
                            nas_security_decode(amf4g_ue, h, pkbuf) == CORE_OK,
                            pkbuf_free(pkbuf); return, 
                            "nas_security_decode failed");
                    }
                }

                /* If NAS(amf4g_ue_t) has already been associated with
                 * older S1(enb_ue_t) context */
                if (ECM_CONNECTED(amf4g_ue))
                {
#if 1 /* IMPLICIT_S1_RELEASE */
                   /* Implcit S1 release */
                    d_trace(5, "Implicit S1 release\n");
                    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
                          amf4g_ue->enb_ue->enb_ue_s1ap_id,
                          amf4g_ue->enb_ue->mme_ue_s1ap_id);
                    rv = enb_ue_remove(amf4g_ue->enb_ue);
                    d_assert(rv == CORE_OK,,);

#else /* S1_HOLDING_TIMER */

                    /* Previous S1(enb_ue_t) context the holding timer(30secs)
                     * is started.
                     * Newly associated S1(enb_ue_t) context holding timer
                     * is stopped. */
                    d_trace(5, "Start S1 Holding Timer\n");
                    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
                            amf4g_ue->enb_ue->enb_ue_s1ap_id, 
                            amf4g_ue->enb_ue->mme_ue_s1ap_id);
                    tm_start(amf4g_ue->enb_ue->holding_timer);

                    /* De-associate S1 with NAS/EMM */
                    rv = enb_ue_deassociate(amf4g_ue->enb_ue);
                    d_assert(rv == CORE_OK,,);
#endif
                }
                tm_stop(enb_ue->holding_timer);
                amf4g_ue_associate_enb_ue(amf4g_ue, enb_ue);
            }

            d_assert(amf4g_ue, pkbuf_free(pkbuf); break, "No AMF4G UE context");
            d_assert(FSM_STATE(&amf4g_ue->sm), pkbuf_free(pkbuf); break, 
                    "No EMM State Machine");

            event_set_param1(e, (c_uintptr_t)amf4g_ue->index);
            event_set_param5(e, (c_uintptr_t)&message);

            fsm_dispatch(&amf4g_ue->sm, (fsm_event_t*)e);
            if (FSM_CHECK(&amf4g_ue->sm, emm_state_exception))
            {
                rv = amf4g_send_delete_session_or_ue_context_release(
                        amf4g_ue, enb_ue);
                d_assert(rv == CORE_OK,,
                        "amf4g_send_delete_session_or_ue_context_release() failed");
            }

            pkbuf_free(pkbuf);
            break;
        }
        case AMF4G_EVT_EMM_T3413:
        {
            amf4g_ue_t *amf4g_ue = amf4g_ue_find(event_get_param1(e));
            d_assert(amf4g_ue, break, "No UE context");
            d_assert(FSM_STATE(&amf4g_ue->sm), break, "No EMM State Machine");

            fsm_dispatch(&amf4g_ue->sm, (fsm_event_t*)e);

            break;
        }
        case AMF4G_EVT_ESM_MESSAGE:
        {
            nas_message_t message;
            amf4g_ue_t *amf4g_ue = NULL;
            amf4g_bearer_t *bearer = NULL;
            amf4g_bearer_t *default_bearer = NULL;
            amf4g_sess_t *sess = NULL;
            pkbuf_t *pkbuf = NULL;

            amf4g_ue = amf4g_ue_find(event_get_param1(e));
            d_assert(amf4g_ue, break, "No UE context");

            pkbuf = (pkbuf_t *)event_get_param2(e);
            d_assert(pkbuf, break, "Null param");
            d_assert(nas_esm_decode(&message, pkbuf) == CORE_OK,
                    pkbuf_free(pkbuf); break, "Can't decode NAS_ESM");

            bearer = amf4g_bearer_find_or_add_by_message(amf4g_ue, &message);
            d_assert(bearer, pkbuf_free(pkbuf); break, "No Bearer context");
            sess = bearer->sess;
            d_assert(sess, pkbuf_free(pkbuf); break, "Null param");
            default_bearer = amf4g_default_bearer_in_sess(sess);
            d_assert(default_bearer, pkbuf_free(pkbuf); break, "Null param");

            event_set_param1(e, (c_uintptr_t)bearer->index);
            event_set_param3(e, (c_uintptr_t)&message);

            fsm_dispatch(&bearer->sm, (fsm_event_t*)e);
            if (FSM_CHECK(&bearer->sm, esm_state_bearer_deactivated) ||
                FSM_CHECK(&bearer->sm, esm_state_exception))
            {
                if (default_bearer->ebi == bearer->ebi)
                {
                    /* if the bearer is a default bearer,
                     * remove all session context linked the default bearer */
                    amf4g_sess_remove(sess);
                }
                else
                {
                    /* if the bearer is not a default bearer,
                     * just remove the bearer context */
                    amf4g_bearer_remove(bearer);
                }
            }
            else if (FSM_CHECK(&bearer->sm, esm_state_pdn_did_disconnect))
            {
                d_assert(default_bearer->ebi == bearer->ebi,
                        pkbuf_free(pkbuf); break,
                        "Bearer[%d] is not Default Bearer",
                        default_bearer->ebi, bearer->ebi);
                amf4g_sess_remove(sess);
            }

            pkbuf_free(pkbuf);
            break;
        }
        case AMF4G_EVT_S6A_MESSAGE:
        {
            status_t rv;
            amf4g_ue_t *amf4g_ue = amf4g_ue_find(event_get_param1(e));
            pkbuf_t *s6abuf = (pkbuf_t *)event_get_param2(e);
            s6a_message_t *s6a_message = NULL;

            d_assert(amf4g_ue, return, "Null param");
            d_assert(s6abuf, return, "Null param");
            s6a_message = s6abuf->payload;
            d_assert(s6a_message, return, "Null param");

            if (s6a_message->result_code != ER_DIAMETER_SUCCESS)
            {
                enb_ue_t *enb_ue = NULL;

                rv = nas_send_attach_reject(amf4g_ue,
                    EMM_CAUSE_IMSI_UNKNOWN_IN_HSS,
                    ESM_CAUSE_PROTOCOL_ERROR_UNSPECIFIED);
                d_assert(rv == CORE_OK,,
                        "nas_send_attach_reject failed");
                d_warn("EMM_CAUSE : IMSI Unknown in HSS");

                enb_ue = amf4g_ue->enb_ue;
                d_assert(enb_ue, break, "No ENB UE context");

                rv = s1ap_send_ue_context_release_command(enb_ue,
                        S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                        S1AP_UE_CTX_REL_UE_CONTEXT_REMOVE, 0);
                d_assert(rv == CORE_OK,, "s1ap send error");

                pkbuf_free(s6abuf);
                break;
            }

            switch(s6a_message->cmd_code)
            {
                case S6A_CMD_CODE_AUTHENTICATION_INFORMATION:
                {
                    amf4g_s6a_handle_aia(amf4g_ue, &s6a_message->aia_message);
                    break;
                }
                case S6A_CMD_CODE_UPDATE_LOCATION:
                {
                    amf4g_s6a_handle_ula(amf4g_ue, &s6a_message->ula_message);

                    if (FSM_CHECK(&amf4g_ue->sm, emm_state_initial_context_setup))
                    {
                        if (amf4g_ue->nas_eps.type == AMF4G_EPS_TYPE_ATTACH_REQUEST)
                        {
                            rv = nas_send_emm_to_esm(amf4g_ue,
                                    &amf4g_ue->pdn_connectivity_request);
                            d_assert(rv == CORE_OK,,
                                    "nas_send_emm_to_esm() failed");
                        }
                        else
                            d_assert(0,, "Invalid Type[%d]",
                                    amf4g_ue->nas_eps.type);
                    }
                    else if (FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
                    {
                        if (amf4g_ue->nas_eps.type == AMF4G_EPS_TYPE_TAU_REQUEST)
                        {
                            rv = nas_send_tau_accept(amf4g_ue,
                                    S1AP_ProcedureCode_id_InitialContextSetup);
                            d_assert(rv == CORE_OK,,
                                    "nas_send_tau_accept() failed");
                        }
                        else if (amf4g_ue->nas_eps.type ==
                            AMF4G_EPS_TYPE_SERVICE_REQUEST)
                        {
                            rv = s1ap_send_initial_context_setup_request(
                                    amf4g_ue);
                            d_assert(rv == CORE_OK,,
                                "s1ap_send_initial_context_setup_request()"
                                "failed");
                        }
                        else
                            d_assert(0,, "Invalid EPS-Type[%d]",
                                    amf4g_ue->nas_eps.type);
                    }
                    else
                        d_assert(0,, "Invaild EMM state for EPS-Type[%d]",
                                    amf4g_ue->nas_eps.type);
                    break;
                }
                default:
                {
                    d_error("Invalid Type[%d]", event_get_param2(e));
                    break;
                }
            }
            pkbuf_free(s6abuf);
            break;
        }
        case AMF4G_EVT_CHECK_OVERLOAD:
        {
            tm_block_id timer = (tm_block_id) event_get_param1(e);
            int n_cores = get_cpu_cores();
            float load_avg = get_cpu_load();
            float load_per_core = load_avg / n_cores;

            d_assert(timer, return, "Null timer");
            d_assert(load_avg >= 0, return, "Negative cpu load average");

            hash_t *enbs = amf4g_self()->enb_id_hash;
            int n_enb = hash_count(enbs);
            int n_enb_to_send = 0;
            pkbuf_t *s1apbuf = NULL;

            /* d_trace(9, "AMF4G_EVT_CHECK_OVERLOAD: load_avg=%.2f, n_cores=%d, threshold=%.2f\n", 
                load_avg, n_cores, OVERLOAD_THRESHOLD); */

            if (load_per_core > OVERLOAD_THRESHOLD)
            {
                if (!amf4g_self()->overload_started)
                {
                    d_trace(1, "AM4G overload_start (load_avg/n_cores=%.2f, threshold=%.2f)\n", 
                        load_per_core, OVERLOAD_THRESHOLD);

                    s1ap_build_overload_start(&s1apbuf);
                    n_enb_to_send = ceil(n_enb * OVERLOAD_NOTIFY_ENB_RATIO);

                    amf4g_self()->overload_started = true;
                }
            }
            else
            {
                if (amf4g_self()->overload_started)
                {
                    d_trace(1, "AMF4G overload_stop (load_avg/n_cores=%.2f, threshold=%.2f)\n", 
                        load_per_core, OVERLOAD_THRESHOLD);

                    s1ap_build_overload_stop(&s1apbuf);
                    n_enb_to_send = n_enb;

                    amf4g_self()->overload_started = false;
                }
            }

            if (s1apbuf != NULL && n_enb_to_send > 0)
            {
                // Send message to randomly selected eNBs which connected to this AMF4G
                amf4g_enb_t **selected_enb = (amf4g_enb_t **) malloc(n_enb_to_send * sizeof(amf4g_enb_t *));
                d_assert(ht_rand_select_non_dup(enbs, n_enb_to_send, (void **) selected_enb) == CORE_OK, break, "Unable to select target eNBs");

                amf4g_enb_t *enb;
                int i = 0;
                for (i = 0; i < n_enb_to_send; i++)
                {                        
                    enb = selected_enb[i];
                    d_trace(1, "- sending message to eNB #%d/%d\n", enb->index, n_enb);
                    d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,
                        "s1ap_send_to_enb() failed");
                }

                free(selected_enb);
            }

            // Restart timer for the next tick
            tm_start(timer);
            break;
        }
        case AMF_EVT_N11_MESSAGE:
        {
            pkbuf_t *recvbuf = (pkbuf_t *)event_get_param1(e);
            
	        int msg_type = event_get_param2(e);
            amf4g_ue_t *amf4g_ue = NULL;
            switch (msg_type)
            {
                case N11_SM_CONTEXT_CREATE:
                {
                    d_trace(10, "create Session Rsp");
                    create_session_t createSession = {0};
                    amf_json_handler_create_session_response(&recvbuf, &createSession);
                    
                    amf4g_ue = amf4g_ue_find_by_imsi(createSession.imsi, createSession.imsi_len);
                    d_assert(amf4g_ue, goto release_amf_n11_pkbuf, "No UE Context");

                    amf_n11_handle_create_session_response(amf4g_ue, &createSession);
                    d_assert(recvbuf, goto release_amf_n11_pkbuf, "Null param");
                    break;
                }
                    
                case N11_SM_CONTEXT_RELEASE:
                {
                    d_trace(10, "[AMF] N11 Delete Session Response");
                    delete_session_t deleteSession = {0};
                    amf_json_handler_delete_session_response(&recvbuf, &deleteSession);
                    
                    amf4g_ue = amf4g_ue_find_by_imsi(deleteSession.imsi, deleteSession.imsi_len);
                    d_assert(amf4g_ue, goto release_amf_n11_pkbuf, "No UE Context");
                    
                    amf_n11_handle_delete_session_response(amf4g_ue, &deleteSession);
                    d_assert(recvbuf, goto release_amf_n11_pkbuf, "Null param");

                    break;
                }
                case N11_SM_CONTEXT_RETRIEVE:
                {
                    break;
                }   
                case N11_SM_CONTEXT_UPDATE:
                {    
                    d_trace(10, "update Session Rsp");
                    modify_bearer_t modifyBearer = {0};
                    amf_json_handler_update_session_response(&recvbuf, &modifyBearer);
                    
                    amf4g_ue = amf4g_ue_find_by_imsi(modifyBearer.imsi, modifyBearer.imsi_len);
                    d_assert(amf4g_ue, goto release_amf_n11_pkbuf, "No UE Context");
                    
                    amf_n11_handle_modify_bearer_response(amf4g_ue, &modifyBearer);
                    d_assert(recvbuf, goto release_amf_n11_pkbuf, "Null param");
                    d_info("AMF Update Session Done");

                    if (ECM_IDLE(amf4g_ue) && modifyBearer.sm_context_update_type == SM_CONTEXT_UPDATE_TYPE_DOWNLINK_DATA_NOTIFICATION)
                    {
                        d_trace(10, "[AMF] amf4g ue is ECM_IDLE\n");
                        s1ap_handle_paging(amf4g_ue);
                        /* Start T3413 */
                        tm_start(amf4g_ue->t3413);
                    }

                    break;
                }
                default :
                {
                    d_error("Not support N11 Message");
                }    
            }

        release_amf_n11_pkbuf:
            pkbuf_free(recvbuf);
            break;
        }
        default:
        {
            d_error("No handler for event %s", amf4g_event_get_name(e));
            break;
        }
    }
}
