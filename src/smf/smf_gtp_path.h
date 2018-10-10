#ifndef __SMF_PATH_H__
#define __SMF_PATH_H__

#include "gtp/gtp_xact.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) smf_gtp_open();
CORE_DECLARE(status_t) smf_gtp_close();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_PATH_H__ */