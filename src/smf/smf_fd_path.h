#ifndef __PGW_FD_PATH_H__
#define __PGW_FD_PATH_H__

#include "core_errno.h"

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _gtp_xact_t gtp_xact_t;

CORE_DECLARE(status_t) smf_fd_init(void);
CORE_DECLARE(void) smf_fd_final(void);

CORE_DECLARE(void) smf_gx_send_ccr(smf_sess_t *sess, c_uint32_t cc_request_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PGW_FD_PATH_H__ */

