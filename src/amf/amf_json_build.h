#ifndef __AMF_JSON_BUILD_H__
#define __AMF_JSON_BUILD_H__

#include "amf4g_context.h"
#include <cJSON/cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf_json_build_create_session(pkbuf_t **pkbuf, amf4g_sess_t *sess);
CORE_DECLARE(status_t) amf_json_build_modify_bearer(pkbuf_t **pkbuf, amf4g_bearer_t *bearer, c_uint8_t sm_context_update_type);
CORE_DECLARE(status_t) amf_json_build_delete_session(pkbuf_t **pkbuf, amf4g_sess_t *sess);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_JSON_BUILD_H__ */
