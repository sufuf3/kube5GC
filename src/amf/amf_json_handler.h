#ifndef __AMF_JSON_HANDLER_H__
#define __AMF_JSON_HANDLER_H__

#include "mme_context.h"
#include "JsonTransform.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf_json_handle_create_session(pkbuf_t **pkbuf, mme_sess_t *pSess);
CORE_DECLARE(status_t) amf_json_handler_create_session_response(pkbuf_t **pkbuf, creat_session_t *pCreateRspSess);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_JSON_HANDLER_H__ */