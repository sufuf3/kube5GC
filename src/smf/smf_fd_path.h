#ifndef __SMF_FD_PATH_H__
#define __SMF_FD_PATH_H__

#include "core_errno.h"

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) smf_fd_init(void);
CORE_DECLARE(void) smf_fd_final(void);

CORE_DECLARE(void) smf_gx_send_ccr(smf_sess_t *sess, c_uint32_t cc_request_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_FD_PATH_H__ */

