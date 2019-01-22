#define TRACE_MODULE _smf_n11_handler

#include "core.h"

#include "smf_n11_handler.h"
#include "smf_n11_build.h"
#include "smf_pfcp_path.h"

void smf_n11_handle_create_session_request(smf_sess_t *sess, create_session_t *createSession)
{
    d_trace(3, "[SMF] N11 %s \n", __FUNCTION__);
    if (createSession->ue_pco.length != 0) {
        memcpy(sess->pco_buf, createSession->ue_pco.buffer, createSession->ue_pco.length);
        sess->pco_len = createSession->ue_pco.length;
    } else {
        d_assert(createSession->ue_pco.length != 0, return, "NO PCO");
    }
}

void smf_n11_handle_update_session_request(smf_sess_t *sess, modify_bearer_t *pModifyBearer)
{
    d_trace(3, "[SMF] N11 %s \n", __FUNCTION__);
    status_t rv;
    smf_bearer_t *bearer = NULL;

    bearer = smf_bearer_find_by_ebi(sess, pModifyBearer->ebi);
    d_assert(bearer, return, "Bearer Context Not Found");

    switch (pModifyBearer->sm_context_update_type)
    {
        case SM_CONTEXT_UPDATE_TYPE_MODIFY:
        {
            bearer->enb_s1u_teid = ntohl(pModifyBearer->enb_s1u_teid);
            bearer->addr = pModifyBearer->enb_s1u_ip.addr;

            sess->dl_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_FORW;

            d_trace(5, "    ENB_S1U_TEID[%d] UPF_S1U_TEID[%d]\n",
                bearer->upf_s1u_teid, bearer->enb_s1u_teid);

            rv = smf_pfcp_send_setup_donwlink_session_modification_request(sess);
            d_assert(rv == CORE_OK, , "pfcp session modification fail");
            break;
        }
        case SM_CONTEXT_UPDATE_TYPE_RELEASE_ACCESS:
        {
            sess->dl_pdr->far->apply_action = PFCP_FAR_APPLY_ACTION_DROP;

            rv = smf_pfcp_send_an_release_session_modification_request(sess);
            d_assert(rv == CORE_OK, , "pfcp session modification fail");
            break;
        }
    }

    sess->sm_context_update_type = pModifyBearer->sm_context_update_type;
}

void smf_n11_handle_delete_session_request(
    smf_sess_t *sess, delete_session_t *pDeleteSession)
{
    d_trace(3, "[SMF] N11 %s \n", __FUNCTION__);
}
