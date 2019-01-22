#ifndef __AMF4G_FD_PATH_H__
#define __AMF4G_FD_PATH_H__

#include "core_errno.h"

#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf4g_fd_init(void);
CORE_DECLARE(void) amf4g_fd_final(void);

/* AMF4G Sends Authentication Information Request to HSS */
CORE_DECLARE(void) amf4g_s6a_send_air(amf4g_ue_t *amf4g_ue,
    nas_authentication_failure_parameter_t *authentication_failure_parameter);
/* AMF4G Sends Update Location Request to HSS */
CORE_DECLARE(void) amf4g_s6a_send_ulr(amf4g_ue_t *amf4g_ue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF4G_FD_PATH_H__ */

