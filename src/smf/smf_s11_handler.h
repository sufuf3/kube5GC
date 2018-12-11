#ifndef __SMF_S11_HANDLER_H__
#define __SMF_S11_HANDLER_H__

#include "gtp/gtp_message.h"
#include "gtp/gtp_xact.h"

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) smf_s11_handle_create_session_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_create_session_request_t *req);
CORE_DECLARE(void) smf_s11_handle_delete_session_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_delete_session_request_t *req);
CORE_DECLARE(void) smf_s11_handle_modify_bearer_request(gtp_xact_t *s11_xact,
        smf_sess_t *sess, gtp_modify_bearer_request_t *req);

CORE_DECLARE(void) smf_n11_handle_create_session_request_by_JsonCreateSession(smf_sess_t *sess, creat_session_t *createSession);
CORE_DECLARE(void) smf_n11_handle_update_session_request_by_JsonUpdateSession(smf_sess_t *sess, modify_bearer_t *pModifyBearer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_N4_HANDLER_H__ */

