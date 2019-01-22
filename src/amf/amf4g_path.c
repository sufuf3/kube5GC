#define TRACE_MODULE _amf4g_path

#include "s1ap_path.h"
#include "nas_path.h"
#include "amf_n11_path.h"
#include "amf4g_path.h"

status_t amf4g_send_delete_session_or_detach(amf4g_ue_t *amf4g_ue)
{
    
    status_t rv;
    d_assert(amf4g_ue, return CORE_ERROR,);

    if (SESSION_CONTEXT_IS_AVAILABLE(amf4g_ue))
    {
        rv = amf_n11_send_delete_all_sessions(amf4g_ue);
        d_assert(rv == CORE_OK,,
            "amf_n11_send_delete_all_sessions failed");
    }
    else
    {
        
        rv = nas_send_detach_accept(amf4g_ue);
        d_assert(rv == CORE_OK,,
            "nas_send_detach_accept failed");
    }

    return rv;
}

status_t amf4g_send_delete_session_or_ue_context_release(
        amf4g_ue_t *amf4g_ue, enb_ue_t *enb_ue)
{
    status_t rv;

    d_assert(enb_ue, return CORE_ERROR,);

    if (SESSION_CONTEXT_IS_AVAILABLE(amf4g_ue))
    {
        rv = amf_n11_send_delete_all_sessions(amf4g_ue);
        d_assert(rv == CORE_OK,,
            "amf_n11_send_delete_all_sessions failed");
    }
    else
    {
        rv = s1ap_send_ue_context_release_command(enb_ue,
                S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                S1AP_UE_CTX_REL_UE_CONTEXT_REMOVE, 0);
        d_assert(rv == CORE_OK,, "s1ap send error");
    }

    return rv;
}

status_t amf4g_send_release_access_bearer_or_ue_context_release(
        amf4g_ue_t *amf4g_ue, enb_ue_t *enb_ue)
{
    status_t rv;

    d_assert(enb_ue, return CORE_ERROR,);

    if (BEARER_CONTEXT_IS_ACTIVE(amf4g_ue))
    {
        rv = amf_n11_send_release_access_bearers_request(amf4g_ue);
        d_assert(rv == CORE_OK,, "gtp send failed");
    }
    else
    {
        rv = s1ap_send_ue_context_release_command(enb_ue,
                S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                S1AP_UE_CTX_REL_S1_NORMAL_RELEASE, 0);
        d_assert(rv == CORE_OK,, "s1ap send error");
    }

    return rv;
}
