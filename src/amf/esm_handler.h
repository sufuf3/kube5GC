#ifndef __ESM_HANDLER_H__
#define __ESM_HANDLER_H__

#include "nas/nas_message.h"

#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) esm_handle_pdn_connectivity_request(amf4g_bearer_t *bearer,
        nas_pdn_connectivity_request_t *pdn_connectivity_request);
CORE_DECLARE(status_t) esm_handle_information_response(amf4g_sess_t *sess, 
        nas_esm_information_response_t *bearer_information_response);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESM_HANDLER_H__ */
