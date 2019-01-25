#define TRACE_MODULE _smf_sm
#include "core_debug.h"
#include "core_lib.h"
#include "core_pkbuf.h"

#include "smf_context.h"
#include "smf_event.h"
#include "smf_pfcp_path.h"
#include "smf_sbi_path.h"
#include "smf_n4_handler.h"
#include "smf_n11_handler.h"
#include "smf_gx_handler.h"
#include "smf_fd_path.h"
#include "smf_sm.h"

#include "pfcp/pfcp_message.h"

#include "sbiJson/JsonTransform.h"
#include "smf_json_handler.h"

void smf_state_initial(fsm_t *s, event_t *e)
{
    smf_sm_trace(3, e);

    d_assert(s, return, "Null param");

    FSM_TRAN(s, &smf_state_operational);
}

void smf_state_final(fsm_t *s, event_t *e)
{
    smf_sm_trace(3, e);

    d_assert(s, return, "Null param");
}

void smf_state_operational(fsm_t *s, event_t *e)
{
    status_t rv;
    smf_sm_trace(3, e);

    d_assert(s, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            rv = smf_pfcp_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish N4-PFCP path");
                break;
            }
            rv = smf_sbi_server_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish SMF-SBI path");
                break;
            }
            break;

        }
        case FSM_EXIT_SIG:
        {
            rv = smf_pfcp_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close N4-PFCP path");
                break;
            }
            rv = smf_sbi_server_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close SMF-SBI path");
                break;
            }
            break;
        }
        case SMF_EVT_N4_MESSAGE:
        {
            status_t rv;
            pkbuf_t *recvbuf = (pkbuf_t *)event_get_param1(e);
            pkbuf_t *copybuf = NULL;
            c_uint16_t copybuf_len = 0;
            pfcp_node_t *upf = (pfcp_node_t *)event_get_param2(e);
            pfcp_xact_t *xact = NULL;
            pfcp_message_t *message = NULL;
            smf_sess_t *sess = NULL;

            d_assert(recvbuf, break, "Null param");
            d_assert(upf, break, "Null param");
            
            copybuf_len = sizeof(pfcp_message_t);
            copybuf = pkbuf_alloc(0, copybuf_len);
            d_assert(copybuf, break, "Null param");
            message = copybuf->payload;
            d_assert(message, break, "Null param");

            rv = pfcp_parse_msg(message, recvbuf);
            d_assert(rv == CORE_OK,
                    pkbuf_free(recvbuf); pkbuf_free(copybuf); break,
                    "parse error");
            d_trace(9, "N4 Message: [Type: %d, SEID: %016llx]\n", 
                    message->h.type,
                    message->h.seid_p ? message->h.seid : 0);
            if (!message->h.seid_p)
            {
                rv = pfcp_xact_receive(upf, &message->h, &xact);
                if (rv != CORE_OK)
                {
                    pkbuf_free(recvbuf);
                    pkbuf_free(copybuf);
                    break;
                }
                
                switch(message->h.type)
                {
                    case PFCP_HEARTBEAT_REQUEST_TYPE:
                        smf_n4_handle_heartbeat_request(
                            xact, &message->pfcp_heartbeat_request);
                        break;        
                    case PFCP_HEARTBEAT_RESPONSE_TYPE:
                        smf_n4_handle_heartbeat_response(
                            xact, &message->pfcp_heartbeat_response);
                        break; 
                    case PFCP_ASSOCIATION_SETUP_RESPONSE_TYPE:
                        smf_n4_handle_association_setup_response(
                            xact, &message->pfcp_association_setup_response);
                        break;
                    case PFCP_ASSOCIATION_UPDATE_RESPONSE_TYPE:
                        smf_n4_handle_association_update_response(
                            xact, &message->pfcp_association_update_response);
                        break;
                    case PFCP_ASSOCIATION_RELEASE_RESPONSE_TYPE:
                        smf_n4_handle_association_release_response(
                            xact, &message->pfcp_association_release_response);
                        break;
                    case PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE:
                        d_error("PFCP peer version not support");
                        rv = pfcp_xact_commit(xact);
                        d_assert(rv == CORE_OK, return, "xact_commit error");                        
                        pfcp_remove_node(&smf_self()->upf_n4_list, upf);
                        break;    
                    default:
                        d_error("Not implemented PFCP message type (%d)", message->h.type);
                        break;
                }
                pkbuf_free(recvbuf);
                pkbuf_free(copybuf);
                break;
            }

            d_assert(message->h.seid, pkbuf_free(recvbuf); pkbuf_free(copybuf); break,
                    "No Session seid");
            sess = smf_sess_find_by_seid(message->h.seid);
            d_assert(sess, pkbuf_free(recvbuf); pkbuf_free(copybuf); break,
                    "No Session Context");

            rv = pfcp_xact_receive(sess->upf_node, &message->h, &xact);
            if (rv != CORE_OK)
            {
                pkbuf_free(recvbuf);
                pkbuf_free(copybuf);
                break;
            }

            switch(message->h.type)
            {
                case PFCP_SESSION_ESTABLISHMENT_RESPONSE_TYPE:
                    smf_n4_handle_session_establishment_response(
                        xact, sess, &message->pfcp_session_establishment_response);
                    break;
                case PFCP_SESSION_MODIFICATION_RESPONSE_TYPE:
                    smf_n4_handle_session_modification_response(
                        xact, sess, &message->pfcp_session_modification_response);
                    break;
                case PFCP_SESSION_DELETION_RESPONSE_TYPE:
                    smf_n4_handle_session_deletion_response(
                        xact, sess,&message->pfcp_session_deletion_response);
                    break;
                case PFCP_SESSION_REPORT_REQUEST_TYPE:
                    smf_n4_handle_session_report_request(
                        xact, sess, &message->pfcp_session_report_request);
                    break;
                default:
                    break;
            }
            pkbuf_free(recvbuf);
            pkbuf_free(copybuf); 
            break;
        }
        case SMF_EVT_N4_T3_RESPONSE:
        case SMF_EVT_N4_T3_HOLDING:
        {
            status_t rv;
            c_uint8_t type;
            
            rv = pfcp_xact_timeout(event_get_param1(e), event_get(e), &type);
            if (rv == CORE_ERROR && type == PFCP_HEARTBEAT_REQUEST_TYPE)
            {
                d_error("Heartbeat fail, should delete association");
            }
            break;
        }
        case SMF_EVT_N11_MESSAGE:
        {
            pkbuf_t *recvbuf = (pkbuf_t *)event_get_param1(e);
            int msgType = event_get_param2(e);
            smf_sess_t *sess = NULL;
            d_info("N11 message enter");
            switch (msgType)
            {
                case N11_TYPE_SM_CONTEXT_CREATE:
                {
                    create_session_t createSession = {0};
                    smf_json_handler_create_session(&recvbuf, &createSession);
                    sess = smf_sess_add_or_find_by_JsonCreateSession(&createSession);
                    smf_n11_handle_create_session_request(sess, &createSession);
                    smf_gx_send_ccr(sess, GX_CC_REQUEST_TYPE_INITIAL_REQUEST);
                    d_assert(recvbuf, goto release_n11_pkbuf, "Null param");
                    break;
                }
                case N11_TYPE_SM_CONTEXT_UPDATE:
                {
                    modify_bearer_t modifyBearer = {0};
                    smf_json_handler_update_session(&recvbuf, &modifyBearer);
                    sess = smf_sess_find_by_JsonUpdateSession(&modifyBearer);
                    smf_n11_handle_update_session_request(sess, &modifyBearer);
                    d_assert(recvbuf, goto release_n11_pkbuf, "Null param");
                    break;
                }
                case N11_TYPE_SM_CONTEXT_RELEASE:
                {
                    delete_session_t deleteSession= {0};
                    d_trace(3, "[SMF] N11 Context Release\n");
                    d_warn("%s", recvbuf->payload);
                    smf_json_handler_delete_session(&recvbuf, &deleteSession);
                    sess = smf_sess_find_by_imsi_apn(deleteSession.imsi, deleteSession.imsi_len, deleteSession.apn);
                    d_assert(sess, goto release_n11_pkbuf, "No Session Context");

                    smf_n11_handle_delete_session_request(sess, &deleteSession);
                    d_assert(recvbuf, goto release_n11_pkbuf, "Null param");
                    smf_gx_send_ccr(sess, GX_CC_REQUEST_TYPE_TERMINATION_REQUEST);
                    break;
                }
                default:
                {
                    d_error("Not support N11 Message");
                }
            }

        release_n11_pkbuf:
            pkbuf_free(recvbuf);
            break;
        }
        case SMF_EVT_GX_MESSAGE:
        {
            index_t sess_index = event_get_param1(e);
            smf_sess_t *sess = NULL;
            pkbuf_t *gxbuf = (pkbuf_t *)event_get_param2(e);
            gx_message_t *gx_message = NULL;

            d_assert(sess_index, return, "Null param");
            sess = smf_sess_find(sess_index);
            d_assert(sess, return, "Null param");

            d_assert(gxbuf, return, "Null param");
            gx_message = gxbuf->payload;
            d_assert(gx_message, return, "Null param");

            switch(gx_message->cmd_code)
            {
                case GX_CMD_CODE_CREDIT_CONTROL:
                {
                    if (gx_message->result_code != ER_DIAMETER_SUCCESS)
                    {
                        d_error("Diameter Error(%d)", gx_message->result_code);
                        break;
                    }
                    switch(gx_message->cc_request_type)
                    {
                        case GX_CC_REQUEST_TYPE_INITIAL_REQUEST:
                        {
                            smf_gx_handle_cca_initial_request(
                                    sess, gx_message);
                            break;
                        }
                        case GX_CC_REQUEST_TYPE_TERMINATION_REQUEST:
                        {
                            smf_gx_handle_cca_termination_request(
                                    sess, gx_message);
                            break;
                        }
                        default:
                        {
                            d_error("Not implemented(%d)", event_get_param4(e));
                            break;
                        }
                    }

                    break;
                }
                case GX_CMD_RE_AUTH:
                {
                    smf_gx_handle_re_auth_request(sess, gx_message);
                    break;
                }
                default:
                {
                    d_error("Invalid type(%d)", event_get_param3(e));
                    break;
                }
            }

            gx_message_free(gx_message);
            pkbuf_free(gxbuf);
            break;
        }
        default:
        {
            d_error("No handler for event %s", smf_event_get_name(e));
            break;
        }
    }
}
