#ifndef __AMF_JSON_BUILD_H__
#define __AMF_JSON_BUILD_H__

#include "mme_context.h"
#include <cJSON/cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) amf_json_build_create_session(pkbuf_t **pkbuf, mme_sess_t *sess);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_JSON_BUILD_H__ */