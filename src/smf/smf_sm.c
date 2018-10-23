#define TRACE_MODULE _smf_sm
#include "core_debug.h"
#include "core_lib.h"
#include "core_pkbuf.h"

#include "smf_context.h"
#include "smf_event.h"
#include "smf_gtp_path.h"
#include "smf_pfcp_path.h"
#include "smf_n4_handler.h"
#include "smf_s11_handler.h"
#include "smf_gx_handler.h"
#include "smf_sm.h"

#include "gtp/gtp_message.h"
#include "pfcp/pfcp_message.h"


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
            rv = smf_gtp_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish S11-GTP-C path");
                break;
            }
            rv = smf_pfcp_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish N4-PFCP path");
                break;
            }
            break;
        }
        case FSM_EXIT_SIG:
        {
            rv = smf_gtp_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close S11-GTP-C path");
                break;
            }
            rv = smf_pfcp_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close N4-PFCP path");
                break;
            }
            break;
        }
        case SMF_EVT_S11_MESSAGE:
        {
            status_t rv;
            pkbuf_t *recvbuf = (pkbuf_t *)event_get_param1(e);
            gtp_message_t s11_message;
            gtp_xact_t *s11_xact = NULL;
            smf_sess_t *sess = NULL;


            rv = gtp_parse_msg(&s11_message, recvbuf);
            d_assert(rv == CORE_OK, goto release_s11_pkbuf;,);
            
            sess = smf_sess_add_or_find_by_message(&s11_message);
            d_assert(sess, goto release_s11_pkbuf;,);

            rv = gtp_xact_receive(sess->mme_node, &s11_message.h, &s11_xact);
            d_assert(rv == CORE_OK, goto release_s11_pkbuf;,);

            d_info("%lu", sess->ipv4->addr[0]);

            switch (s11_message.h.type)
            {
                case GTP_CREATE_SESSION_REQUEST_TYPE:
                {
                    smf_s11_handle_create_session_request(s11_xact, sess, &s11_message.create_session_request);
                    break;
                }
                case GTP_DELETE_SESSION_REQUEST_TYPE:
                {
                    smf_s11_handle_delete_session_request(s11_xact, sess, &s11_message.delete_session_request);
                    break;
                }
                default:
                {
                    d_error("No handler for event %s", smf_event_get_name(e));
                    break;
                }
            }

        release_s11_pkbuf:
            pkbuf_free(recvbuf);
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
            d_trace(3, "message type: %d", message->h.type);
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
                        xact, sess,&message->pfcp_session_modification_response);
                    break;
                case PFCP_SESSION_DELETION_RESPONSE_TYPE:
                    smf_n4_handle_session_deletion_response(
                        xact, sess,&message->pfcp_session_deletion_response);
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
                    index_t xact_index = event_get_param3(e);
                    gtp_xact_t *xact = NULL;

                    pkbuf_t *gtpbuf = (pkbuf_t *)event_get_param4(e);
                    gtp_message_t *message = NULL;

                    d_assert(xact_index, return, "Null param");
                    xact = gtp_xact_find(xact_index);
                    d_assert(xact, return, "Null param");

                    d_assert(gtpbuf, return, "Null param");
                    message = gtpbuf->payload;

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
                                    sess, gx_message, xact, 
                                    &message->create_session_request);
                            break;
                        }
                        case GX_CC_REQUEST_TYPE_TERMINATION_REQUEST:
                        {
                            smf_gx_handle_cca_termination_request(
                                    sess, gx_message, xact,
                                    &message->delete_session_request);
                            break;
                        }
                        default:
                        {
                            d_error("Not implemented(%d)", event_get_param4(e));
                            break;
                        }
                    }

                    pkbuf_free(gtpbuf);
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
