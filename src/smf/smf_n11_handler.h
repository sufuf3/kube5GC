#ifndef __SMF_N11_HANDLER_H__
#define __SMF_N11_HANDLER_H__

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) smf_n11_handle_create_session_request(smf_sess_t *sess, create_session_t *createSession);
CORE_DECLARE(void) smf_n11_handle_update_session_request(smf_sess_t *sess, modify_bearer_t *pModifyBearer);
CORE_DECLARE(void) smf_n11_handle_delete_session_request(smf_sess_t *sess, delete_session_t *pDeleteSession);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_N11_HANDLER_H__ */

