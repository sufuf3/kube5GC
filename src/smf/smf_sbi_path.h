#ifndef __SMF_SBI_PATH_H__
#define __SMF_SBI_PATH_H__

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) smf_sbi_server_open();
CORE_DECLARE(status_t) smf_sbi_server_close();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_SBI_PATH_H__ */

