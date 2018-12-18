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

    gtp_f_teid_t *mme_s11_teid = NULL;

    /* Receive Control Plane(DL) : MME-S11 */
    mme_s11_teid = req->sender_f_teid_for_control_plane.data;
    d_assert(mme_s11_teid, return, "Null param");
    sess->mme_s11_teid = ntohl(mme_s11_teid->teid);

    rv = smf_pfcp_send_session_establishment_request(sess);
    d_assert(rv == CORE_OK, , "pfcp session create fail");
    
    sess->s11_xact = s11_xact;
}

void smf_s11_handle_delete_session_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_delete_session_request_t *req)
{
    d_trace(3, "[SMF] S11 Delete Session Reqeust\n");
}

void smf_s11_handle_modify_bearer_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_modify_bearer_request_t *req)
{
    d_trace(3, "[SMF] S11 Modify Bearer Reqeust\n");

    status_t rv;
    pkbuf_t *pkbuf = NULL;
    smf_bearer_t *bearer = NULL;
    gtp_header_t h;
    gtp_f_teid_t *enb_s1u_teid = NULL;

    if (req->bearer_contexts_to_be_modified.presence == 0)
    {
        d_error("No Bearer Context");
        return;
    }
    
    if (req->bearer_contexts_to_be_modified.eps_bearer_id.presence == 0)
    {
        d_error("No EPS Bearer ID");
        return;
    }

    if (req->bearer_contexts_to_be_modified.s1_u_enodeb_f_teid.presence == 0)
    {
        d_error("No eNB TEID");
        return;
    }

    bearer = smf_bearer_find_by_ebi(sess, req->bearer_contexts_to_be_modified.eps_bearer_id.u8);
    d_assert(bearer, return, "Bearer Context Not Found");

    enb_s1u_teid = req->bearer_contexts_to_be_modified.s1_u_enodeb_f_teid.data;
    bearer->enb_s1u_teid = ntohl(enb_s1u_teid->teid);
    bearer->addr = enb_s1u_teid->addr;

    d_trace(5, "    MME_S11_TEID[%d] SGW_S11_TEID[%d]\n",
        sess->mme_s11_teid, sess->sgw_s11_teid);
    d_trace(5, "    ENB_S1U_TEID[%d] SGW_S1U_TEID[%d]\n",
        bearer->sgw_s1u_teid, bearer->enb_s1u_teid);

    memset(&h, 0, sizeof(gtp_header_t));
    h.type = GTP_MODIFY_BEARER_RESPONSE_TYPE;
    h.teid = sess->mme_s11_teid;

    rv = smf_pfcp_send_session_modification_request(sess);
    d_assert(rv == CORE_OK, , "pfcp session modification fail");

    rv = smf_s11_build_modify_bearer_response(
        &pkbuf, sess);
    d_assert(rv == CORE_OK, return, "gtp build error");

    rv = gtp_xact_update_tx(s11_xact, &h, pkbuf);
    d_assert(rv == CORE_OK, return, "gtp_xact_update_tx error");

    rv = gtp_xact_commit(s11_xact);
    d_assert(rv == CORE_OK, return, "xact_commit error");
}

void smf_n11_handle_create_session_request_by_JsonCreateSession(smf_sess_t *sess, create_session_t *createSession)
{
    d_trace(3, "[SMF] N11 Create Session Reqeust by_JsonCreateSession\n");
    d_info("%s:%d(%s)", __FILE__, __LINE__, __FUNCTION__);
    if (createSession->ue_pco.length != 0) {
        memcpy(sess->pco_buf, createSession->ue_pco.buffer, createSession->ue_pco.length);
        sess->pco_len = createSession->ue_pco.length;
    } else {
        d_assert(createSession->ue_pco.length != 0, return, "NO PCO");
    }
}

void smf_n11_handle_update_session_request_by_JsonUpdateSession(smf_sess_t *sess, modify_bearer_t *pModifyBearer)
{
    d_trace(3, "[SMF] N11 update Session Reqeust by_JsonUpdateSession\n");
    status_t rv;
    smf_bearer_t *bearer = NULL;

    bearer = smf_bearer_find_by_ebi(sess, pModifyBearer->ebi);
    d_assert(bearer, return, "Bearer Context Not Found");

    
    bearer->enb_s1u_teid = ntohl(pModifyBearer->enb_s1u_teid);
    bearer->addr = pModifyBearer->enb_s1u_ip.addr;

    d_trace(5, "    MME_S11_TEID[%d] SGW_S11_TEID[%d]\n",
        sess->mme_s11_teid, sess->sgw_s11_teid);
    d_trace(5, "    ENB_S1U_TEID[%d] SGW_S1U_TEID[%d]\n",
        bearer->sgw_s1u_teid, bearer->enb_s1u_teid);

    rv = smf_pfcp_send_session_modification_request(sess);
    d_assert(rv == CORE_OK, , "pfcp session modification fail");

    d_trace(-1, "SM Context Update %d\n", pModifyBearer->sm_context_update_type);
    sess->sm_context_update_type = pModifyBearer->sm_context_update_type;

}

void smf_n11_handle_delete_session_request_by_JsonDeleteSession(
    smf_sess_t *sess, delete_session_t *pDeleteSession)
{
}
