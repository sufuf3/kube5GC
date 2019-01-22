#define TRACE_MODULE _upf_sm

#include "core_debug.h"
#include "core_lib.h"

#include "upf_sm.h"
#include "upf_context.h"
#include "upf_event.h"
#include "upf_n4_handler.h"
#include "upf_pfcp_path.h"
#include "upf_gtp_path.h"

void upf_state_initial(fsm_t *s, event_t *e)
{
    upf_sm_trace(3, e);

    d_assert(s, return, "Null param");

    FSM_TRAN(s, &upf_state_operational);
}

void upf_state_final(fsm_t *s, event_t *e)
{
    upf_sm_trace(3, e);

    d_assert(s, return, "Null param");
}

void upf_state_operational(fsm_t *s, event_t *e)
{
    status_t rv;

    upf_sm_trace(3, e);

    d_assert(s, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            rv = upf_gtp_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish UPF path");
                break;
            }
            rv = upf_pfcp_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish UPF path (PFCP)");
                break;
            }
            break;
        }

        case FSM_EXIT_SIG:
        {
            rv = upf_gtp_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close UPF path");
                break;
            }

            rv = upf_pfcp_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close UPF path (PFCP)");
                break;
            }
            break;
        }

        case UPF_EVT_N4_MESSAGE:
        {
            status_t rv;
            pkbuf_t *recvbuf = (pkbuf_t *)event_get_param1(e);
            pkbuf_t *copybuf = NULL;
            c_uint16_t copybuf_len = 0;
            pfcp_node_t *upf = (pfcp_node_t *)event_get_param2(e);
            pfcp_xact_t *xact = NULL;
            pfcp_message_t *message = NULL;
            upf_sess_t *sess = NULL;

            d_assert(recvbuf, break, "Null param");
            d_assert(upf, break, "Null param");

            copybuf_len = sizeof(pfcp_message_t);
            copybuf = pkbuf_alloc(0, copybuf_len);
            d_assert(copybuf, break, "Null param");
            message = copybuf->payload;
            d_assert(message, break, "Null param");

            rv = pfcp_parse_msg(message, recvbuf);
            d_assert(rv == CORE_OK,
                    pkbuf_free(recvbuf);
                    pkbuf_free(copybuf); break,
                                        "parse error");

            if (!message->h.seid_p)
            {
                rv = pfcp_xact_receive(upf, &message->h, &xact);
                if (rv != CORE_OK)
                {
                    pkbuf_free(recvbuf);
                    pkbuf_free(copybuf);
                    break;
                }

                switch (message->h.type)
                {
                case PFCP_HEARTBEAT_REQUEST_TYPE:
                        upf_n4_handle_heartbeat_request(
                            xact, &message->pfcp_heartbeat_request);
                        break;
                    case PFCP_HEARTBEAT_RESPONSE_TYPE:
                        upf_n4_handle_heartbeat_response(
                            xact, &message->pfcp_heartbeat_response);
                        break;
                    case PFCP_ASSOCIATION_SETUP_REQUEST_TYPE:
                        upf_n4_handle_association_setup_request(
                            xact, &message->pfcp_association_setup_request);
                        break;
                    case PFCP_ASSOCIATION_UPDATE_REQUEST_TYPE:
                        upf_n4_handle_association_update_request(
                            xact, &message->pfcp_association_update_request);
                        break;
                    case PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE:
                        upf_n4_handle_association_release_request(
                            xact, &message->pfcp_association_release_request);
                        break;
                    // case PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE:
                    //     upf_n4_handle_session_establishment_response(
                    //         xact, &message->pfcp_version_not_supported_response);
                    //     break;
                    default:
                        d_error("Not implemented PFCP message type (%d)", message->h.type);
                        break;
                }
                pkbuf_free(recvbuf);
                pkbuf_free(copybuf);
                break;
            }

            d_assert(message->h.seid_p, pkbuf_free(recvbuf); pkbuf_free(copybuf); break,
                                                                                "No Session seid");
            if (!message->h.seid)
            {
                if (message->h.type == PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE)
                    sess = upf_sess_add_by_message(message);
                else
                    d_assert(0, pkbuf_free(recvbuf); pkbuf_free(copybuf); return,
                                                                                "Session seid 0 but not session establish");
            }
            else
            {
                sess = upf_sess_find_by_seid(message->h.seid);
                d_trace(-1, "upf_sess_smf_seid : %lu, upf_sess_upf_seid: %lu", sess->smf_seid, sess->upf_seid);
            }
            // sess = upf_sess_find_by_seid(message->h.seid);
            // if (!sess && message->h.type==PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE)
            // {
            //     sess = upf_sess_add_by_message(message);
            // }
            d_assert(sess, pkbuf_free(recvbuf); pkbuf_free(copybuf); break, "No Session Context");
            sess->pnode = upf;

            rv = pfcp_xact_receive(sess->pnode, &message->h, &xact);
            if (rv != CORE_OK)
            {
                pkbuf_free(recvbuf);
                pkbuf_free(copybuf);
                break;
            }

            switch (message->h.type)
            {
                case PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE:
                    upf_n4_handle_session_establishment_request(
                        sess, xact, &message->pfcp_session_establishment_request);
                    break;
                case PFCP_SESSION_MODIFICATION_REQUEST_TYPE:
                    upf_n4_handle_session_modification_request(
                        sess, xact, &message->pfcp_session_modification_request);
                    break;
                case PFCP_SESSION_DELETION_REQUEST_TYPE:
                    upf_n4_handle_session_deletion_request(
                        sess, xact, &message->pfcp_session_deletion_request);
                    break;
                case PFCP_SESSION_REPORT_RESPONSE_TYPE:
                    upf_n4_handle_session_report_response(
                        sess, xact, &message->pfcp_session_report_response);
                    break;
                default:
                    break;
            }
            
            pkbuf_free(recvbuf);
            pkbuf_free(copybuf);
            break;
        }

        case UPF_EVT_N4_T3_RESPONSE:
        case UPF_EVT_N4_T3_HOLDING:
        {
            c_uint8_t type;
            pfcp_xact_timeout(event_get_param1(e), event_get(e), &type);
            break;
        }

        default:
        {
            d_error("No handler for event %s", upf_event_get_name(e));
            break;
        }
    }
}
