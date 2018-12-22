#ifndef __AMF_SBI_PATH_H__
#define __AMF_SBI_PATH_H__

#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define N11_SM_CONTEXT_CREATE          1
#define N11_SM_CONTEXT_UPDATE          2
#define N11_SM_CONTEXT_RELEASE         3
#define N11_SM_CONTEXT_RETRIEVE        4

CORE_DECLARE(status_t) amf_sbi_server_open();
CORE_DECLARE(status_t) amf_sbi_server_close();
CORE_DECLARE(status_t) amf_sbi_delete(sock_id sock);

CORE_DECLARE(status_t) amf_sbi_send_sm_context_create(pkbuf_t *pkbuf);
CORE_DECLARE(status_t) amf_sbi_send_sm_context_update(pkbuf_t *pkbuf);
CORE_DECLARE(status_t) amf_sbi_send_sm_context_release(pkbuf_t *pkbuf);
CORE_DECLARE(status_t) amf_sbi_send_sm_context_retrieve(pkbuf_t *pkbuf);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_SBI_PATH_H__ */

