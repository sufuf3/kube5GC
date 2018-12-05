#ifndef __SMF_JSON_HANDLER_H__
#define __SMF_JSON_HANDLER_H__

#include <cJSON/cJSON.h>
#include "smf_context.h"

#include "sbiJson/JsonTransform.h"
#include "sbiJson/commonJsonKey.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

status_t smf_json_handler_create_session(pkbuf_t **pkbuf, creat_session_t *createSession);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_JSON_HANDLER_H__ */