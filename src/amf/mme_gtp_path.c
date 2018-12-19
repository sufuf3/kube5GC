#define TRACE_MODULE _mme_gtp_path
#include "core_debug.h"
#include "core_pkbuf.h"

#include "gtp/gtp_node.h"
#include "gtp/gtp_path.h"
#include "gtp/gtp_xact.h"

#include "amf_sbi_path.h"

#include "mme_event.h"
#include "mme_gtp_path.h"
#include "mme_s11_build.h"
#include "amf_json_build.h"
#include "amf_json_handler.h"
#include "mme_sm.h"

status_t mme_gtp_send_create_session_request(mme_sess_t *sess)
{
    d_trace(-1, "mme_gtp_send_create_session_request");
#ifndef FIVE_G_CORE
    status_t rv;
    gtp_header_t h;
    pkbuf_t *pkbuf = NULL;
    gtp_xact_t *xact = NULL;
    mme_ue_t *mme_ue = NULL;

    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");

    memset(&h, 0, sizeof(gtp_header_t));
    h.type = GTP_CREATE_SESSION_REQUEST_TYPE;
    h.teid = mme_ue->sgw_s11_teid;

    rv = mme_s11_build_create_session_request(&pkbuf, h.type, sess);

    d_assert(rv == CORE_OK, return CORE_ERROR,
            "S11 build error");

    xact = gtp_xact_local_create(mme_ue->gnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "Null param");

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

    return CORE_OK;
}
#else
    mme_ue_t *mme_ue = NULL;
    pkbuf_t *pkbuf = NULL;
    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");
    
    amf_json_build_create_session(&pkbuf, sess);

    amf_sbi_send_sm_context_create(pkbuf);

    pkbuf_free(pkbuf);
#if 0 // TempDisable Test
    amf_json_handle_create_session(&pkbuf, sess); 
#endif

    return CORE_OK;


}
#endif

status_t mme_gtp_send_modify_bearer_request(
        mme_bearer_t *bearer, int uli_presence)
{
    d_trace(-1, "mme_gtp_send_modify_bearer_request");
#ifndef FIVE_G_CORE
    status_t rv;

    gtp_xact_t *xact = NULL;
    mme_ue_t *mme_ue = NULL;

    gtp_header_t h;
    pkbuf_t *pkbuf = NULL;

    d_assert(bearer, return CORE_ERROR, "Null param");
    mme_ue = bearer->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");

    memset(&h, 0, sizeof(gtp_header_t));
    h.type = GTP_MODIFY_BEARER_REQUEST_TYPE;
    h.teid = mme_ue->sgw_s11_teid;

    rv = mme_s11_build_modify_bearer_request(
            &pkbuf, h.type, bearer, uli_presence);
    d_assert(rv == CORE_OK, return CORE_ERROR, "S11 build error");

    xact = gtp_xact_local_create(mme_ue->gnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "Null param");

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

    return CORE_OK;
#else
    mme_ue_t *mme_ue = NULL;
    pkbuf_t *pkbuf = NULL;
    mme_ue = bearer->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");
    amf_json_build_modify_bearer(&pkbuf, bearer, SM_CONTEXT_UPDATE_TYPE_MODIFY);
    // d_info("send sbi sm context update start");
    amf_sbi_send_sm_context_update(pkbuf);
    // d_info("send sbi sm context update end");
    pkbuf_free(pkbuf);
    return CORE_OK;
#endif
}

