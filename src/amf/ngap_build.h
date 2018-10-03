#ifndef __NGAP_BUILD_H__
#define __NGAP_BUILD_H__

#include "ngap/ngap_message.h"
#include "mme_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) ngap_build_setup_rsp(pkbuf_t **pkbuf);

CORE_DECLARE(status_t) ngap_build_setup_failure(pkbuf_t **pkbuf, NGAP_Cause_PR group, long cause, long time_to_wait);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NGAP_BUILD_H__ */
