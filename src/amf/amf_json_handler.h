#ifndef __AMF_JSON_HANDLER_H__
#define __AMF_JSON_HANDLER_H__

#include "mme_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) handle_create_session(const char* json_string, mme_sess_t* _sess);
CORE_DECLARE(status_t) amf_json_handle_create_session(pkbuf_t **pkbuf, mme_sess_t *pSess);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_JSON_HANDLER_H__ */