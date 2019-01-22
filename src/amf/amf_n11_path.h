#ifndef __AMF_N11_PATH_H__
#define __AMF_N11_PATH_H__

#include "core_pkbuf.h"

#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf_n11_send_create_session_request(amf4g_sess_t *sess);
CORE_DECLARE(status_t) amf_n11_send_modify_bearer_request(
        amf4g_bearer_t *bearer, int uli_presence);
CORE_DECLARE(status_t) amf_n11_send_delete_session_request(amf4g_sess_t *sess);
CORE_DECLARE(status_t) amf_n11_send_delete_all_sessions(amf4g_ue_t *amf4g_ue);
CORE_DECLARE(status_t) amf_n11_send_release_access_bearers_request(amf4g_ue_t *amf4g_ue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_N11_PATH_H__ */
