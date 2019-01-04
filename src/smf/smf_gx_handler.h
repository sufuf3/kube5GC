#ifndef __SMF_GX_HANDLER_H__
#define __SMF_GX_HANDLER_H__

#include "fd/fd_lib.h"
#include "fd/gx/gx_message.h"

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) smf_gx_handle_cca_initial_request(
        smf_sess_t *sess, gx_message_t *gx_message);
CORE_DECLARE(void) smf_gx_handle_cca_termination_request(
        smf_sess_t *sess, gx_message_t *gx_message);
CORE_DECLARE(void) smf_gx_handle_re_auth_request(
        smf_sess_t *sess, gx_message_t *gx_message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_GX_HANDLER_H__ */
