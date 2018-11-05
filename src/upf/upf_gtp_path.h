#ifndef __UPF_PATH_H__
#define __UPF_PATH_H__

#include "gtp/gtp_xact.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) upf_gtp_open();
CORE_DECLARE(status_t) upf_gtp_close();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UPF_PATH_H__ */
