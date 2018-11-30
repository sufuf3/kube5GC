#ifndef __AMF_JSON_HANDLER_H__
#define __AMF_JSON_HANDLER_H__

#include "mme_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) handle_create_session(const char* json_string, mme_sess_t* _sess);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_JSON_HANDLER_H__ */