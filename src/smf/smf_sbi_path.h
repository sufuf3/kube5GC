#ifndef __SMF_SBI_PATH_H__
#define __SMF_SBI_PATH_H__

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define N11_TYPE_SM_CONTEXT_CREATE          1
#define N11_TYPE_SM_CONTEXT_UPDATE          2
#define N11_TYPE_SM_CONTEXT_RELEASE         3
#define N11_TYPE_SM_CONTEXT_RETRIEVE        4

CORE_DECLARE(status_t) smf_sbi_server_open();
CORE_DECLARE(status_t) smf_sbi_server_close();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_SBI_PATH_H__ */

