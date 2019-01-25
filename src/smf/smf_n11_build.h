#ifndef __SMF_N11_BUILD_H__
#define __SMF_N11_BUILD_H__

#include "JsonTransform.h"

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) smf_n11_build_create_session_response(
        pkbuf_t **pkbuf, smf_sess_t *sess);
        
CORE_DECLARE(status_t) smf_n11_build_update_session_response(
        pkbuf_t **pkbuf, smf_sess_t *sess);

CORE_DECLARE(status_t) smf_n11_build_delete_session_response(
        pkbuf_t **pkbuf, smf_sess_t *sess);

status_t smf_n11_build_update_session_downlink_data_notification_request(
    pkbuf_t **pkbuf, smf_sess_t *sess);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_N11_BUILD_H__ */

