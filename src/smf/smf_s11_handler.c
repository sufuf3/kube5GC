#define TRACE_MODULE _smf_s11_handler

#include "core.h"

#include "smf_s11_handler.h"
#include "smf_s11_build.h"
#include "smf_pfcp_path.h"

void smf_s11_handle_create_session_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_create_session_request_t *req)
{
    d_trace(3, "[SMF] S11 Create Session Reqeust\n");
    status_t rv;
    
    if (req->bearer_contexts_to_be_created.presence == 0)
    {
        d_error("No Bearer");
        return;
    }
    
    if (req->bearer_contexts_to_be_created.eps_bearer_id.presence == 0)
    {
        d_error("No EPS Bearer ID");
        return;
    }
    rv = smf_pfcp_send_session_establishment_request(sess);
    d_assert(rv == CORE_OK, , "pfcp session create fail");
    
    sess->s11_xact = s11_xact;
}

void smf_s11_handle_delete_session_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_delete_session_request_t *req)
{
    d_trace(3, "[SMF] S11 Delete Session Reqeust\n");
}