#ifndef __AMF_JSON_HANDLER_H__
#define __AMF_JSON_HANDLER_H__

#include "amf4g_context.h"
#include "JsonTransform.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf_json_handle_create_session(pkbuf_t **pkbuf, amf4g_sess_t *pSess);
CORE_DECLARE(status_t) amf_json_handler_create_session_response(pkbuf_t **pkbuf, create_session_t *pCreateRspSess);
CORE_DECLARE(status_t) amf_json_handler_update_session_response(pkbuf_t **pkbuf, modify_bearer_t *modifyBearer);
CORE_DECLARE(status_t) amf_json_handler_delete_session_response(pkbuf_t **pkbuf, delete_session_t *deleteSession);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_JSON_HANDLER_H__ */
