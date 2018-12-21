#ifndef __AMF4G_PATH_H__
#define __AMF4G_PATH_H__

#include "core_errno.h"

#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf4g_send_delete_session_or_detach(amf4g_ue_t *amf4g_ue);
CORE_DECLARE(status_t) amf4g_send_delete_session_or_ue_context_release(
        amf4g_ue_t *amf4g_ue, enb_ue_t *enb_ue);
CORE_DECLARE(status_t) amf4g_send_release_access_bearer_or_ue_context_release(
        amf4g_ue_t *amf4g_ue, enb_ue_t *enb_ue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF4G_PATH_H__ */
