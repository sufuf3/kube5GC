#define TRACE_MODULE _smf_n11_handler

#include "core.h"

#include "smf_n11_handler.h"
#include "smf_n11_build.h"
#include "smf_pfcp_path.h"

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

    d_trace(-1, "SM Context Update %d\n", pModifyBearer->sm_context_update_type);
    d_trace(-1, "SM addr %d\n", sess);
    sess->sm_context_update_type = pModifyBearer->sm_context_update_type;

    rv = smf_pfcp_send_session_modification_request(sess);
    d_assert(rv == CORE_OK, , "pfcp session modification fail");
}

void smf_n11_handle_delete_session_request_by_JsonDeleteSession(
    smf_sess_t *sess, delete_session_t *pDeleteSession)
{
}
