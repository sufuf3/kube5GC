#ifndef __AMF4G_S6A_HANDLER_H__
#define __AMF4G_S6A_HANDLER_H__

#include "fd/s6a/s6a_message.h"
#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) amf4g_s6a_handle_aia(
        amf4g_ue_t *amf4g_ue, s6a_aia_message_t *aia_message);
CORE_DECLARE(void) amf4g_s6a_handle_ula(
        amf4g_ue_t *amf4g_ue, s6a_ula_message_t *ula_message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF4G_S6A_HANDLER_H__ */
