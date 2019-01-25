#define TRACE_MODULE _amf4g_gtp_path
#include "core_debug.h"
#include "core_pkbuf.h"

#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"
#include "gtp/gtp_xact.h"

#include "amf_sbi_path.h"

#include "amf4g_event.h"
#include "amf_n11_path.h"
#include "amf_json_build.h"
#include "amf_json_handler.h"
#include "amf4g_sm.h"

status_t amf_n11_send_create_session_request(amf4g_sess_t *sess)
{
    d_trace(10, "amf_n11_send_create_session_request");
    amf4g_ue_t *amf4g_ue = NULL;
    pkbuf_t *pkbuf = NULL;
    amf4g_ue = sess->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    
    amf_json_build_create_session(&pkbuf, sess);

    amf_sbi_send_sm_context_create(pkbuf);

    pkbuf_free(pkbuf);

    return CORE_OK;
}


status_t amf_n11_send_modify_bearer_request(
        amf4g_bearer_t *bearer, int uli_presence)
{
    d_trace(10, "amf_n11_send_modify_bearer_request");
    amf4g_ue_t *amf4g_ue = NULL;
    pkbuf_t *pkbuf = NULL;
    amf4g_ue = bearer->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    amf_json_build_modify_bearer(&pkbuf, bearer, SM_CONTEXT_UPDATE_TYPE_MODIFY);
    // d_info("send sbi sm context update start");
    amf_sbi_send_sm_context_update(pkbuf);
    // d_info("send sbi sm context update end");
    pkbuf_free(pkbuf);
    return CORE_OK;

}


status_t amf_n11_send_delete_session_request(amf4g_sess_t *sess)
{
    d_trace(10, "amf_n11_send_delete_session_request");
    amf4g_ue_t *amf4g_ue = NULL;
    pkbuf_t *pkbuf = NULL;
    amf4g_ue = sess->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    
    amf_json_build_delete_session(&pkbuf, sess);

    amf_sbi_send_sm_context_release(pkbuf);

    pkbuf_free(pkbuf);

    return CORE_OK;
}

status_t amf_n11_send_delete_all_sessions(amf4g_ue_t *amf4g_ue)
{
    d_trace(10, "amf_n11_send_delete_all_sessions");
    status_t rv;
    amf4g_sess_t *sess = NULL, *next_sess = NULL;

    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    sess = amf4g_sess_first(amf4g_ue);
    while (sess != NULL)
    {
        next_sess = amf4g_sess_next(sess);

        if (AMF4G_HAVE_UPF_S1U_PATH(sess))
        {
            amf4g_bearer_t *bearer = amf4g_default_bearer_in_sess(sess);
            d_assert(bearer,, "Null param");

            if (bearer && FSM_CHECK(&bearer->sm, esm_state_pdn_will_disconnect))
            {
                d_warn("PDN will disconnect[EBI:%d]", bearer->ebi);
            }
            else
            {
                rv = amf_n11_send_delete_session_request(sess);
                d_assert(rv == CORE_OK, return CORE_ERROR,
                        "amf_n11_send_delete_session_request error");
            }
        }
        else
        {
            amf4g_sess_remove(sess);
        }

        sess = next_sess;
    }

    return CORE_OK;
}

status_t amf_n11_send_release_access_bearers_request(amf4g_ue_t *amf4g_ue)
{
    d_trace(10, "amf_n11_send_release_access_bearers_request");

    pkbuf_t *pkbuf = NULL;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    amf4g_sess_t *sess = NULL;
    d_assert(amf4g_ue, return CORE_ERROR,);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        d_trace(-1, "release_access_bearers_request sess enter\n");
        amf4g_bearer_t *bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            d_trace(-1, "release_access_bearers_request bearer enter\n");
            CLEAR_ENB_S1U_PATH(bearer);
            amf_json_build_modify_bearer(&pkbuf, bearer, SM_CONTEXT_UPDATE_TYPE_RELEASE_ACCESS);
            amf_sbi_send_sm_context_update(pkbuf);
            pkbuf_free(pkbuf);

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
        d_trace(-1, "release_access_bearers_request sess exit enter\n");
    }
    return CORE_OK;
}