// TODO: release session
status_t mme_gtp_send_delete_session_request(mme_sess_t *sess)
{
    d_trace(-1, "mme_gtp_send_delete_session_request");
#ifndef FIVE_G_CORE
    status_t rv;
    pkbuf_t *s11buf = NULL;
    gtp_header_t h;
    gtp_xact_t *xact = NULL;
    mme_ue_t *mme_ue = NULL;

    d_assert(sess, return CORE_ERROR, "Null param");
    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");

    memset(&h, 0, sizeof(gtp_header_t));
    h.type = GTP_DELETE_SESSION_REQUEST_TYPE;
    h.teid = mme_ue->sgw_s11_teid;

    rv = mme_s11_build_delete_session_request(&s11buf, h.type, sess);
    d_assert(rv == CORE_OK, return CORE_ERROR, "S11 build error");

    xact = gtp_xact_local_create(mme_ue->gnode, &h, s11buf);
    d_assert(xact, return CORE_ERROR, "Null param");

    GTP_XACT_STORE_SESSION(xact, sess);

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");

    return CORE_OK;
#else
    mme_ue_t *mme_ue = NULL;
    pkbuf_t *pkbuf = NULL;
    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");
    
    amf_json_build_delete_session(&pkbuf, sess);

    amf_sbi_send_sm_context_release(pkbuf);

    pkbuf_free(pkbuf);

    return CORE_OK;
#endif
}

status_t mme_gtp_send_delete_all_sessions(mme_ue_t *mme_ue)
{
    d_trace(-1, "mme_gtp_send_delete_all_sessions");
    status_t rv;
    mme_sess_t *sess = NULL, *next_sess = NULL;

    d_assert(mme_ue, return CORE_ERROR, "Null param");
    sess = mme_sess_first(mme_ue);
    while (sess != NULL)
    {
        next_sess = mme_sess_next(sess);

        if (MME_HAVE_SGW_S1U_PATH(sess))
        {
            mme_bearer_t *bearer = mme_default_bearer_in_sess(sess);
            d_assert(bearer,, "Null param");

            if (bearer && FSM_CHECK(&bearer->sm, esm_state_pdn_will_disconnect))
            {
                d_warn("PDN will disconnect[EBI:%d]", bearer->ebi);
            }
            else
            {
                rv = mme_gtp_send_delete_session_request(sess);
                d_assert(rv == CORE_OK, return CORE_ERROR,
                        "mme_gtp_send_delete_session_request error");
            }
        }
        else
        {
            mme_sess_remove(sess);
        }

        sess = next_sess;
    }

    return CORE_OK;
}

status_t mme_gtp_send_release_access_bearers_request(mme_ue_t *mme_ue)
{
    d_trace(-1, "mme_gtp_send_release_access_bearers_request");
#ifndef FIVE_G_CORE
    status_t rv;
    gtp_header_t h;
    pkbuf_t *pkbuf = NULL;
    gtp_xact_t *xact = NULL;
    d_assert(mme_ue, return CORE_ERROR, "Null param");

    memset(&h, 0, sizeof(gtp_header_t));
    h.type = GTP_RELEASE_ACCESS_BEARERS_REQUEST_TYPE;
    h.teid = mme_ue->sgw_s11_teid;

    rv = mme_s11_build_release_access_bearers_request(&pkbuf, h.type);
    d_assert(rv == CORE_OK, return CORE_ERROR, "S11 build error");

    xact = gtp_xact_local_create(mme_ue->gnode, &h, pkbuf);
    d_assert(xact, return CORE_ERROR, "Null param");

    rv = gtp_xact_commit(xact);
    d_assert(rv == CORE_OK, return CORE_ERROR, "xact_commit error");
    return CORE_OK;
#else
    mme_bearer_t *bearer = NULL;
    pkbuf_t *pkbuf = NULL;
    d_assert(mme_ue, return CORE_ERROR, "Null param");
    
    bearer = mme_bearer_find_by_ue_ebi(mme_ue, mme_ue->ebi);

    bearer->enb_s1u_teid = 0;
    amf_json_build_modify_bearer(&pkbuf, bearer, SM_CONTEXT_UPDATE_TYPE_RELEASE_ACCESS);
    // d_info("send sbi sm context update start");
    amf_sbi_send_sm_context_update(pkbuf);
    // d_info("send sbi sm context update end");
    pkbuf_free(pkbuf);

    return CORE_OK;
#endif
}
